#include <glpk.h>	// glpk functions
#include <stdint.h>	// uint64_t
#include <algorithm>	// fill
#include <iomanip>	// setprecision
#include <iostream>	// cout, endl
#include <fstream>	// ifstream, read

// Change this to point to the network binary files folder
#define NETWORKS_PATH "../enumerator/networks/"

typedef unsigned __int128 uint128_t;

using namespace std;

int main(int argc, char** argv)
{
	// Handler for wrong arguments set
	// Should be 3 arguments: number of nodes, area side, and experiment round number
	if(argc!=4)
	{
		cout << "Invalid arguments" << endl;
		cout << "USAGE: ./scheduling <NUMBER_OF_NODES> <AREA> <ROUND_NUMBER>" << endl;
		return 0;
	}
	
	// Reads arguments and opens the binary file containing the feasible sets
	string n, area, run, filename;
	n = argv[1];
	area = argv[2];
	run = argv[3];
	filename.append(NETWORKS_PATH).append("n").append(n).append("a").append(area).append("r").append(run).append(".dat");
	ifstream infile;
	infile.open(filename, ios::binary);

	if(!infile.is_open())
	{
		cout << n << "\t" << area << "\t" << run << "\t0\t0\t-1\t0\t0" << endl;
		return 0;
	}	

	// Reads the number of links and the number of feasible sets from the first 128 bits (64 bits each)
	uint64_t m, f;
        infile.read((char*)&m, sizeof(uint64_t));
        infile.read((char*)&f, sizeof(uint64_t));
	
	// If either m or f is 0, then there is nothing to optimize
	if(f==0)
	{
	        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t-1\t0\t0" << endl;
		return 0;
	}

	// Initializes GLPK and sets its parameters
	glp_prob* lp;                           // glpk program instance
        lp = glp_create_prob();                 // initiates lp problem
        glp_set_prob_name(lp, "scheduler");    // labels lp problem
        glp_set_obj_dir(lp, GLP_MAX);           // set lp objective direction (max or min)
        glp_term_out(GLP_OFF);        // disables glpk terminal output

	uint64_t i, j;

	// Adds rows (constraints)
        glp_add_rows(lp, f);
        for(i = 0; i < f; i++)
        {
                glp_set_row_name(lp, i + 1, to_string(i).c_str());
                glp_set_row_bnds(lp, i + 1, GLP_UP, 1.0, 1.0);
        }

	// Adds columns (variables)
        glp_add_cols(lp, m);                    // structural variables
        for(i = 0; i < m; i++)
        {
                glp_set_col_name(lp, i + 1, to_string(i).c_str());
                glp_set_col_bnds(lp, i + 1, GLP_FR, 0.0, 0.0);
                glp_set_obj_coef(lp, i + 1, 1.0);
        }

	// Populates GLPK axiliary data structures
	// ar -> linearized coefficient matrix (rows rearranged side-by-side)
	// ia, aj -> linearized coefficient matrix's indices
	// Reads every 128 bits from the binary file and iteratively converts the number to its binary form
	// Every number represents a feasible set of links and, consequently, one constraint of the LP
	// The remainder of each iteration (either 0 or 1) represents the coefficient for certain constraint's variable
	uint128_t p, q;
	uint64_t a = m*f;
	int r;

	int* ia = new int[1 + a];
        int* ja = new int[1 + a];
        double* ar = new double[1 + a];

        fill(ar, ar + 1 + a, 0);

	for(j = 0; j < f; j++)
        {
		infile.read((char*)&p, sizeof(uint128_t));
		i = 0;
                do {
                        q = p / 2;
                        r = p % 2;
                        ar[j * m + i + 1] = (r == 1) ? 1 : 0;
                        p = q;
                        i++;
                } while(q > 0);
        }

	infile.close();

	for(i = 0; i < a; i++)
	{
		ia[i + 1] = i / m + 1;
		ja[i + 1] = i % m + 1;
	}

	// Actually solves the LP and stores the optimization process situation in the variable result
	// 0: OK 
	glp_load_matrix(lp, a, ia, ja, ar);
	int result = glp_simplex(lp, NULL);

	// Computes wether it is worth it using multicoloring
	// Simply checks if any variable is not integer.
	double y;
	int y_abs;
	bool mc;

	mc = false;
        for (i = 0; i < m; i++)
        {
                y = glp_get_col_prim(lp, i + 1);
		y_abs = abs(y);
                if((double)y_abs != y)
                {
                        mc = true;
                        break;
                }
        }

	// Gets the objective function result and prints some important data
	// output: nodes | area | run | links | feasible sets | optimization situation | multicoloring indicator | optimization result
        double z = glp_get_obj_val(lp);
        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t" << result << "\t" << mc << "\t" << fixed << setprecision(6) << z << endl;

	glp_delete_prob(lp);
	
	return 0;
}
