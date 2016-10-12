#include <glpk.h>
#include <string>

#include "Network.h"
#include "Recursive.h"

using namespace std;

int main(int argc, char** argv)
{
	int n = atoi(argv[1]);
	double area = (double)atof(argv[2]);
	double tpower = (double)atof(argv[3]);

	Network* network = new Network(n, area, tpower);
	Recursive* recursive = new Recursive(network);
	recursive->find_fset(0);

	int f = recursive->get_fset().size();	// # of structural variables	(collumns)
	int m = network->get_links().size();	// # of auxiliary variables (rows)
	int a = f * m;												// # of constraints' coefficients

	double y[f];	// structural vars
	double z;			// object function

	int ia[1 + a], ja[1 + a];
	double ar[1 + a];
	uint64_t* sets = &(recursive->get_fset()[0]);

	glp_prob* lp;	// glpk program instance
	lp = glp_create_prob();								// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MAX);					// set lp objective direction (max or min)

	glp_add_rows(lp, m);	// auxiliary variables (constraints)
	for(int i = 0; i < m; i++)
	{
		glp_set_row_name(lp, i + 1, to_string(i).c_str());
		glp_set_row_bnds(lp, i + 1, GLP_FX, 1.0, 1.0);
	}

	glp_add_cols(lp, f);	// structural variables
	for(int i = 0; i < f; i++)
	{
		glp_set_col_name(lp, i + 1, to_string(i).c_str());
		glp_set_col_bnds(lp, i + 1, GLP_LO, 0.0, 0.0);
		glp_set_obj_coef(lp, i + 1, 1.0);
	}

	int p, q, r, i;
	for(int j = 0; j < f; j++)
	{
		p = sets[j];
		i = 0;
		do {
			q = p / 2;
			r = p % 2;
			ar[f*j + i] = (r == 1) ? 1 : 0;
			p = q;
		} while(q > 0);
	}
	/*
	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	for(int i = 0; i < f; i++)
		y[i] = glp_set_col_prim(lp, i + 1);

	//printf("\nz = %g; x1 = %g; x2 = %g; x3 = %g\n", z, x1, x2, x3);
	*/
	glp_delete_prob(lp);

	return 0;
}
