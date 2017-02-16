#include <stdio.h>	// setbuf
#include <stdint.h>	// uint64_t
#include <vector>	// vector
#include <algorithm>	// fill
#include <ctime>	// clock_t
#include <iomanip>	// setprecision

#include "gurobi_c++.h"
#include "Network.h"
#include "Enumerator.h"

typedef unsigned __int128 uint128_t;

using namespace std;

int main(int argc, char** argv)
{
	vector<uint64_t> sets = {1, 2, 4, 8, 16, 9, 18, 5, 10, 20};
	uint64_t m = 5;
	uint64_t f = 10;

	try
	{
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);

		GRBVar variables[m];
		GRBLinExpr objective = 0;
		GRBLinExpr constraints[f] = {0};

		for(int i = 0; i < m; i++)
		{
			variables[i] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 1.0, GRB_CONTINUOUS, to_string(i).c_str());
			objective = objective + variables[i];
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
					constraints[j] = constraints[j] + variables[i];
				p = q;
				i++;
			} while(q > 0);
		}
	
		model.setObjective(objective, GRB_MAXIMIZE);
		for(uint64_t i = 0; i < f; i++)
			model.addConstr(constraints[i], GRB_LESS_EQUAL, 1, to_string(i).c_str());
		model.optimize();

	} 
	catch(GRBException e) 
	{
		cout << "Error code = " << e.getErrorCode() << endl;
    		cout << e.getMessage() << endl;
  	}
	catch(...) 
	{
    		cout << "Exception during optimization" << endl;
  	}
	
	return 0;
}
