#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "semaphore.h"

void semaphore_wait(int semaphore_id, unsigned short semaphore_key)
{
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_op = -1;
    semaphore_buffer.sem_flg = 0;
    semaphore_buffer.sem_num = semaphore_id;
    if(semop(semaphore_key, &semaphore_buffer, 1) < 0) {
	perror("Failed to Wait");
	exit(WAIT_FAILURE);
    }
}

void semaphore_signal(int semaphore_id, unsigned short semaphore_key)
{
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_op = 1;
    semaphore_buffer.sem_flg = 0;
    semaphore_buffer.sem_num = semaphore_id;
    if(semop(semaphore_key, &semaphore_buffer, 1) < 0) {
	perror("Failed to Signal");
	exit(SIGNAL_FAILURE);
    }
}

key_t generate_ipc_key()
{
    key_t ipc_key = ftok(__FILE__, SEMKEY);
     if(ipc_key < 0) {
	perror("Failed to Generate IPC Key");
	exit(IPC_FAILURE);
    }
     
     return ipc_key;
}

int semaphore_create(int semaphore_num)
{
    int semaphore_id = semget(generate_ipc_key(), semaphore_num, 0666|IPC_CREAT);
    if(semaphore_id < 0) {
	perror("Failed to Create Semaphore");
	exit(CREATE_FAILURE);
    }
    
    return semaphore_id;
}

void semaphore_initval(int semaphore_id, int semaphore_offset, int value)
{
    if(semctl(semaphore_id, semaphore_offset, SETVAL, value) == -1) {
	perror("Failed to Set Initial Value");
	exit(INIT_FAILURE);
    }
}

void semaphore_clear(int semaphore_id, int semaphore_offset)
{
    if(semctl(semaphore_id, semaphore_offset, IPC_RMID, 0) == -1) {
	perror("Failed to Remove Semaphore");
	exit(CLEAR_FAILURE);
    }
}
