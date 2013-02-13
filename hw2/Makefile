FLAGS = -Wall -Werror -std=c99 -D_XOPEN_SOURCE=600
SOURCE = sort.c
OUTPUT = sort

all: sort

sort: $(SOURCE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SOURCE)
	chmod u+x $(OUTPUT)

debug: $(SOURCE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SOURCE) -DDEBUG

clean:
	@rm -f $(OUTPUT) output debug
