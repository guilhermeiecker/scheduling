#include <stdint.h>	// uint64_t
#include <algorithm>	// fill
#include <iomanip>	// setprecision
#include <iostream>	// cout, endl
#include <fstream>	// ifstream, read
#include "gurobi_c++.h"

// TODO This location is for Jupiter only
//#define NETWORKS_PATH "/home/guilherme/scheduling/guilherme/enumerating/networks/"
#define NETWORKS_PATH "/home/guilherme/enumerating/incremental/enum-files/"
#define SOLVING_METHOD "0"

typedef unsigned __int128 uint128_t;

using namespace std;


// n | area | run | m | f | success flag | mc | z |  time
int main(int argc, char** argv)
{
	if(argc!=4)
	{
		cout << "Invalid arguments" << endl;
		cout << "USAGE: ./scheduling <NUMBER_OF_NODES> <AREA> <ROUND_NUMBER>" << endl;
		return 0;
	}

	string n, area, run, filename;
	
	n = argv[1];
	area = argv[2];
	run = argv[3];

	filename = NETWORKS_PATH + n + "-" + area + "-" + run + ".dat";
	ifstream infile;
	infile.open(filename, ios::binary);

	if(!infile.is_open())
	{
		cout << n << "\t" << area << "\t" << run << "\t0\t0\t-1\t0\t0\t0" << endl;
		return 0;
	}

	uint64_t m, f;
        infile.read((char*)&m, sizeof(uint64_t));
        infile.read((char*)&f, sizeof(uint64_t));
	
	if(f==0)
	{
	        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t-1\t0\t0\t0" << endl;
		return 0;
	}

	try
        {
		clock_t t, tt;

		t = clock();
		
                GRBEnv env = GRBEnv();
                GRBModel model = GRBModel(env);
		model.set("Method", "0");
                model.getEnv().set(GRB_IntParam_OutputFlag, 0);
		model.getEnv().set("Presolve", "0");

                GRBLinExpr objective = 0;
                GRBLinExpr* constraints = new GRBLinExpr[m];
		fill(constraints, constraints + m, 0);
		
		GRBVar* vars = model.addVars(f, GRB_CONTINUOUS);
		uint128_t p, q;
                uint64_t r, idx;
                for(uint64_t j = 0; j < f; j++)
                {
			objective += vars[j];
                	infile.read((char*)&p, sizeof(uint128_t));
                        idx = 0;
                        do {
                                q = p / 2;
                                r = p % 2;
                                if (r == 1)
                                        constraints[idx] += vars[j];
                                p = q;
                                idx++;
                        } while(q > 0);
                }
		infile.close();

		model.setObjective(objective, GRB_MINIMIZE);
                for(uint64_t i = 0; i < m; i++)
                        model.addConstr(constraints[i], GRB_EQUAL, 1, to_string(i).c_str());

		model.optimize();

		double z = model.get(GRB_DoubleAttr_ObjVal);
		bool mc= false;
		double y;
		int y_abs;
                for (uint64_t i = 0; i < f; i++)
                {
                        y = vars[i].get(GRB_DoubleAttr_X);
			y_abs = abs(y);
			if((double)y_abs != y)
                	{
                        	mc = true;
                        	break;
                	}
                }

		delete[] vars;
		tt = clock();

		cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t0\t" << mc << "\t" << fixed << setprecision(6) << z << "\t" << ((double)(tt - t))/CLOCKS_PER_SEC << endl;
        }
	catch(GRBException e)
        {
                cout << "Error code = " << e.getErrorCode() << endl;
                return 0;
	}
        catch(...)
        {
        	cout << "Exception during optimization" << endl;
                return 0;
        }

	return 0;
}
