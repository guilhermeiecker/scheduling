#include "Link.h"

uint64_t Link::get_id() { return id; }
Node * Link::get_sender() { return sender; }
Node * Link::get_recver() { return recver; }
double Link::get_dist() { return distance; }
double Link::get_rpower() { return rpower; }
double Link::get_interference() { return interference; }
vector<double> Link::get_interf() { return interf; }

void Link::set_id(uint64_t _id) { id = _id; }
void Link::set_sender(Node * _s) { sender = _s; }
void Link::set_recver(Node * _r) { recver = _r; }
void Link::set_dist(double _d) { distance = _d; }
void Link::set_rpower(double _rpower) { rpower = _rpower; }
void Link::set_interference(double _n) { interference = _n; }
void Link::set_interf(vector<double> _interf) { interf = _interf; }

void Link::add_interf(double _n) { interf.push_back(_n); }
void Link::inc_interference(double _n) { interference = interference + _n; }
void Link::dec_interference(double _n) { interference = interference - _n; }

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

void Link::del_interf() { interf.pop_back(); }
void Link::clr_interf() { interf.clear(); }
