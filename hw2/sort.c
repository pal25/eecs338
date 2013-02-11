#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ALLOC_ERROR 1
#define REALLOC_ERROR 2
#define PIPE_ERROR 3
#define FORK_ERROR 4
#define EXEC_ERROR 5

#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2
#define WRITE_END 1
#define READ_END 0

#define BUFFER_SIZE 256

char* getbytes(int fd)
{
  char buffer[BUFFER_SIZE];
  FILE *stream = fdopen(fd, "r");
 
  size_t contentSize = 1;
  char *content = calloc(BUFFER_SIZE, sizeof(char));
 
  if(content == NULL) {
    perror("Failed to allocate content");
    exit(ALLOC_ERROR);
  }

  while(fgets(buffer, BUFFER_SIZE, stream)) {
    char* old = content;
    contentSize += strlen(buffer);
    content = realloc(content, contentSize);
    
    if(content == NULL) {
      perror("Failed to reallocate content");
      free(old);
      exit(REALLOC_ERROR);
    }

    strcat(content, buffer);
    content = strtok(content, "\n");
  }

  return content;
}


void checked_pipe(int *pipefd)
{
  if(pipe(pipefd)) {
    perror("Failed to create pipe");
    exit(PIPE_ERROR);
  }
}

pid_t checked_fork()
{
  pid_t pid = fork();
  if(pid < 0) {
    perror("Failed to fork");
    exit(FORK_ERROR);
  }

  return pid;
}

void split_workers(int *inpipe, int *outpipe)
{
  checked_pipe(inpipe);
  checked_pipe(outpipe);
  pid_t child = checked_fork();
  if(child == 0) {
    dup2(inpipe[READ_END], STDIN_FD);   // Make sure the new proc can read
    dup2(outpipe[WRITE_END], STDOUT_FD); // Make sure the new proc can write
    dup2(outpipe[WRITE_END], STDERR_FD); // Just in case make sure new proc can err
    
    close(inpipe[WRITE_END]);  // Proc wont write to the inpipe
    close(outpipe[READ_END]); // Proc wont read from the outpipe

    execl("./sort", "", (char*) NULL); 
    exit(EXEC_ERROR);
  } else {
    close(inpipe[READ_END]);  // The parent wont read from inpipe
    close(outpipe[WRITE_END]); // Parent wont write from outpipe
  }
}

void merge(char *out, char *m1, char *m2)
{
  int m1_len = strlen(m1);
  int m2_len = strlen(m2);
  
  int m1_ptr = 0;
  int m2_ptr = 0;
  int out_ptr = 0;

  while(out_ptr < m1_len + m2_len) {
    if(m1_ptr >= strlen(m1)) {
      out[out_ptr] = m2[m2_ptr];
      m2_ptr++;
    } else if (m2_ptr >= strlen(m2)) {
      out[out_ptr] = m1[m1_ptr];
      m1_ptr++;
    } else if(m1[m1_ptr] < m2[m2_ptr]) {
      out[out_ptr] = m1[m1_ptr];
      m1_ptr++;
    } else {
      out[out_ptr] = m2[m2_ptr];
      m2_ptr++;
    }
    out_ptr++;
  }

  out[out_ptr] = '\0';

  FILE* dbg_fd = fopen("./debug", "a");
  fprintf(dbg_fd, "Merge Content(PID=%d): Left=%s, Right=%s, Merged=%s\n", getpid(), m1, m2, out);
  fclose(dbg_fd);

}

void split_array(char* content, char* left, char* right)
{
  int len = strlen(content);

  strncpy(left, content, len/2);
  left[len/2] = '\0';
  strncpy(right, content + len/2, len);

  FILE* dbg_fd = fopen("./debug", "a");
  fprintf(dbg_fd, "Split Content(PID=%d): Left=%s, Right=%s, Original=%s\n", getpid(), left, right, content);
  fclose(dbg_fd);
   
}

int main(int argc, char** argv)
{  
  char *input = getbytes(STDIN_FD);
  int len = strlen(input);
 
  if(len <= 1) {
    FILE* dbg_fd = fopen("./debug", "a");
    fprintf(dbg_fd, "Base Content(PID=%d): %s\n", getpid(), input);
    fclose(dbg_fd);
    fprintf(stdout, "%s", input); //Send char back up the pipe
  } else {
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    split_array(input, left, right);
   
    int linpipe[2], loutpipe[2];
    split_workers(linpipe, loutpipe); // Create a new proc to send to
    write(linpipe[WRITE_END], left, strlen(left));
    close(linpipe[WRITE_END]);
    char* left_content = getbytes(loutpipe[READ_END]);
    close(loutpipe[READ_END]);
   
    int rinpipe[2], routpipe[2];
    split_workers(rinpipe, routpipe); // Create a new proc to send to
    write(rinpipe[WRITE_END], right, strlen(right));
    close(rinpipe[WRITE_END]);
    char* right_content = getbytes(routpipe[READ_END]);
    close(routpipe[READ_END]);

    char output[BUFFER_SIZE];
    merge(output, left_content, right_content);
    FILE* dbg_fd = fopen("./debug", "a");
    fprintf(dbg_fd, "Writing Content(PID=%d): %s\n", getpid(), output);
    fclose(dbg_fd);    
    fprintf(stdout, "%s", output);
  }

  return 0;
}
