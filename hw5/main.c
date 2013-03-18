#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "semaphore.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    int sem_key = semaphore_key(4);
    int shm_key = shared_memory_create(sizeof(data_t));
    data_t* data = (data_t*)shared_memory_addr(shm_key);

    data->oxygenCount = 0;
    data->hydrogenCount = 0;
    data->barrierCount = 0;

    semaphore_initval(MUTEX, sem_key, 1);
    semaphore_initval(OSEM, sem_key, 0);
    semaphore_initval(HSEM, sem_key, 0);
    semaphore_initval(BSEM, sem_key, 1);

    pid_t pid1, pid2, pid3, pid4, pid5;
    
    if((pid1=fork()) == 0) {
	execl("./oxygen", "", (char*) NULL);
    } else if((pid2=fork()) == 0) {
	execl("./hydrogen", "", (char*) NULL);
    } else if((pid3=fork()) == 0) {
	execl("./hydrogen", "", (char*) NULL);
    } else if((pid4=fork()) == 0) {
	execl("./oxygen", "", (char*) NULL);
    } else if((pid5=fork()) ==0) {
	execl("./oxygen", "", (char*) NULL);
    }

    wait(NULL);
    sleep(1);
    
    semaphore_clear(sem_key);
    shared_memory_clear(shm_key);

    return 0;
}
