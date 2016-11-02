#include <glpk.h>
#include <stdio.h>
#include <cstring>
#include <string>

#include "Network.h"
#include "Recursive.h"

#define SIZE 10000000

using namespace std;

int main(int argc, char** argv)
{
	setbuf(stdout,NULL);
	uint64_t n = atoi(argv[1]);
	double area = (double)atof(argv[2]);
	double tpower = 300.0;

	Network* network = new Network(n, area, tpower);
	Recursive* recursive = new Recursive(network);
	recursive->find_fset(0);
	vector<uint64_t> sets = recursive->get_fset();
	
	uint64_t m = network->get_links().size();	// # of auxiliary variables (rows)
	uint64_t f = recursive->get_fset().size();	// # of structural variables	(collumns)
	uint64_t a = f * m;				// # of constraints' coefficients
	
	double y[f];				// structural vars
	double z;				// object function
	int ia[1 + a], ja[1 + a];
	double ar[1 + a], aux[f][m];

	memset(aux, 0, sizeof(aux[0][0]) * m * f);
	fill(ar, ar + 1 + a, 0);
 
	cout << "Network generated with n=" << n << " nodes and m=" << m << " links." << endl;
	cout << "The enumeration algorithm found f=" << f << " feasible sets." << endl;
	
	glp_prob* lp;				// glpk program instance
	lp = glp_create_prob();			// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MIN);		// set lp objective direction (max or min)

	glp_add_rows(lp, m);			// auxiliary variables (constraints)
	for(uint64_t i = 0; i < m; i++)
	{
		glp_set_row_name(lp, i + 1, to_string(i).c_str());
		glp_set_row_bnds(lp, i + 1, GLP_FX, 1.0, 1.0);
	}

	glp_add_cols(lp, f);			// structural variables
	for(uint64_t i = 0; i < f; i++)
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
			aux[j][i] = (r == 1) ? 1 : 0;
			p = q;
			i++;
		} while(q > 0);
	}

	for(uint64_t j = 0; j < m; j++)
		for(uint64_t i = 0; i < f; i++)
        		ar[j * f + i + 1] = aux[i][j];
	
	for(uint64_t i = 0; i < a; i++)
        {
                ia[i + 1] = i / f + 1;
                ja[i + 1] = i % f + 1;
        }
	
	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	for(uint64_t i = 0; i < f; i++)
		y[i] = glp_get_col_prim(lp, i + 1);

	
	cout << "z=" << z << endl;
	for (uint64_t i = 0; i < f; i++)
	{
		if((y[i] > 0)&&(y[i] < 1))
			cout << "y[" << sets[i] << "]=" << y[i] << endl;
	}	
	glp_delete_prob(lp);

	return 0;
}
