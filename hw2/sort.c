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

#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2

#define BUFFER_SIZE 1024

char* getbytes()
{
  char buffer[BUFFER_SIZE];
  
  size_t contentSize = 1;
  char *content = malloc(sizeof(char)*BUFFER_SIZE);
  content[0] = '\0';

  if(content == NULL) {
    perror("Failed to allocate content");
    exit(ALLOC_ERROR);
  }

  while(fgets(buffer, BUFFER_SIZE, stdin)) {
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

    dup2(inpipe[0], STDIN_FD);
    dup2(outpipe[1], STDOUT_FD);
    dup2(outpipe[1], STDERR_FD);
    
    close(inpipe[1]);
    close(outpipe[0]);

    execl("./sort", "", (char*) NULL);
  } else {
    close(inpipe[0]);
    close(outpipe[1]);
  }
}

int main(int argc, char** argv)
{
  char *content = getbytes();
  printf("Content: %s\n", content);

  int len = strlen(content);
 
  if(len <= 1) {
    printf("Final level: %s\n", content);
  }

  int linpipe[2], loutpipe[2];
  split(linpipe, loutpipe);
  write(linpipe[1], content, len);
  close(linpipe[1]);
  
  int rinpipe[2], routpipe[2];
  split(rinpipe, routpipe);
  write(rinpipe[1], &content[len/2+1], (len-len/2));
  close(rinpipe[1]);
  

  

  return 0;
}
