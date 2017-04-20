# Copyright 2016, Gurobi Optimization, Inc.

PLATFORM = linux64
INC      = /opt/gurobi701/linux64/include/
CC       = gcc
CPP      = g++
CARGS    = -m64 -g
CLIB     = -L/opt/gurobi701/linux64/lib/ -lgurobi70
CPPLIB   = -L/opt/gurobi701/linux64/lib/ -lgurobi_c++ -lgurobi70

scheduler: main.cpp
	$(CPP) $(CARGS) -std=c++11 main.cpp -o main -I$(INC) $(CPPLIB) -lm

clean:
	rm main

clean-all:
	rm main
	rm log/*
