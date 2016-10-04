CC		= g++

INCTCL          = -I/usr/include

CFLAGS		= $(INCTCL) -g -std=c++11 -Wextra -Wunused

LFLAGS         = -lm -lglpk -lemon

STDDEF		= -D__GNUG__

SRCS		= scheduling.cc isfeasible.cc


all: scheduling

scheduling: scheduling.o
	$(CC) -o scheduling scheduling.o $(LFLAGS)

scheduling.o: scheduling.cc
	$(CC) -c $(CFLAGS) scheduling.cc

depend::
	makedepend $(STDDEF) -- $(CFLAGS) -- $(SRCS)

clean: *.o
	\rm -f *.o

distclean:
	\rm -f *.o *~ scheduling
