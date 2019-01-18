# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

CC = g++                       # compiler to use
OBJS = main.o vdisc.o

lab6: $(OBJS)
				$(CC) -g -std=c++11 -o lab6 $(OBJS)

main.o: vdisc.h
vdisc.o: vdisc.h


.PHONY = clean
clean:
				rm lab6 $(OBJS)
