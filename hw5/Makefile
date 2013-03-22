CFLAGS = -Wall -Werror -std=gnu99

all: main

debug: oxygen hydrogen semaphore.o main.c
	gcc $(CFLAGS) -g main.c semaphore.o -o main

main: oxygen hydrogen semaphore.o main.c
	gcc $(CFLAGS) main.c semaphore.o -o main

oxygen: oxygen.c semaphore.o
	gcc $(CFLAGS) oxygen.c semaphore.o -o oxygen 

hydrogen: hydrogen.c semaphore.o
	gcc $(CFLAGS) hydrogen.c semaphore.o -o hydrogen 

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c

clean:
	rm -f *.o hydrogen oxygen main