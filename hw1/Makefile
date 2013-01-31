FLAGS = -Wall -Werror -std=c99 -D_XOPEN_SOURCE=600
SOURCE = smokers.c
OUTPUT = smokers

all: $(SOURCE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SOURCE)
	chmod u+x $(OUTPUT)

clean:
	@rm -f $(OUTPUT)
