CFLAGS = -Wall -Werror -std=gnu99 -pthread

all: crossing

debug: my_crossing.c
	gcc $(CFLAGS) -g my_crossing.c -o crossing

crossing: crossing.c
	gcc $(CFLAGS) crossing.c -o crossing

clean:
	rm -f *.o crossing