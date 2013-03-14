#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>


#define LEFT_NUM 1
#define RIGHT_NUM 3

#define THREAD_SHARED 0
#define RAND_SEED 5135


// Global Variables for Threads
sem_t mutex, toR, toL, print;
int xingCount = 0;
int xedCount = 0;
int toRWaitCount = 0;
int toLWaitCount = 0;
int ids[LEFT_NUM + RIGHT_NUM];


typedef enum {None, LtoR, RtoL} dir_t;
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


void* leftCross(void* pthread_id)
{
    printThreadInfo("About to sleep before starting", "Left to Right", pthread_id);
    thread_sleep(1000);
    
    sem_wait(&mutex);
    if((xingDirection == LtoR || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	xingDirection = LtoR;
	xingCount++;
	sem_post(&mutex);
    } else {
	toRWaitCount++;
	sem_post(&mutex);
	sem_wait(&toR);
	toRWaitCount--;
	xingCount++;
	xingDirection = LtoR;
	sem_post(&mutex);
    }

    printThreadInfo("Crossing", "Left to Right", pthread_id);
    thread_sleep(10000);
    printThreadInfo("Crossed!", "Left to Right", pthread_id);
    
    sem_wait(&mutex);
    xedCount++;
    xingCount--;
    if(xingCount > 0 && xingCount >= 4 &&
       (((xedCount+xingCount) < 10 && toRWaitCount == 0) ||
	((xedCount+xingCount) >= 10 && toLWaitCount != 0))) {

	sem_post(&mutex);
	
    } else if(xingCount > 0 && xingCount >= 4 &&
	      (((xedCount+xingCount) < 10 && toRWaitCount !=0) ||
	       ((xedCount+xingCount) >= 10 && toLWaitCount != 0))) {

	sem_post(&toR);

    } else if((xingCount == 0 && (((xedCount+xingCount) < 10 &&
				   toRWaitCount == 0 && toLWaitCount != 0) ||
				  ((xedCount+xingCount) >= 10 && toLWaitCount != 0)))) {

	xingDirection = RtoL;
	xedCount = 0;
	sem_post(&toL);

    } else if(xingCount == 0 && toRWaitCount == 0 && toLWaitCount == 0) {

	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);

    } else {
	printThreadInfo("Condition Error", "Left to Right", pthread_id);
	exit(EXIT_FAILURE);
    }
    return NULL;
}


void* rightCross(void* pthread_id)
{
    printThreadInfo("About to sleep before starting", "Right to Left", pthread_id);
    thread_sleep(1000);
    
    sem_wait(&mutex);
    if((xingDirection == RtoL || xingDirection == None) && xingCount < 5 && (xedCount + xingCount) < 10) {
	xingDirection = RtoL;
	xingCount++;
	sem_post(&mutex);
    } else {
	toRWaitCount++;
	sem_post(&mutex);
	sem_wait(&toL);
	toRWaitCount--;
	xingCount++;
	xingDirection = RtoL;
	sem_post(&mutex);
    }

    printThreadInfo("Crossing", "Right to Left", pthread_id);
    thread_sleep(10000);
    printThreadInfo("Crossed!", "Right to Left", pthread_id);
    
    sem_wait(&mutex);
    xedCount++;
    xingCount--;
    if(xingCount > 0 && xingCount >= 4 &&
       (((xedCount+xingCount) < 10 && toLWaitCount == 0) ||
	((xedCount+xingCount) >= 10 && toRWaitCount != 0))) {

	sem_post(&mutex);
	
    } else if(xingCount > 0 && xingCount >= 4 &&
	      (((xedCount+xingCount) < 10 && toLWaitCount !=0) ||
	       ((xedCount+xingCount) >= 10 && toRWaitCount != 0))) {

	sem_post(&toL);

    } else if((xingCount == 0 && (((xedCount+xingCount) < 10 &&
				   toLWaitCount == 0 && toRWaitCount != 0) ||
				  ((xedCount+xingCount) >= 10 && toRWaitCount != 0)))) {

	xingDirection = LtoR;
	xedCount = 0;
	sem_post(&toR);

    } else if(xingCount == 0 && toLWaitCount == 0 && toRWaitCount == 0) {

	xingDirection = None;
	xedCount = 0;
	sem_post(&mutex);

    } else {
	printThreadInfo("Condition Error", "Right to Left", pthread_id);
	exit(EXIT_FAILURE);
    }

    return NULL;
}


int main(int argc, char** argv)
{
    pthread_t leftBaboons[LEFT_NUM];
    pthread_t rightBaboons[RIGHT_NUM];
    pthread_attr_t attr;

    srandom(RAND_SEED);
    
    // Explicitly allow the threads to be joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    // Initialize the semaphores
    sem_init(&mutex, THREAD_SHARED, 1);
    sem_init(&toL, THREAD_SHARED, 0);
    sem_init(&toR, THREAD_SHARED, 0);
    sem_init(&print, THREAD_SHARED, 1);

    // Create Left Crossing Baboons
    for(int i = 0; i < LEFT_NUM; i++) {
	ids[i] = i;
	if(pthread_create(&leftBaboons[i], &attr, leftCross, (void*)&ids[i]) == -1) {
	    perror("Thread Creation Error: Left");
	    exit(EXIT_FAILURE);
	}
    }

    // Create Right Crossing Baboons
    for(int i = 0; i < RIGHT_NUM; i++) {
	ids[LEFT_NUM+i] = i;
	if(pthread_create(&rightBaboons[i], &attr, rightCross, (void*)&ids[LEFT_NUM+i]) == -1) {
	    perror("Thread Creation Error: Right");
	    exit(EXIT_FAILURE);
	}
    }

    void* status;

    // Join Left Crossing Baboons
    for(int i = 0; i < LEFT_NUM; i++) {
	if(pthread_join(leftBaboons[i], &status) == -1) {
	    perror("Thread Join Error: Left");
	    exit(EXIT_FAILURE);
	}
    }

    // Join Right Crossing Baboons
    for(int i = 0; i < RIGHT_NUM; i++) {
	if(pthread_join(rightBaboons[i], &status) == -1) {
	    perror("Thread Join Error: Right");
	    exit(EXIT_FAILURE);
	}
    }
    
    return 0;
}

