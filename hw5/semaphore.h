#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/ipc.h>
#include <sys/types.h>

#define SEMKEY 25

#define WAIT_FAILURE 1
#define SIGNAL_FAILURE 2
#define CREATE_FAILURE 3
#define IPC_FAILURE 4
#define INIT_FAILURE 5
#define CLEAR_FAILURE 6

void semaphore_wait(int semaphore_id, unsigned short semaphore_key);
void semaphore_signal(int semaphore_id, unsigned short semaphore_key);
key_t generate_ipc_key();
int semaphore_create(int semaphore_num); 
void semaphore_initval(int semaphore_id, int semaphore_offset, int value);
void semaphore_clear(int semaphore_id, int semaphore_offset);

#endif
