FLAGS = -Wall -Werror -std=c99 -D_XOPEN_SOURCE=600
SOURCE = sort.c
OUTPUT = sort

all: $(SOURCE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SOURCE)
	chmod u+x $(OUTPUT)

clean:
	@rm -f $(OUTPUT) output
