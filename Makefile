CC = gcc
CFLAGS = -Wall -pedantic -g
MAIN = parseline
OBJS = parse.o misc.o
all = $(MAIN)

$(MAIN) : $(OBJS) parse.c
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

misc.o : misc.c misc.h
	$(CC) $(CFLAGS) -c misc.c

clean :
	rm *.o $(MAIN)