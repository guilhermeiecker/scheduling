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

/// output: n | a | r | m | f | mc | z | mdrop_min | mdrop_max | fdrop_max | t

int main(int argc, char** argv)
{

	clock_t t, tt;
        t = clock();

        uint64_t n = atoi(argv[1]);
        double area = (double)atof(argv[2]);
        uint64_t run = atoi(argv[3]);
        double tpower = 300.0;

        uint64_t m, f;
        m = f = 0;

        srand(run);

        Network* network;
        network = new Network(n, area, tpower);
        m = network->get_links().size();

        if(m == 0)
        {
                tt = clock();
                cout << n << "\t" << area << "\t" << run << "\t0\t0\t0\t0.0\t1\t0\t0\t" << fixed << setprecision(6) << ((double)(tt - t))/CLOCKS_PER_SEC << endl;
                return 0;
        }

        if(m > 128)
        {
                tt = clock();
                cout << n << "\t" << area << "\t" << run << "\t0\t0\t0\t0.0\t0\t1\t0\t" << fixed << setprecision(6) << ((double)(tt - t))/CLOCKS_PER_SEC << endl;
                return 0;
        }

        Enumerator* enumerator;
        enumerator = new Enumerator(network);
        enumerator->find_fset(0);
        f = enumerator->get_fset().size();

	if(f == 0)
        {
                tt = clock();
                cout << n << "\t" << area << "\t" << run << "\t0\t0\t0\t0.0\t0\t0\t1\t" << fixed << setprecision(6) << ((double)(tt - t))/CLOCKS_PER_SEC << endl;
                return 0;
        }

        vector<uint128_t> sets = enumerator->get_fset();
        delete enumerator;
        delete network;

	double z, y;
	uint64_t mc;

	try
	{
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);
		
		// comment the following line to show solution process information
		//model.getEnv().set(GRB_IntParam_OutputFlag, 0);

		GRBVar variables[m];
		GRBLinExpr objective = 0;
		GRBLinExpr constraints[f] = {0};

		for(int i = 0; i < m; i++)
		{
			variables[i] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 1.0, GRB_CONTINUOUS, to_string(i).c_str());
			objective = objective + variables[i];
		}

		uint128_t p, q;	
		uint64_t r, i;
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

		mc = 0;
		z = model.get(GRB_DoubleAttr_ObjVal); 

		for (uint64_t i = 0; i < m; i++)
        	{
			y = variables[i].get(GRB_DoubleAttr_X);
                	if((y > 0) && (y < 1))
                	{
                        	mc = 1;
                        	break;
                	}
        	}
	} 
	catch(GRBException e) 
	{
		cout << "Error code = " << e.getErrorCode() << endl;
    		//cout << e.getMessage() << endl;
    	//	return 0;
  	}
	catch(...) 
	{
    		cout << "Exception during optimization" << endl;
	//	return 0;
  	}

	tt = clock();
        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t" << mc << "\t" << z << "\t0\t0\t0\t" << fixed << setprecision(6) << ((double)(tt - t))/CLOCKS_PER_SEC << endl;

	return 0;
}
