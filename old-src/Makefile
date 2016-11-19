CC=g++
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
OBJ= scheduling.o Recursive.o Network.o Link.o Node.o
STDDEF= -D__GNUG__
SRCS= scheduling.cc
LFLAGS= -lm -l glpk -l gmp -l z

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

depend:
	makedepend $(STDDEF) -- $(CFLAGS) -- $(SRCS)

clean:
	rm *.o scheduling 
	make

dist-clean:
	rm *.o scheduling
