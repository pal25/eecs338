CFLAGS = -Wall -Werror -std=gnu99

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c

oxygen.o: semaphore.h oxygen.c
	gcc $(CFLAGS) -c oxygen.c

hydrogen.o: semaphore.h hydrogen.c
	gcc $(CFLAGS) -c hydrogen.c

clean:
	rm -f *.o 
	rm -i *~