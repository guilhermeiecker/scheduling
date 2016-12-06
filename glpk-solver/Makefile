CC=g++
CFLAGS=$(INCTCL) -g -std=c++11 -Wextra -Wunused -Wall -mcmodel=large
INCTCL=-I/usr/include
LFLAGS= -lm -lglpk

main: $(OBJ)
	$(CC) $(CFLAGS) main.cc -o main $(LFLAGS)

clean:
	rm main 
