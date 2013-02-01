#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FORK_FAILURE 1
#define QTY_FAILURE 2
#define WAIT_FAILURE 3

void set_envvar(char* var, int amount)
{
  char value[4];
  sprintf(value, "%d", amount);
  setenv(var, value, 1);
}

int get_envvar(char* var)
{
  char *value;
  value = getenv(var);
  return atoi(value);
}

void dec_envvar()
{
  set_envvar("PAPER", get_envvar("PAPER") - 1);
  set_envvar("TOBACCO", get_envvar("TOBACCO") - 1);
  set_envvar("MATCHES", get_envvar("MATCHES") - 1);
}

void setup_envvars(int p, int t, int m)
{
  set_envvar("PAPER", p);
  set_envvar("TOBACCO", t);
  set_envvar("MATCHES", m);
  printf("SETTING UP QTYS\n");
  printf("PAPER:   %d\n", get_envvar("PAPER"));
  printf("TOBACCO: %d\n", get_envvar("TOBACCO"));
  printf("MATCHES: %d\n", get_envvar("MATCHES"));
  printf("\n");
}

pid_t checked_fork()
{
  pid_t pid = fork();

  if(pid < 0) {
    perror("fork() failed");
    exit(FORK_FAILURE);
  } else {
    return pid;
  }
}

void checked_wait(pid_t pid)
{
  int status;
  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid() failed");
    exit(WAIT_FAILURE);
  } else if (WEXITSTATUS(status) != 0) {
    fprintf(stderr, "Smoker (pid %d) failed (%d)\n", pid, WEXITSTATUS(status));
  }
}

void print_proc_info()
{
  char name[256];
  gethostname(name, sizeof(name));
  
  time_t curtime = time(NULL);
  struct tm *nowtm;
  nowtm = localtime(&curtime);
  char timebuffer[256];
  strftime(timebuffer, sizeof(timebuffer), "%H:%M:%S", nowtm);

  printf("AGENT INFO\n");
  printf("HOST:   %s\n", name);
  printf("PID:    %d\n", getpid());
  printf("UID:    %d\n", getuid());
  printf("TIME:   %s\n", timebuffer);
  printf("\n"); 
}

int smoke(int smoker)
{
  printf("S%d (PID %d):\n", smoker, getpid()); fflush(stdout);
  
  if(get_envvar("PAPER") > 0) {
    printf("PAPER IS AVAILABLE! P: %d\n", get_envvar("PAPER")); fflush(stdout);
  } else {
    printf("NO PAPER!\n"); fflush(stdout); 
    exit(QTY_FAILURE);
  }

  if(get_envvar("TOBACCO") > 0) {
    printf("TOBACCO IS AVAILABLE! T: %d\n", get_envvar("TOBACCO")); fflush(stdout);
  } else {
    printf("NO TOBACCO!\n"); fflush(stdout);
    exit(QTY_FAILURE);
  }

  if(get_envvar("MATCHES") > 0) {
    printf("MATCHES ARE AVAILABLE! M: %d\n", get_envvar("MATCHES")); fflush(stdout);
  } else {
    printf("NO MATCHES!\n"); fflush(stdout); 
    exit(QTY_FAILURE);
  }

  printf("SMOKING!\n\n"); fflush(stdout);
  return 0;
}

void chain_smoke()
{
  pid_t s1, s2, s3;
  while(get_envvar("PAPER") > 0 && get_envvar("TOBACCO") > 0 && get_envvar("MATCHES") > 0) {
    s1 = checked_fork();
    if(s1 == 0) {
      _exit(smoke(1));
    } else {
      checked_wait(s1);
      dec_envvar();
      s2 = checked_fork();
      if(s2 == 0) {
	sleep(2);
	_exit(smoke(2));
      } else {
	checked_wait(s2);
	dec_envvar();
	s3 = checked_fork();
	if(s3 == 0) {
	  sleep(4);
	  _exit(smoke(3));
	} else {
      	  checked_wait(s3);
	  dec_envvar();
	}
      }
    }
  }
}

int main(int argc, char **argv)
{
  print_proc_info();

  setup_envvars(2, 2, 2); fflush(stdout);
  chain_smoke();    
  
  setup_envvars(1, 2, 3); fflush(stdout);
  chain_smoke();

  setup_envvars(3, 2, 2); fflush(stdout);
  chain_smoke();

  return 0;
}
