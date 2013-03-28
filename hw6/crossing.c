#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define THREAD_SHARED 0

// Global Variables for Threads
sem_t mutex, toB, toA, print;
int xingCount = 0;
int xedCount = 0;
int toAWaitCount = 0;
int toBWaitCount = 0;
int* ids;
typedef enum {None, AtoB, BtoA} dir_t;
dir_t xingDirection = None;


// Function to sleep for a random amount of millisecs
void thread_sleep(int max_in_ms)
{
    useconds_t duration = (random() % max_in_ms + 1) * 1000;
    if(usleep(duration) != 0) {
	perror("Sleep Error");
	exit(EXIT_FAILURE);
    }	
}


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


int main(int argc, char** argv)
{
    int a_num = 0;
    int b_num = 0
	;
    if(argc != 3) {
	printf("Usage: ./crossing a_num b_num\n");
	printf("a_num = Number of Baboons A -> B\n");
	printf("b_num = Number of Baboons B -> A\n");
	exit(EXIT_FAILURE);
    } else {
	a_num = atoi(argv[1]);
	b_num = atoi(argv[2]);
    }
    
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

