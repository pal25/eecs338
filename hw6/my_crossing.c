#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define THREAD_SHARED 0

// Global Variables for Threads
sem_t mutex, blockRight, blockLeft, leftCross, rightCross, turnstile, print;
int leftCount = 0;
int rightCount = 0;


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
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%H:%M:%S", timeinfo);

    int* val = (int*)id;
    
    printf("%3d | %s | %s | %s\n", *val, type, buffer, status);

    sem_post(&print);
}


void* a_to_b_cross(void* pthread_id)
{
    printThreadInfo("Dont Want to Cross Yet", "L -> R", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Wants to Cross ", "L -> R", pthread_id);
    sem_wait(&turnstile);
    sem_wait(&blockLeft);
    sem_post(&blockLeft);

    sem_wait(&mutex);
    leftCount = leftCount + 1;
    if(leftCount == 1)
	sem_wait(&blockRight);
    sem_post(&mutex);
    sem_post(&turnstile);

    sem_wait(&leftCross);
    printThreadInfo("Crossing", "L -> R", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Crossed!", "L -> R", pthread_id);
    sem_post(&leftCross);

    sem_wait(&mutex);
    leftCount = leftCount - 1;
    if(leftCount == 0)
	sem_post(&blockRight);
    sem_post(&mutex);
   
    return NULL;
}


void* b_to_a_cross(void* pthread_id)
{
    printThreadInfo("Dont Want to Cross Yet", "R -> L", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Wants to Cross ", "R -> L", pthread_id);
    sem_wait(&turnstile);
    sem_wait(&blockRight);
    sem_post(&blockRight);

    sem_wait(&mutex);
    rightCount = rightCount + 1;
    if(rightCount == 1)
	sem_wait(&blockLeft);
    sem_post(&mutex);
    sem_post(&turnstile);

    sem_wait(&rightCross);
    printThreadInfo("Crossing", "L -> R", pthread_id);
    thread_sleep(1000);
    printThreadInfo("Crossed!", "L -> R", pthread_id);
    sem_post(&rightCross);

    sem_wait(&mutex);
    rightCount = rightCount - 1;
    if(rightCount == 0)
	sem_post(&blockLeft);
    sem_post(&mutex);
   
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
    sem_init(&blockRight, THREAD_SHARED, 1);
    sem_init(&blockLeft, THREAD_SHARED, 1);
    sem_init(&leftCross, THREAD_SHARED, 5);
    sem_init(&rightCross, THREAD_SHARED, 5);
    sem_init(&turnstile, THREAD_SHARED, 1);
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

