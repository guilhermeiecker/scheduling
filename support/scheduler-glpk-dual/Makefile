CC=g++
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
LFLAGS= -lm -lglpk

scheduler: $(OBJ)
	$(CC) $(CFLAGS) scheduler.cpp -o scheduler $(LFLAGS)

clean:
	rm scheduler
