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

// input format:  n | area | run
// output format: n | area | run | m | f | mc | z | t1 | t2 | drop
int main(int argc, char** argv)
{
	// begin: Toy example
	/*
	vector<uint64_t> sets = {1, 2, 4, 8, 16, 9, 18, 5, 10, 20};
	uint64_t m = 5;
	uint64_t f = 10;
	uint64_t a = m * f;
	*/
	// end: Toy example

	if(argc!=4)
	{
		cout << "Invalid arguments" << endl;
		cout << "USAGE: ./scheduling <NUMBER_OF_NODES> <AREA> <ROUND_NUMBER>" << endl;
		return 0;
	}
	clock_t t, tt, ttt;

	t = clock();
	
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
		// drop = 1: m = 0
		cout << n << "\t" << area << "\t" << run << "\t0\t0\t0\t0\t0\t0\t1" << endl;
		return 0;
	}
	if(m > 128)
	{
		// drop = 2: m > 128
                cout << n << "\t" << area << "\t" << run << "\t" << m << "\t0\t0\t0\t0\t0\t2" << endl;
		return 0;
	}

	enumerator = new Enumerator(network);
	enumerator->find_fset(0);
	tt = clock();
	f = enumerator->get_fset().size();
	if(f == 0)
	{
		// drop = 3: f = 0 
                cout << n << "\t" << area << "\t" << run << "\t" << m << "\t0\t0\t0\t" << ((double)(tt - t))/CLOCKS_PER_SEC << "\t0\t3" << endl;
		return 0;
	}
	vector<uint128_t> sets = enumerator->get_fset();
	delete enumerator;
	delete network;
	
	uint64_t a = f * m;				// # of constraints' coefficients

	if (a > 500000000)
	{
		// drop = 4: a > 500M
                cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t0\t0\t" << ((double)(tt - t))/CLOCKS_PER_SEC << "\t0\t4" << endl;
		return 0;
	}

	double z, y;
	int* ia = new int[1 + a]; 
	int* ja = new int[1 + a];
	double* ar = new double[1 + a];

	fill(ar, ar + 1 + a, 0);
 
	glp_prob* lp;				// glpk program instance
	lp = glp_create_prob();			// initiates lp problem
	glp_set_prob_name(lp, "scheduling");	// labels lp problem
	glp_set_obj_dir(lp, GLP_MAX);		// set lp objective direction (max or min)
        //glp_term_out(GLP_OFF);        // disables glpk terminal output

	glp_add_rows(lp, f);			// auxiliary variables (constraints)
	for(uint64_t i = 0; i < f; i++)
	{
		glp_set_row_name(lp, i + 1, to_string(i).c_str());
		glp_set_row_bnds(lp, i + 1, GLP_UP, 1.0, 1.0);
	}

	glp_add_cols(lp, m);			// structural variables
	for(uint64_t i = 0; i < m; i++)
	{
		glp_set_col_name(lp, i + 1, to_string(i).c_str());
		glp_set_col_bnds(lp, i + 1, GLP_FR, 0.0, 0.0);
		glp_set_obj_coef(lp, i + 1, 1.0);
	}

	uint64_t i, q, r;
	uint128_t p;
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
		ia[i + 1] = i / m + 1;
		ja[i + 1] = i % m + 1;
	}
	glp_load_matrix(lp, a, ia, ja, ar);
	glp_simplex(lp, NULL);
	ttt = clock();

	int mc = 0;
        z = glp_get_obj_val(lp);
        for (uint64_t i = 0; i < m; i++)
        {
                y = glp_get_col_prim(lp, i + 1);
                if((y > 0) && (y < 1))
                {
                        mc = 1;
                        break;
                }
        }

        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t" << mc << "\t" << fixed << setprecision(6) << z << "\t" << ((double)(tt - t))/CLOCKS_PER_SEC << "\t" << ((double)(ttt - tt))/CLOCKS_PER_SEC << "\t0" << endl;

	glp_delete_prob(lp);

	return 0;
}
