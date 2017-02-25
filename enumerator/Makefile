CC=g++
CFLAGS=-g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
LFLAGS= -lm

enumerator: $(OBJ)
	$(CC) $(CFLAGS) enumerator.cpp -o enumerator $(LFLAGS)

clean:
	rm enumerator 
