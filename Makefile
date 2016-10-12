CC=g++
LFLAGS=-lglpk -lm -lgmp
CFLAGS=-Wall -std=c++11

OBJ= scheduling.o Recursive.o Network.o Link.o Node.o

scheduling: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o scheduling

scheduling.o: scheduling.cc
	$(CC) $(CFLAGS) $(LFLAGS) -c scheduling.cc

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
