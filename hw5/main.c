#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "semaphore.h"

/* Function to sleep for a random amount ms on range [0 to max_in_ms).
   NOTE: This function is not thread-safe.

   @input int max_in_ms: The max amount of time to sleep in millisecs
*/
void fork_sleep(int max_in_ms)
{
    struct timespec time_ns;
    int duration = random() % max_in_ms + 1;
    time_ns.tv_sec = 0;
    time_ns.tv_nsec = duration * 1000000;
    if(nanosleep(&time_ns, NULL) != 0) {
	perror("Sleep Error");
	exit(EXIT_FAILURE);
    }
}

/* Decides to start an oxygen or hydrogen processes.
   
   @input int hydrogen_left: Number of hydrogen procs left to start for N bonds.
   @input int oxygen_left: Number of oxygen procs left to start for N bonds.

   @output: Numbers 0 (for oxygen) or 1 (for hydrogen) depending.
            If there are no amount of a particular process left this function
            will always pick the other one. If there are no processes left to
            run this function will return -1 to signal there are no more procs.
*/
int start_random(int hydrogen_left, int oxygen_left) {
    int value;
    
    if(hydrogen_left > 0 && oxygen_left > 0) {
	value = rand() / (RAND_MAX / 2 + 1); // Generates an int on [0, 1]
    } else if(hydrogen_left == 0 && oxygen_left > 0) {
	value = 0;
    } else if(hydrogen_left > 0 && oxygen_left == 0) {
	value = 1;
    } else {
	value = -1;
    }

    return value;
}

/* MAIN FUNCTION
   This function is responsible for starting an arbitrary number of bonds.
   main() with start the bonds, randomly, and then wait for them to finish.
   Once all the bonds are completed this function will free-up any shared
   semaphores and segments of memory.

   @input: The program takes an additional arguement for the number of
           bonds to create and exits if no amount is given.
	   example: ./main 10 -- This will try to run through 10 bonds.
*/
int main(int argc, char** argv)
{
    int total_bonds = 0;
    
    if(argc != 2) {
	printf("USAGE: ./main num_of_bonds\n");
	exit(EXIT_FAILURE);
    } else {
	total_bonds = atoi(argv[1]);
    }
    
    // The next few sections setup semaphores and shared memory
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

    // Start the actual processes and the bonding actions
    pid_t pid;
    int hydrogen_left = total_bonds*2;
    int oxygen_left = total_bonds;
    int to_start;
    srandom(time(NULL));
        
    for(int i = 0; i < total_bonds*3; i++) {
	to_start = start_random(hydrogen_left, oxygen_left);
	if(to_start == 0) {
	    oxygen_left--;
	    pid = fork();
	    if(pid < 0) { //Error check
		perror("Error: forking");
		exit(EXIT_FAILURE);
	    } else if(pid == 0) { // If child fork/exec
		printf("PID %d: Starting Oxygen Proc.\n", getpid());
		execl("./oxygen", "", (char*) NULL);
	    } else {
		fork_sleep(1000); // If this is the parent sleep for a bit
	    }
	} else if(to_start == 1) {
	    hydrogen_left--;
	    pid = fork();
	    if(pid < 0) { // Error check
		perror("Error: forking");
		exit(EXIT_FAILURE);
	    } else if(pid == 0) { // If child fork/exec
		printf("PID %d: Starting Hydrogen Proc.\n", getpid());
		execl("./hydrogen", "", (char*) NULL);
	    } else {
		fork_sleep(1000); // If this is the parent sleep for a bit
	    }
	
	} else {
	    printf("No more processes to run!\n");
	}
    }

    //Wait for the processes to exit to start the shutdown
    wait(NULL);
    sleep(1);

    printf("Clearing all shared resources \n");
    
    // Clear SYS-V shared segments
    semaphore_clear(sem_key);
    shared_memory_clear(shm_key);

    return 0;
}
