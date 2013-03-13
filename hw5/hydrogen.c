#include <stdio.h>
#include <unistd.h>
#include "semaphore.h"

int main(int argc, char** argv)
{
    int sem_key = semaphore_key(4);
    int shm_key = shared_memory_key(BUFFER);

    data_t* data = (data_t*)shared_memory_addr(shm_key);

    semaphore_wait(MUTEX, sem_key);
    if(data->hydrogenCount >= 1 && data->oxygenCount >= 1) {
	data->barrierCount = 2;
	semaphore_signal(HSEM, sem_key);
	semaphore_signal(OSEM, sem_key);	
    } else {
	data->hydrogenCount++;
	semaphore_signal(MUTEX, sem_key);
	semaphore_wait(HSEM, sem_key);
	semaphore_wait(BSEM, sem_key);
	data->hydrogenCount--;
	data->barrierCount--;
	if(data->barrierCount != 0) {
	    semaphore_signal(BSEM, sem_key);
	} else {
	    semaphore_signal(BSEM, sem_key);
	    semaphore_signal(MUTEX, sem_key);
	}
    }
   
    printf("Bonding a Hydrogen! PID: %d\n", getpid());
    
    return 0;
}
