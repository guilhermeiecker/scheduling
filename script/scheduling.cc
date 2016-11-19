#include <glpk.h>	// glpk functions
#include <stdio.h>	// setbuf
#include <stdint.h>	// uint64_t
#include <vector>	// vector
#include <algorithm>	// fill

#include "Network.h"
#include "Recursive.h"

#define M 64
#define F 100000

using namespace std;

int main(int argc, char** argv)
{
	//setbuf(stdout,NULL);

	uint64_t n = atoi(argv[1]);
	double area = (double)atof(argv[2]);
	uint64_t run = atoi(argv[3]);
	double tpower = 300.0;
	//srand(time(NULL));
	srand(run);

	Network* network = new Network(n, area, tpower);
	uint64_t m = network->get_links().size();	// # of auxiliary variables (rows)
	if ((m > 64)||(m == 0))
	{
		//printf("%ld\t%lf\t%ld\t%ld\t?\t?\t?\n", n, area, run, m);
		return 0;
	}
	
	Recursive* recursive = new Recursive(network);
	recursive->find_fset(0);
	vector<uint64_t> sets = recursive->get_fset();
	uint64_t f = recursive->get_fset().size();	// # of structural variables	(collumns)
	delete network;
	delete recursive;
	if (f > 9765625)	// Para RAM=20GB => f≃9.765.625; para RAM=3GB =>f≃1.500.000 
	{
		cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t?\t?" << endl;
		//printf("%ld\t%lf\t%ld\t%ld\t%ld\t?\t?\n", n, area, run, m, f);
		return 0;
	}
	
	uint64_t a = f * m;				// # of constraints' coefficients
	double z;
	int* ia = new int[1 + a]; 
	int* ja = new int[1 + a];
	double* ar = new double[1 + a];

	fill(ar, ar + 1 + a, 0);
 
	glp_prob* lp;				// glpk program instance
	lp = glp_create_prob();			// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MIN);		// set lp objective direction (max or min)
	glp_term_out(GLP_OFF);	// disables glpk terminal output
	
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
			ar[i * f + j + 1] = (r == 1) ? 1 : 0;
			p = q;
			i++;
		} while(q > 0);
	}

	for(uint64_t i = 0; i < a; i++)
  {
		ia[i + 1] = i / f + 1;
		ja[i + 1] = i % f + 1;
  }

	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);

	int mc;	// flag1: solution found; flag2: MC==T
	double y; // if all the y values are relevant, this must be replaced with a dynamic array of size f
	
	mc = 0;
	z = glp_get_obj_val(lp);
	for (uint64_t i = 0; i < f; i++)
	{
		y = glp_get_col_prim(lp, i + 1);
		if((y > 0) && (y < 1))
		{
			mc = 1;
			break;
		}
	}
	cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t" << mc << "\t" << z << endl;
	//printf("%ld\t%lf\t%ld\t%ld\t%ld\t%d\t%lf\n", n, area, run, m, f, mc, z);
	
	glp_delete_prob(lp);

	return 0;
}
