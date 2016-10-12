#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>       /* time */

#include "Link.h"
#include "Node.h"

using namespace std;

class Network {
private:
	uint64_t num_nodes;
	double area_side;
	double tpower;

	vector<Node> nodes;
	vector<Link> links;

public:
	const double bandwidth = 20e06;
	const double noise_mW = 1380e-23 * 290 * bandwidth;
	const double noise_dBm = 10 * log10(noise_mW);
	const double d0 = 1.0;
	const double l0_dB = 0.0;
	const double alpha = 4.0;
	const double beta_dB = 25.0;
	const double beta_mW = pow(10, beta_dB / 10.0);
	const double tpower_dBm = 10 * log10(tpower);
	const double max_range = d0*pow(10, (tpower_dBm - noise_dBm - beta_dB - l0_dB) / (10 * alpha));

	Network(uint64_t _n = 100, double _a = 3000.0, double _p = 300.0) : num_nodes(_n), area_side(_a), tpower(_p)
	{
		set_nodes();
		set_links();
	}

	vector<Node> get_nodes();
	vector<Link> get_links();

	void set_nodes();
	void set_links();

	Link* get_link(uint64_t);
	void print_links();
};
