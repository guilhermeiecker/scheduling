CC=g++
LFLAGS=-lglpk -lm -lgmp
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
OBJ= scheduling.o Recursive.o Network.o Link.o Node.o

scheduling: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o scheduling

scheduling.o: scheduling.cc
	$(CC) -c $(CFLAGS) scheduling.cc

Recursive.o: Recursive.cc
	$(CC) $(CFLAGS) -c Recursive.cc

Network.o: Network.cc
	$(CC) $(CFLAGS) -c Network.cc

Link.o: Link.cc
	$(CC) $(CFLAGS) -c Link.cc

Node.o: Node.cc
	$(CC) $(CFLAGS) -c Node.cc

clean:
	rm *.o scheduling 
	make

dist-clean:
	rm *.o scheduling
