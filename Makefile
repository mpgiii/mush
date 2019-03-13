CC = gcc
CFLAGS = -Wall -pedantic -g
MAIN = mush
OBJS = parse.o misc.o builtins.o
all = $(MAIN)

$(MAIN) : $(OBJS) parse.c
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

misc.o : misc.c misc.h
	$(CC) $(CFLAGS) -c misc.c

builtins.o : builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c

clean :
	rm *.o $(MAIN)
