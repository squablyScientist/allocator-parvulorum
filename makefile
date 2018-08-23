CC = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -fsanitize=address -ggdb
allocator: allpar.o
	$(CC) $(CFLAGS) allpar.o -o allpar
allpar.o: allpar.c
	$(CC) $(CFLAGS) -c allpar.c 

clean:
	rm *.o 
