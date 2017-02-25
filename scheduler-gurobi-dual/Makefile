# Copyright 2016, Gurobi Optimization, Inc.

PLATFORM = linux64
INC      = /opt/gurobi701/linux64/include/
CC       = gcc
CPP      = g++
CARGS    = -m64 -g
CLIB     = -L/opt/gurobi701/linux64/lib/ -lgurobi70
CPPLIB   = -L/opt/gurobi701/linux64/lib/ -lgurobi_c++ -lgurobi70

scheduler: scheduler.cpp
	$(CPP) $(CARGS) -std=c++11 scheduler.cpp -o scheduler -I$(INC) $(CPPLIB) -lm

clean:
	rm scheduler

