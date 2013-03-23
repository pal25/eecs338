#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define A_NUM 1
#define B_NUM 3

#define THREAD_SHARED 0

// Global Variables for Threads
sem_t mutex, toB, toA, print;
int xingCount = 0;
int xedCount = 0;
int toAWaitCount = 0;
int toBWaitCount = 0;
int ids[A_NUM + B_NUM];


typedef enum {None, AtoB, BtoA} dir_t;
dir_t xingDirection = None;


// Function to sleep for a random amount of millisecs
void thread_sleep(int max_in_ms)
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


void printThreadInfo(char* status, char* type, void* id)
{
    sem_wait(&print);
    time_t rawtime;
    time(&rawtime);

    int* val = (int*)id;
    
    printf("Thread ID: %d\n", *val);
    printf("Status: %s\n", status);
    printf("Type: %s\n", type);
    printf("Time: %s\n\n", ctime(&rawtime));
    sem_post(&print);
}


void* a_to_b_cross(void* pthread_id)
{
    printThreadInfo("About to sleep before starting", "A -> B", pthread_id);
    thread_sleep(1000);
    
    sem_wait(&mutex);
    if((xingDirection == AtoB || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	xingDirection = AtoB;
	xingCount++;
	sem_post(&mutex);
    } else {
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
    if(xingCount > 0 && xingCount <= 4 &&
       (((xedCount+xingCount) < 10 && toBWaitCount == 0) ||
	((xedCount+xingCount) >= 10 && toAWaitCount != 0))) {

	sem_post(&mutex);
	
    } else if(xingCount > 0 && xingCount <= 4 &&
	      (((xedCount+xingCount) < 10 && toBWaitCount !=0) ||
	       ((xedCount+xingCount) >= 10 && toAWaitCount == 0 && toBWaitCount != 0))) {

	sem_post(&toB);

    } else if((xingCount == 0 && (((xedCount+xingCount) < 10 &&
				   toBWaitCount == 0 && toAWaitCount != 0) ||
				  ((xedCount+xingCount) >= 10 && toAWaitCount != 0)))) {

	xingDirection = BtoA;
	xedCount = 0;
	sem_post(&toA);

    } else if(xingCount == 0 && toBWaitCount == 0 && toAWaitCount == 0) {

	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);

    } else {
	printThreadInfo("Condition Error", "A -> B", pthread_id);
	exit(EXIT_FAILURE);
    }
    return NULL;
}


void* b_to_a_cross(void* pthread_id)
{
    printThreadInfo("About to sleep before starting", "B -> A", pthread_id);
    thread_sleep(1000);
    
    sem_wait(&mutex);
    if((xingDirection == BtoA || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	xingDirection = BtoA;
	xingCount++;
	sem_post(&mutex);
    } else {
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
    if(xingCount > 0 && xingCount <= 4 &&
       (((xedCount+xingCount) < 10 && toAWaitCount == 0) ||
	((xedCount+xingCount) >= 10 && toBWaitCount != 0))) {

	sem_post(&mutex);
	
    } else if(xingCount > 0 && xingCount <= 4 &&
	      (((xedCount+xingCount) < 10 && toAWaitCount !=0) ||
	       ((xedCount+xingCount) >= 10 && toBWaitCount == 0 && toAWaitCount != 0))) {

	sem_post(&toA);

    } else if((xingCount == 0 && (((xedCount+xingCount) < 10 &&
				   toAWaitCount == 0 && toBWaitCount != 0) ||
				  ((xedCount+xingCount) >= 10 && toBWaitCount != 0)))) {

	xingDirection = AtoB;
	xedCount = 0;
	sem_post(&toB);

    } else if(xingCount == 0 && toAWaitCount == 0 && toBWaitCount == 0) {

	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);

    } else {
	printThreadInfo("Condition Error", "B -> A", pthread_id);
	exit(EXIT_FAILURE);
    }

    return NULL;
}


int main(int argc, char** argv)
{
    pthread_t aBaboons[A_NUM];
    pthread_t bBaboons[B_NUM];
    pthread_attr_t attr;

    srandom(time(NULL));
    
    // Explicitly allow the threads to be joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    // Initialize the semaphores
    sem_init(&mutex, THREAD_SHARED, 1);
    sem_init(&toA, THREAD_SHARED, 0);
    sem_init(&toB, THREAD_SHARED, 0);
    sem_init(&print, THREAD_SHARED, 1);

    // Create A->B Crossing Baboons
    for(int i = 0; i < A_NUM; i++) {
	ids[i] = i;
	if(pthread_create(&aBaboons[i], &attr, a_to_b_cross, (void*)&ids[i]) == -1) {
	    perror("Thread Creation Error: A");
	    exit(EXIT_FAILURE);
	}
    }

    // Create B->A Crossing Baboons
    for(int i = 0; i < B_NUM; i++) {
	ids[A_NUM+i] = i;
	if(pthread_create(&bBaboons[i], &attr, b_to_a_cross, (void*)&ids[A_NUM+i]) == -1) {
	    perror("Thread Creation Error: B");
	    exit(EXIT_FAILURE);
	}
    }

    void* status;

    // Join A Crossing Baboons
    for(int i = 0; i < A_NUM; i++) {
	if(pthread_join(aBaboons[i], &status) == -1) {
	    perror("Thread Join Error: A");
	    exit(EXIT_FAILURE);
	}
    }

    // Join B Crossing Baboons
    for(int i = 0; i < B_NUM; i++) {
	if(pthread_join(bBaboons[i], &status) == -1) {
	    perror("Thread Join Error: B");
	    exit(EXIT_FAILURE);
	}
    }
    
    return 0;
}

