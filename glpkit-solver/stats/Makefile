CC=g++
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
LFLAGS= -lm

stats: $(OBJ)
	$(CC) $(CFLAGS) stats.cc -o stats $(LFLAGS)

clean:
	rm stats 
