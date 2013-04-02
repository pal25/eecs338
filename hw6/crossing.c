#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define THREAD_SHARED 0

// Global Variables for pthreads
sem_t mutex, toB, toA, print;
int xingCount = 0;
int xedCount = 0;
int toAWaitCount = 0;
int toBWaitCount = 0;
int* ids;
typedef enum {None, AtoB, BtoA} dir_t;
dir_t xingDirection = None;


/* Function to sleep for a random amount of ms on range [0, max_in_ms).

   NOTE: This function is not thread-safe however it doesn't really
         matter much for this purpose. We just want to sleep for a
	 "seemingly random" amount of time.

   @input unsigned int max_in_ms: The max amount of time to sleep in millisecs
*/
void thread_sleep(unsigned int max_in_ms)
{
    useconds_t duration = (random() % max_in_ms + 1) * 1000;
    if(usleep(duration) != 0) {
	perror("Sleep Error");
	exit(EXIT_FAILURE);
    }	
}

/* This function takes a status, a thread type, and a thread id.
   It then pretty prints out this information time-stampped.

   @input char* status: The status message to display
   @input char* type: The type of thread, for our purposed "A->B" or "B->A"
   @input void* id: A numerical id that should be type cast to int*, it is
                    void* by default because that is that the defualt
		    pthreads use as arguments.

*/
void printThreadInfo(char* status, char* type, void* id)
{
    sem_wait(&print);
    time_t rawtime;
    struct tm* timeinfo;
    char buffer1[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer1, 80, "%H:%M:%S", timeinfo);

    int* val = (int*)id;

    char buffer2[256];
    sprintf(buffer2, "%s (xingCount=%d, xedCount=%d)", status, xingCount, xedCount);
    
    printf("%3d | %s | %s | %s\n", *val, type, buffer1, buffer2);

    sem_post(&print);
}

/* This function handles the logic involved with A->B threads crossing.
   This function is one of two functions that the threads run. The other
   function is a mirrored version of this function named "b_to_a_cross()"
   which has almost indentical functionality.

   @input void* pthread_id: A void* cast of int* that contains the "thread id"
                            to the thread invoking this function.

*/
void* a_to_b_cross(void* pthread_id)
{
    printThreadInfo("Dont Want to Cross Yet", "A -> B", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Wants to Cross ", "A -> B", pthread_id);
    
    sem_wait(&mutex);
    if((xingDirection == AtoB || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	printThreadInfo("Can Cross", "A -> B", pthread_id);
	xingDirection = AtoB;
	xingCount++;
	sem_post(&mutex);
    } else {
	printThreadInfo("Waiting to Cross", "A -> B", pthread_id);
	toBWaitCount++;
	sem_post(&mutex);
	sem_wait(&toB);
	toBWaitCount--;
	xingCount++;
	xingDirection = AtoB;
	sem_post(&mutex);
    }

    printThreadInfo("Crossing", "A -> B", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Crossed!", "A -> B", pthread_id);
    
    sem_wait(&mutex);
    xedCount++;
    xingCount--;
    if(toBWaitCount != 0 && (((xedCount+xingCount)<10) || ((xedCount+xingCount) >= 10 && toAWaitCount == 0))) {
	sem_post(&toB);
    } else if(xingCount == 0 && toAWaitCount != 0 && (toBWaitCount == 0 || (xedCount + xingCount) >= 10)) {
	xingDirection = BtoA;
	xedCount = 0;
	sem_post(&toA);
    } else if(xingCount == 0 && toBWaitCount == 0 && toAWaitCount == 0) {
	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);
    } else {
	sem_post(&mutex);
    }
    
    return NULL;
}

/* This function handles the logic involved with B->A threads crossing.
   This function is one of two functions that the threads run. The other
   function is a mirrored version of this function named "a_to_b_cross()"
   which has almost indentical functionality.

   @input void* pthread_id: A void* cast of int* that contains the "thread id"
                            to the thread invoking this function.

*/
void* b_to_a_cross(void* pthread_id)
{
    printThreadInfo("Dont Want to Cross Yet", "B -> A", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Wants to Cross ", "B -> A", pthread_id);
        
    sem_wait(&mutex);
    if((xingDirection == BtoA || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	printThreadInfo("Can Cross", "B -> A", pthread_id);
	xingDirection = BtoA;
	xingCount++;
	sem_post(&mutex);
    } else {
	printThreadInfo("Waiting to Cross", "B -> A", pthread_id);
	toAWaitCount++;
	sem_post(&mutex);
	sem_wait(&toA);
	toAWaitCount--;
	xingCount++;
	xingDirection = BtoA;
	sem_post(&mutex);
    }

    printThreadInfo("Crossing", "B -> A", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Crossed!", "B -> A", pthread_id);
    
    sem_wait(&mutex);
    xedCount++;
    xingCount--;
    if(toAWaitCount != 0 && (((xedCount+xingCount)<10) || ((xedCount+xingCount) >= 10 && toBWaitCount == 0))) {
	sem_post(&toA);
    } else if(xingCount == 0 && toBWaitCount != 0 && (toAWaitCount == 0 || (xedCount + xingCount) >= 10)) {
	xingDirection = AtoB;
	xedCount = 0;
	sem_post(&toB);
    } else if(xingCount == 0 && toAWaitCount == 0 && toBWaitCount == 0) {
	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);
    } else {
	sem_post(&mutex);
    }

    return NULL;
}

/* MAIN FUNCTION
   This function is responsible for a few things:
   1.) Parsing command line arguments to make sure that the input is properly sanatized
   2.) Initializing semaphores and the default values for pthreads used.
   3.) Starting the appropriate pthreads
   4.) Joining the appropriate pthreads when they are done.
   5.) Freeing any allocated memory still left.

   @input int: Command line argument for number of A->B threads to start
   @input int: Command line argument for number of B->A threads to start
   
*/
int main(int argc, char** argv)
{
    int a_num = 0;
    int b_num = 0;
    if(argc != 3) {
	printf("Usage: ./crossing a_num b_num\n");
	printf("a_num = Number of Baboons A -> B\n");
	printf("b_num = Number of Baboons B -> A\n");
	exit(EXIT_FAILURE);
    } else {
	a_num = atoi(argv[1]);
	b_num = atoi(argv[2]);
    }

    // Create the pthread arrays, attrs, and ids
    pthread_t aBaboons[a_num];
    pthread_t bBaboons[b_num];
    pthread_attr_t attr;
    ids = (int*)calloc(a_num+b_num, sizeof(int));
    
    srandom(time(NULL));
    
    // Explicitly allow the threads to be joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    // Initialize the semaphores
    sem_init(&mutex, THREAD_SHARED, 1);
    sem_init(&toA, THREAD_SHARED, 0);
    sem_init(&toB, THREAD_SHARED, 0);
    sem_init(&print, THREAD_SHARED, 1);

    printf("TID |  Type  |   Time   | Status \n");
    
    // Create A->B Crossing Baboons
    for(int i = 0; i < a_num; i++) {
	ids[i] = i;
	if(pthread_create(&aBaboons[i], &attr, a_to_b_cross, (void*)&ids[i]) == -1) {
	    perror("Thread Creation Error: A");
	    exit(EXIT_FAILURE);
	}
    }

    // Create B->A Crossing Baboons
    for(int i = 0; i < b_num; i++) {
	ids[a_num+i] = a_num + i;
	if(pthread_create(&bBaboons[i], &attr, b_to_a_cross, (void*)&ids[a_num+i]) == -1) {
	    perror("Thread Creation Error: B");
	    exit(EXIT_FAILURE);
	}
    }

    void* status;

    // Join A Crossing Baboons
    for(int i = 0; i < a_num; i++) {
	if(pthread_join(aBaboons[i], &status) == -1) {
	    perror("Thread Join Error: A");
	    exit(EXIT_FAILURE);
	}
    }

    // Join B Crossing Baboons
    for(int i = 0; i < b_num; i++) {
	if(pthread_join(bBaboons[i], &status) == -1) {
	    perror("Thread Join Error: B");
	    exit(EXIT_FAILURE);
	}
    }

    free(ids);
    return 0;
}

