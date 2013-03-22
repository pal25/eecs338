#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/ipc.h>
#include <sys/types.h>

#define KEY 'A' // Define the key to use to generate shared segments

// Define the integer indices of the semaphores
#define MUTEX 0
#define OSEM 1
#define HSEM 2
#define BSEM 3

// Define error codes for potential ways the program can fail.
#define WAIT_FAILURE 1
#define SIGNAL_FAILURE 2
#define CREATE_FAILURE 3
#define IPC_FAILURE 4
#define INIT_FAILURE 5
#define CLEAR_FAILURE 6
#define MAP_FAILURE 7
#define INFINITE_WAIT 8

// Internal struct that is stored in shared memory. 
typedef struct {
    int hydrogenCount;
    int oxygenCount;
    int barrierCount;
} data_t;


//NOTE: All below functions are implemented in semaphore.c
// Wrapper function for generating an ipc_key.
key_t generate_ipc_key();

// Wrapper functions dealing with semaphores
void semaphore_wait(int semaphore_offset, int semaphore_key);
void semaphore_signal(int semaphore_offset, int semaphore_key);
int semaphore_key(int semaphore_num); 
void semaphore_initval(int semaphore_offset, int semaphore_key, int value);
void semaphore_clear(int semaphore_key);

// Wrapper functions dealing with shared memory
int shared_memory_create(size_t memory_size);
int shared_memory_key(size_t memory_size);
void* shared_memory_addr(int shm_key);
void shared_memory_clear(int shm_key);
#endif
