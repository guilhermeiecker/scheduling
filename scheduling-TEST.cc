#include <glpk.h>
#include <stdio.h>
#include <cstring>
#include <string>

#include "Network.h"
#include "Recursive.h"

using namespace std;

int main(int argc, char** argv)
{
	/*
	int n = atoi(argv[1]);
	double area = (double)atof(argv[2]);
	double tpower = (double)atof(argv[3]);

	srand((int)(area*100));

	Network* network = new Network(n, area, tpower);
	Recursive* recursive = new Recursive(network);
	recursive->find_fset(0);

	vector<uint64_t> sets = recursive->get_fset();

	int f = recursive->get_fset().size();	// # of structural variables	(collumns)
	int m = network->get_links().size();	// # of auxiliary variables (rows)
	int a = f * m;				// # of constraints' coefficients
	*/
	
	//Network example
	uint64_t sets[10] = {1, 2, 4, 5, 8, 9, 10, 16, 18, 20};
	int f = 10;
	int m = 5;
	int a = f * m;

	double y[f];				// structural vars
	double z;				// object function
	
	int ia[1 + a], ja[1 + a];
	double ar[1 + a], aux[f][m];
	
	memset(aux, 0, sizeof(aux[0][0]) * m * f);
	fill(ar, ar + 1 + a, 0);
	
	//cout << "Network generated with n=" << n << " nodes and m=" << m << " links." << endl;
	//cout << "The enumeration algorithm found f=" << f << " feasible sets." << endl;
	//recursive->print_fset();

	glp_prob* lp;				// glpk program instance
	lp = glp_create_prob();			// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MIN);		// set lp objective direction (max or min)

	glp_add_rows(lp, m);			// auxiliary variables (constraints)
	for(int i = 0; i < m; i++)
	{
		glp_set_row_name(lp, i + 1, to_string(i).c_str());
		glp_set_row_bnds(lp, i + 1, GLP_FX, 1.0, 1.0);
	}

	glp_add_cols(lp, f);			// structural variables
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
			aux[j][i] = (r == 1) ? 1 : 0;
			p = q;
			i++;
		} while(q > 0);
	}

	cout << "Decoded array..." << endl;
	for(int i = 0; i < f; i++)
	{
		for(int j = 0; j < m; j++)
			cout << aux[i][j] << "\t";
		cout << endl;
	}
	cout << endl;

	for(int j = 0; j < m; j++)
		for(int i = 0; i < f; i++)
        		ar[j * f + i + 1] = aux[i][j];
	
	for(int i = 0; i < a; i++)
        {
                ia[i + 1] = i / f + 1;
                ja[i + 1] = i % f + 1;
                cout << "ia[" << i + 1 << "]=" << ia[i + 1] << ", ja[" << i + 1 << "]=" << ja[i + 1] << ", aux[" << i + 1 << "]=" << ar[i + 1] << endl;
        }
	
	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	for(int i = 0; i < f; i++)
		y[i] = glp_get_col_prim(lp, i + 1);

	cout << "z=" << z << endl;
	for(int i = 0; i < f; i++)
	{
		cout << "y" << sets[i] << "=" << y[i] << endl;
	}
	glp_delete_prob(lp);

	return 0;
}
