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

/// nodes | area | run | m | f | mc | z | mdrop_min | mdrop_max | fdrop_max | glpk_drop | time 

int main(int argc, char** argv)
{
	// begin: Toy example
        vector<uint64_t> sets = {1, 2, 4, 8, 16, 9, 18, 5, 10, 20};
        uint64_t m = 5;
        uint64_t f = 10;
        uint64_t a = m * f;
        // end: Toy example

	GRBVar vars[f];
	GRBLinExpr objective = 0;
	GRBLinExpr constraints[m];

	for(int i = 0; i < f; i++)
	{
		vars[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, to_string(i).c_str());
		obj = obj + vars[i];
	}
	
	uint64_t p, q, r, i;
	for(uint64_t j = 0; j < f; j++)
	{
		p = sets[j];
		i = 0;
		do {
			q = p / 2;
			r = p % 2;
			if (r == 1)
				constraints[i] = constraints[i] + vars[j];
			p = q;
			i++;
		} while(q > 0);
	}

	return 0;
}
