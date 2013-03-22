#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "semaphore.h"

/* This function initiates a wait() on a particular semaphore.
   The semaphore to use is based on the key and the offset stored
   in that key. If an EIDRM error flag is given then all processes
   waiting to bond will exit with a message notifying the user that
   these remaining processes will be unable to bond. In the case of
   any other error it will exit with a WAIT_FAILURE error code.

   @input semaphore_offset: The offset in the sembuf struct while holds
                            the value of the semaphore we wish to wait for.
   @input semaphore_key: This is the key that determines which semaphore to use.
                         For this purpose there is only one semaphore structure. 
*/
void semaphore_wait(int semaphore_offset, int semaphore_key)
{
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_op = -1;
    semaphore_buffer.sem_flg = 0;
    semaphore_buffer.sem_num = semaphore_offset;
    if(semop(semaphore_key, &semaphore_buffer, 1) < 0) {
	if(errno == EIDRM) {
	    printf("Exiting process w/ PID: %d. Will never bond.\n", getpid());
	    exit(INFINITE_WAIT);
	} else {
	    perror("Failed to Wait");
	    exit(WAIT_FAILURE);
	}
    }
}

/* This function initiates a signal() on a particular semaphore.
   The semaphore to use is based on the key and the offset stored
   in that key. In the case of an error it will exit with a
   SIGNAL_FAILURE error code.

   @input semaphore_offset: The offset in the sembuf struct while holds
                            the value of the semaphore we wish to wait for.
   @input semaphore_key: This is the key that determines which semaphore to use.
                         For this purpose there is only one semaphore structure. 
*/
void semaphore_signal(int semaphore_offset, int semaphore_key)
{
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_op = 1;
    semaphore_buffer.sem_flg = 0;
    semaphore_buffer.sem_num = semaphore_offset;
    if(semop(semaphore_key, &semaphore_buffer, 1) < 0) {
	perror("Failed to Signal");
	exit(SIGNAL_FAILURE);
    }
}

/* This function generates an ipc_key which is guaranteed to be the
   same for any processes that is started from the main processes.
   NOTE: This function is for internal use only by other functions
   in this file.
   
   @output key_t: The output is of type key_t and can be used to
                  generate a semaphore and shared memory key.
*/
key_t generate_ipc_key()
{
    key_t ipc_key = ftok(__FILE__, KEY);
    if(ipc_key < 0) {
    	perror("Failed to Generate IPC Key");
    	exit(IPC_FAILURE);
    }
     
     return ipc_key;
}

/* This function will return a semaphore key given the number of
   semaphores total. It relies on an ipc_key which will only be
   unique if generated from the main processes. In addition if
   the group of semaphores has not been created yet this function
   will create the semaphore group with permissions 666. On an
   error this process will exit with error code CREATE_FAILURE.

   @input semaphore_num: This is an integer that is the TOTAL number
                         of semaphores for a given group/ipc_key

   @output: This function returns a semaphore key that can be used
            to perform operations on the semaphore group represented
	    by any given semaphore key.
*/
int semaphore_key(int semaphore_num)
{
    int semaphore_id = semget(generate_ipc_key(), semaphore_num, IPC_CREAT | 0666);
    if(semaphore_id < 0) {
	perror("Failed to Create Semaphore");
	exit(CREATE_FAILURE);
    }
    
    return semaphore_id;
}

/* This function is used for initializing a semaphore to a particular value.
   This function will only work for non-binary semaphores as no error is
   given in the case of initializing a binary semaphore to more than 1.
   In the case of an error exit will be called with an INIT_FAILURE code.

   @input semaphore_offset: The offset in the sembuf struct while holds
                            the value of the semaphore we wish to wait for.
   @input semaphore_key: This is the key that determines which semaphore to use.
                         For this purpose there is only one semaphore structure. 
   @input value: This parameter specifies the initial value to be given to the
                 semaphore at the semaphore_offset.
*/
void semaphore_initval(int semaphore_offset, int semaphore_key, int value)
{
    if(semctl(semaphore_key, semaphore_offset, SETVAL, value) == -1) {
	perror("Failed to Set Initial Value");
	exit(INIT_FAILURE);
    }
}

/* This function will remove a *group* or semaphores represented
   by a particular semaphore key. On error this will exit with
   an error code of CLEAR_FAILURE.

   @input semaphore_key: This is the key that determines which semaphore to use.
                         For this purpose there is only one semaphore structure. 
*/
void semaphore_clear(int semaphore_key)
{
    if(semctl(semaphore_key, 0, IPC_RMID, NULL) == -1) {
	perror("Failed to Remove Semaphore");
	exit(CLEAR_FAILURE);
    }
}

/* This function will create a chunk of shared memory of a size
   specified by the functions input. The shared memory segment
   will be created with permissions set to 666. In the event of
   an error this will exit with error code CREATE_FAILURE

   @input memory_size: This is the size of the block of shared
                       memory to be created.

   @output: The output will be a shared memory key that represents
            a particular shared memory segment.
*/
int shared_memory_create(size_t memory_size)
{
    int shm_key = shmget(generate_ipc_key(), memory_size, IPC_CREAT | 0666);
    if(shm_key < 0) {
	perror("Failed to Create Shared Memory Key");
	exit(CREATE_FAILURE);
    }
    
    return shm_key;
}

/* This function will return a shared memory key. The shared memory
   segment will be created with permissions set to 666. It relies
   on an ipc_key which will only be unique if generated from the
   main processes. In the event of an error this will exit with error
   code CREATE_FAILURE. NOTE: This function will NOT create a new
   shared memory segment.

   @input memory_size: This is the size of the block of shared
                       memory to be created.

   @output: The output will be a shared memory key that represents
            a particular shared memory segment.
*/
int shared_memory_key(size_t memory_size)
{
    int shm_key = shmget(generate_ipc_key(), memory_size, 0666);
    if(shm_key < 0) {
	perror("Failed to Get Shared Memory Key");
	exit(CREATE_FAILURE);
    }
    
    return shm_key;
}

/* This function returns the address of the shared memory segment.
   The functions input is a key to the particular segment to be
   accessed. On an error this will exit with error code MAP_FAILURE.

   @input shm_key: This is the shared memory key that represents
                   a particular chuck of shared memory.

   @output: A void* pointer to the memory location of the shared
            memory segment.
*/
void* shared_memory_addr(int shm_key)
{
    void* addr = shmat(shm_key, NULL, 0);
    if((int*)addr < 0) {
	perror("Failed to Map Shared Memory");
	exit(MAP_FAILURE);
    }
    
    return addr;
}

/* This function will clear a chunk of shared memory that is
   mapped with the smared memory key. This will clear ALL of
   the memory space within the pre-specified size specifed
   when the shared memory segment was initialized/created.
   In the case of an error this will exit with error code
   CLEAR_FAILURE.

   @input shm_key: This is the shared memory key that represents
                   a particular chuck of shared memory.   
*/  
void shared_memory_clear(int shm_key)
{
    if(shmctl(shm_key, IPC_RMID, NULL) == -1) {
	perror("Failed to Remove Shared Memory");
	exit(CLEAR_FAILURE);
    }
}
