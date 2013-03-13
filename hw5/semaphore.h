#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/ipc.h>
#include <sys/types.h>

#define KEY 25

#define MUTEX 0
#define OSEM 1
#define HSEM 2
#define BSEM 3

#define WAIT_FAILURE 1
#define SIGNAL_FAILURE 2
#define CREATE_FAILURE 3
#define IPC_FAILURE 4
#define INIT_FAILURE 5
#define CLEAR_FAILURE 6
#define MAP_FAILURE 7

typedef struct {
    int hydrogenCount;
    int oxygenCount;
    int barrierCount;
} data_t;

void semaphore_wait(int semaphore_id, unsigned short semaphore_key);
void semaphore_signal(int semaphore_id, unsigned short semaphore_key);
key_t generate_ipc_key();
int semaphore_key(int semaphore_num); 
void semaphore_initval(int semaphore_id, int semaphore_offset, int value);
void semaphore_clear(int semaphore_id, int semaphore_offset);

int shared_memory_key(size_t memory_size);
void* shared_memory_addr(int shm_key);
void shared_memory_clear(int shm_key);
#endif
