#pragma once

#include <iostream>
#include <stdint.h>		// uint64_t
#include <math.h>
#include "Node.h"

using namespace std;

class Link {
private:
	Node* sender;			// sender node
	Node* recver;			// receiver node
	uint64_t id;			// unique identification number for a link
	double distance;		// distance between sender and receiver (m)
	double rpower;			// reception power at the receiver (mW)
	double interference;	// total interference (mW)
	vector<double> interf;	// interference within a certain scope (mW)
	
public:
	Link(Node* _s, Node* _r, uint64_t _id, double _distance, double _rpower) : sender(_s), recver(_r), id(_id), distance(_distance), rpower(_rpower) 
	{
		interference = 0.0;
	}

	uint64_t get_id();				// Id getter
	Node* get_sender();				// Sender getter
	Node* get_recver();				// Recver getter
	double get_dist();				// distance getter
	double get_rpower();			// rpower getter
	double get_interference();		// interference getter
	vector<double> get_interf();	// interf getter
	
	void set_id(uint64_t);					// Id setter
	void set_sender(Node*);					// Sender setter
	void set_recver(Node*);					// Recver setter
	void set_dist(double);					// distance setter
	void set_rpower(double);				// rpower setter
	void set_interference(double);			// interference setter
	void set_interf(vector<double> interf);	// interf setter
	
	void inc_interference(double);
	void dec_interference(double);
	
	void add_interf(double);// add new interference
	double clc_interf();
	void prt_interf();// print interference vector of a specific link
	void del_interf();		// delete  interference
	void clr_interf();		// clear   interference
};
