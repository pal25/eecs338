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

#define BUFFER_SIZE 1024

char* getbytes(int fd)
{
  char buffer[BUFFER_SIZE];
  FILE *stream = fdopen(fd, "r");
 
  size_t contentSize = 1;
  char *content = malloc(sizeof(char)*BUFFER_SIZE);
  content[0] = '\0';

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

void split(int *inpipe, int *outpipe)
{
  checked_pipe(inpipe);
  checked_pipe(outpipe);
  pid_t child = checked_fork();
  if(child == 0) {
    printf("CHILD: %d\n", getpid());

    dup2(inpipe[0], STDIN_FD);   // Make sure the new proc can read
    dup2(outpipe[1], STDOUT_FD); // Make sure the new proc can write
    dup2(outpipe[1], STDERR_FD); // Just in case make sure new proc can err
    
    close(inpipe[1]);  // Proc wont write to the inpipe
    close(outpipe[0]); // Proc wont read from the outpipe

    execl("./sort", "", (char*) NULL); 
    exit(EXEC_ERROR);
  } else {
    close(inpipe[0]);  // The parent wont read from inpipe
    close(outpipe[1]); // Parent wont write from outpipe
  }
}

int main(int argc, char** argv)
{
  char *content = getbytes(STDIN_FD);
  printf("Content: %s\n", content);
  int len = strlen(content);
 
  if(len <= 1) {
    fprintf(stdin, "%s", content);
  } else {
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    strncpy(left, content, len/2);
    left[len/2] = '\0';
    strncpy(right, content + len/2, (len - len/2));
    printf("LEFT(LEN=%d): %s\n", strlen(left), left);
    //printf("RIGHT(LEN=%d): %s\n", strlen(right), right);

    int linpipe[2], loutpipe[2];
    split(linpipe, loutpipe);
    write(linpipe[1], left, strlen(left));
    close(linpipe[1]);
   
    //int rinpipe[2], routpipe[2];
    //split(rinpipe, routpipe);
    //write(rinpipe[1], right, strlen(right));
    //close(rinpipe[1]);
    
    char* left_content = getbytes(loutpipe[0]);
    close(loutpipe[0]);

    //char* right_content = getbytes(routpipe[0]);
    // close(loutpipe[0]);
    
    char output[BUFFER_SIZE];
    strcat(output, left_content);
    //strcat(output, right_content);
    write(STDOUT_FD, content, strlen(content));
  }

  return 0;
}
