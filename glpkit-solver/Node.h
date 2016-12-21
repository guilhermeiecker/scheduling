#pragma once

#include <stdint.h>	// uint64_t
#include <random>		// rand
#include <math.h>		// sqrt, pow

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

Node::Node(uint64_t _id, double _area): id(_id)
{
	degree = 0;
	random_position(_area);
}

uint64_t Node::get_id()
{
	return id;
}

uint64_t Node::get_degree()
{
	return degree;
}

double Node::get_x()
{
	return x;
}

double Node::get_y()
{
	return y;
}

void Node::set_id(uint64_t _id)
{
	id = _id;
}

void Node::set_degree(uint64_t _degree)
{
	degree = _degree;
}

void Node::set_x(double _x)
{
	x = _x;
}

void Node::set_y(double _y)
{
	y = _y;
}

void Node::inc_degree()
{
	degree = degree + 1;
}

void Node::dec_degree()
{
	degree = degree - 1;
}

double Node::distance(Node n)
{
	return sqrt(pow(x - n.get_x(), 2) + pow(y - n.get_y(), 2));
}

void Node::random_position(double area)
{
	x = ((double)rand() / (RAND_MAX)) * area;
	y = ((double)rand() / (RAND_MAX)) * area;
}
