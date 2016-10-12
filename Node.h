#pragma once

#include <stdint.h>
#include <random>
#include <time.h>       /* time */

class Node
{
private:
	uint64_t id, degree;
	double x, y;

public:
	Node(uint64_t, double);

	uint64_t get_id();
	uint64_t get_degree();
	double get_x();
	double get_y();


	void set_id(uint64_t);
	void set_degree(uint64_t);
	void set_x(double);
	void set_y(double);

	void inc_degree();
	void dec_degree();

	double distance(Node);
	void random_position(double);
};
