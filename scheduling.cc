#include <glpk.h>

#include "Network.h"
#include "Recursive.h"

int main()
{
	Network* n = new Network(64, 2000.0, 350.0);
	Recursive* a = new Recursive(n);

	a->find_fset(0);

	glp_prob* lp;	// glpk program instance
	
	int r, c, a; 	// number of aux. variables, struc. variables, and constraints' coefficients, respec.
	double z;		// objective function

	//TODO: Initialize p, q, and r

	int ia[a+1], ja[a+1]; 	// constraints' coeficient matrix indices
	double ar[a+1];			// constraints' coefficient matrix entries

	double x[c+1];		// objective function's independent (struc.) variables
	
	double of_coef[c+1]; 	// objective function's coefficients
	double ct_coef[r+1];	// constraints' coefficients

	//TODO: Initialize of_coef and ct_coef

	lp = glp_create_prob();					// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// names lp problem
	glp_set_obj_dir(lp, GLP_MAX);			// set lp objective direction (max or min)

	glp_add_rows(lp, r);	// auxiliary variables (constraints)

	//TODO: Set rows' names and bounds
	/*
	for(int i = 0; i < r; i++)
	{
		glp_set_row_name(lp, i+1, itoa(i));
		glp_set_row_bnds(lp, i+1, ???, ???, ???);
	}
	*/

	glp_add_cols(lp, c);	// structural variables

	//TODO: Set collumns' names, bounds, and coefficients
	/*
	for(int i = 0; i < c; i++)
	{
		glp_set_col_name(lp, i+1, itoa(i));
		glp_set_col_bnds(lp, i+1, ???, ???, ???);
		glp_set_col_coef(lp, i+1, of_coef[i])
	}
	*/

	//TODO: Build coefficient matrix, i.e., set values for ia, ja, ar
	/*
	for(int i = 0; i < r; i++)
	{
		ia[i] = 1, ja[i] = 1, ar[i] = 1.0;
	}
	*/

	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	for(int i = 0; i < q; i++)
		x[i] = glp_set_col_prim(lp, i+1);

	printf("\nz = %g; x1 = %g; x2 = %g; x3 = %g\n", z, x1, x2, x3);

	glp_delete_prob(lp);

	return 0;
}