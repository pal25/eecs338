#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "semaphore.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    int sem_key = semaphore_key(4);
    int shm_key = shared_memory_create(sizeof(data_t));
    printf("1\n");
    data_t* data = (data_t*)shared_memory_addr(shm_key);
    printf("2\n");

    data->oxygenCount = 0;
    data->hydrogenCount = 0;
    data->barrierCount = 0;
    printf("3\n");

    semaphore_initval(MUTEX, sem_key, 1);
    semaphore_initval(OSEM, sem_key, 0);
    semaphore_initval(HSEM, sem_key, 0);
    semaphore_initval(BSEM, sem_key, 1);
    printf("4\n");

    pid_t pid1, pid2, pid3, pid4;
    printf("5\n");
    
    if((pid1=fork()) == 0) {
	execl("./oxygen", "", (char*) NULL);
    } else if((pid2=fork()) == 0) {
	execl("./hydrogen", "", (char*) NULL);
    } else if((pid3=fork()) == 0) {
	execl("./hydrogen", "", (char*) NULL);
    } else if((pid4=fork()) == 0) {
	execl("./oxygen", "", (char*) NULL);
    }
    printf("6\n");

    wait(0);
    sleep(2);
    
    semaphore_clear(MUTEX, sem_key);
    semaphore_clear(OSEM, sem_key);
    semaphore_clear(HSEM, sem_key);
    semaphore_clear(BSEM, sem_key);
    shared_memory_clear(shm_key);
    
    return 0;
}
