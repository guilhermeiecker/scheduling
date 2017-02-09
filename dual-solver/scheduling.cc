#include <glpk.h>	// glpk functions
#include <stdio.h>	// setbuf
#include <stdint.h>	// uint64_t
#include <vector>	// vector
#include <algorithm>	// fill
#include <ctime>	// clock_t
#include <iomanip>	// setprecision

#include "Network.h"
#include "Enumerator.h"

typedef unsigned __int128 uint128_t;

using namespace std;

int main(int argc, char** argv)
{
	// begin: Toy example
	vector<uint64_t> sets = {1, 2, 4, 8, 16, 9, 18, 5, 10, 20};
	uint64_t m = 5;
	uint64_t f = 10;
	uint64_t a = m * f;
	// end: Toy example

	/*
	uint64_t n = atoi(argv[1]);
	double area = (double)atof(argv[2]);
	uint64_t run = atoi(argv[3]);
	double tpower = 300.0;

	Network* network;
	Enumerator* enumerator;
	uint64_t m, f;
	
	m = f = 0;
	srand(run);
	network = new Network(n, area, tpower);
	m = network->get_links().size();
	if(m == 0)
	{
		cout << "ERROR: m=0";
		return 0;
	}
	if(m > 128)
	{
		cout << "ERROR: m>128";
		return 0;
	}

	enumerator = new Enumerator(network);
	enumerator->find_fset(0);
	f = enumerator->get_fset().size();
	if(f == 0)
	{
		cout << "ERROR: f reached RAM limitation";
		return 0;
	}

	vector<uint128_t> sets = enumerator->get_fset();
	delete enumerator;
	delete network;
	
	uint64_t a = f * m;				// # of constraints' coefficients

	if (a > 500000000)
	{
		cout << "ERROR: glpk limitation reached";
		return 0;
	}
	*/

	double z, y;
	int* ia = new int[1 + a]; 
	int* ja = new int[1 + a];
	double* ar = new double[1 + a];

	fill(ar, ar + 1 + a, 0);
 
	glp_prob* lp;				// glpk program instance
	lp = glp_create_prob();			// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MAX);		// set lp objective direction (max or min)
	
	glp_add_rows(lp, f);			// auxiliary variables (constraints)
	for(uint64_t i = 0; i < f; i++)
	{
		glp_set_row_name(lp, i + 1, to_string(i).c_str());
		glp_set_row_bnds(lp, i + 1, GLP_UP, 0.0, 1.0);
	}

	glp_add_cols(lp, m);			// structural variables
	for(uint64_t i = 0; i < m; i++)
	{
		glp_set_col_name(lp, i + 1, to_string(i).c_str());
		glp_set_col_bnds(lp, i + 1, GLP_LO, 0.0, 0.0);
		glp_set_obj_coef(lp, i + 1, 1.0);
	}

	uint64_t p, q, r, i;
	for(uint64_t j = 0; j < f; j++)
	{
		p = sets[j];
		i = 0;
		do {
			q = p / 2;
			r = p % 2;
			ar[j * m + i + 1] = (r == 1) ? 1 : 0;
			p = q;
			i++;
		} while(q > 0);
	}

	for(uint64_t i = 0; i < a; i++)
	{
		ja[i + 1] = i / f + 1;
		ia[i + 1] = i % f + 1;
	}

	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	cout << "\nz=" << z << endl;

	cout << "primal variables:" << endl;
	for (uint64_t i = 0; i < m; i++)
	{
		y = glp_get_col_prim(lp, i + 1);
		cout << y << "\t";
	}

	cout << "\ndual variables:" << endl;
	for (uint64_t i = 0; i < m; i++)
	{
		y = glp_get_col_dual(lp, i + 1);
		cout << y << "\t";
	}
	cout << endl;

	glp_delete_prob(lp);

	return 0;
}
