#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void checked_fork()
{
}

void checked_wait()
{
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

void smoke()
{
}

int main(int argc, char **argv)
{
  print_proc_info();
  setup_envvars(2, 2, 2); fflush(stdout);
  
  return 0;
}
