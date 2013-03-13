CFLAGS = -Wall -Werror -std=gnu99

all: main

main: oxygen hydrogen semaphore.o main.o
	gcc $(CFLAGS) main.o semaphore.o -o main

oxygen: oxygen.o semaphore.o
	gcc $(CFLAGS) oxygen.o semaphore.o -o oxygen 

hydrogen: hydrogen.o semaphore.o
	gcc $(CFLAGS) hydrogen.o semaphore.o -o hydrogen 

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c

main.o: semaphore.h main.c
	gcc $(CFLAGS) -c main.c

oxygen.o: semaphore.h oxygen.c
	gcc $(CFLAGS) -c oxygen.c

hydrogen.o: semaphore.h hydrogen.c
	gcc $(CFLAGS) -c hydrogen.c

clean:
	rm -f *.o hydrogen oxygen main