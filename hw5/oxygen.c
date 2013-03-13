#include <stdio.h>
#include <unistd.h>
#include "semaphore.h"

int main(int argc, char** argv)
{
    int sem_key = semaphore_key(4);
    int shm_key = shared_memory_key(BUFFER);

    data_t* data = (data_t*)shared_memory_addr(shm_key);

    semaphore_wait(MUTEX, sem_key);
    if(data->hydrogenCount >= 2) {
	data->barrierCount = 2;
	semaphore_signal(HSEM, sem_key);
	semaphore_signal(HSEM, sem_key);	
    } else {
	data->oxygenCount++;
	semaphore_signal(MUTEX, sem_key);
	semaphore_wait(OSEM, sem_key);
	semaphore_wait(BSEM, sem_key);
	data->oxygenCount--;
	data->barrierCount--;
	if(data->barrierCount != 0) {
	    semaphore_signal(BSEM, sem_key);
	} else {
	    semaphore_signal(BSEM, sem_key);
	    semaphore_signal(MUTEX, sem_key);
	}
    }
   
    printf("Bonding a Oxygen! PID: %d\n", getpid());

    return 0;
}
