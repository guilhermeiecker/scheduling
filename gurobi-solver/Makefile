CC=g++
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
LFLAGS= -lm -lglpk

scheduling: $(OBJ)
	$(CC) $(CFLAGS) scheduling.cc -o scheduling $(LFLAGS)

clean:
	rm scheduling 
