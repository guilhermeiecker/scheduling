#pragma once

#include <iostream>	// cout
#include <stdint.h>	// uint64_t
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

uint64_t Link::get_id() 
{ 
	return id; 
}

Node * Link::get_sender() 
{ 
	return sender; 
}

Node * Link::get_recver() 
{ 
	return recver; 
}

double Link::get_dist() 
{ 
	return distance; 
}

double Link::get_rpower() 
{ 
	return rpower; 
}

double Link::get_interference() 
{ 
	return interference; 
}

vector<double> Link::get_interf() 
{ 
	return interf; 
}

void Link::set_id(uint64_t _id) 
{ 
	id = _id; 
}

void Link::set_sender(Node * _s) 
{ 
	sender = _s; 
}

void Link::set_recver(Node * _r) 
{ 
	recver = _r; 
}

void Link::set_dist(double _d) 
{ 
	distance = _d; 
}

void Link::set_rpower(double _rpower) 
{ 
	rpower = _rpower; 
}

void Link::set_interference(double _n) 
{ 
	interference = _n; 
}

void Link::set_interf(vector<double> _interf) 
{ 
	interf = _interf; 
}

void Link::add_interf(double _n) 
{ 
	interf.push_back(_n); 
}

void Link::inc_interference(double _n) 
{ 
	interference = interference + _n; 
}

void Link::dec_interference(double _n) 
{ 
	interference = interference - _n; 
}

double Link::clc_interf()
{
	double result = 0.0;
	for(vector<double>::iterator i = interf.begin(); i != interf.end(); ++i)
		result = result + *i;
	return result;
}

void Link::prt_interf()
{
	cout << "Interference vector for link " << id << "..." << endl;
	for(vector<double>::iterator i = interf.begin(); i != interf.end(); ++i)
		cout << "\t" << *i;
	cout << endl;
}

void Link::del_interf() 
{ 
	interf.pop_back(); 
}

void Link::clr_interf() 
{ 
	interf.clear(); 
}
