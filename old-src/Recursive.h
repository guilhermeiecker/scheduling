/*
** Algorithm 6: Recursive + Parallel masks test without interruption
**
*/

#pragma once

#include <stdint.h>
#include <vector>
#include <math.h>
#include <omp.h>
#include "Node.h"
#include "Link.h"
#include "Network.h"

class Recursive
{
private:
	uint64_t it, index, n, m;
	vector<Link*> cset;
	vector<uint64_t> fset;
	Network* network;

	void add_link(uint64_t);
	double calculate_interference(Node*, Node*);
	bool is_feasible();
	bool primary_test();
	bool secondary_test();
	double calculate_sinr(Link*);
	void del_link(uint64_t);

public:
	Recursive(Network*);
	void find_fset(uint64_t);
	vector<uint64_t> get_fset();
	void print_fset();
	void clear_fset();
};
