CC = gcc
CFLAGS = -pedantic -std=c99 -Wall -Wextra -Werror

allocator: allpar.o
	$(CC) $(CFLAGS) allpar.o -o allpar

allpar.o: allpar.c
	$(CC)  $(CFLAGS) -c allpar.c 

clean:
	rm *.o 
