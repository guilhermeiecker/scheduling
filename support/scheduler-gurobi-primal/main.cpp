#include <stdint.h>	// uint64_t
#include <algorithm>	// fill
#include <iomanip>	// setprecision
#include <iostream>	// cout, endl
#include <fstream>	// ifstream, read
#include "gurobi_c++.h"

// TODO This location is for Jupiter only
#define NETWORKS_PATH "/home/guilherme/scheduling/guilherme/enumerating/networks/"

typedef unsigned __int128 uint128_t;

using namespace std;

int main(int argc, char** argv)
{
	if(argc!=6)
	{
		cout << "Invalid arguments" << endl;
		cout << "USAGE: ./scheduling <NUMBER_OF_NODES> <AREA> <ROUND_NUMBER> <SOLVING_METHOD> <DEBUG_FLAG>" << endl;
		return 0;
	}

	string n, area, run, method, filename;
	int debug;
	
	n = argv[1];
	area = argv[2];
	run = argv[3];
	method = argv[4];
	debug = atoi(argv[5]);

	filename = NETWORKS_PATH + n + "-" + area + "-" + run + ".dat";
	ifstream infile;
	infile.open(filename, ios::binary);

	if(!infile.is_open())
	{
		cout << n << "\t" << area << "\t" << run << "\t0\t0\t-1\t0\t0" << endl;
		return 0;
	}

	uint64_t m, f;
        infile.read((char*)&m, sizeof(uint64_t));
        infile.read((char*)&f, sizeof(uint64_t));
	cout << "m=" << m << " f=" << f << endl;
	
	if(f==0)
	{
	        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t-1\t0\t0" << endl;
		return 0;
	}

	try
        {
		clock_t t1, t2, t3, t4, t5, t6, t7, t8;

		t1 = clock();
		
		cout << "Creating model instance... " << flush;
                GRBEnv env = GRBEnv();
                GRBModel model = GRBModel(env);
		model.set("Method", method);
                model.getEnv().set(GRB_IntParam_OutputFlag, 0);
		t2 = clock();
		cout << fixed << setprecision(6) << ((double)(t2 - t1))/CLOCKS_PER_SEC << endl;

		cout << "Allocating variables and data structures... " << flush;
                GRBLinExpr objective = 0;
                GRBLinExpr* constraints = new GRBLinExpr[m];
		fill(constraints, constraints + m, 0);
		t3 = clock();
		cout << fixed << setprecision(6) << ((double)(t3 - t2))/CLOCKS_PER_SEC << endl; 
		
		cout << "Defining variables and expressions... " << flush;
		GRBVar* vars = model.addVars(f, GRB_CONTINUOUS);
		t7 = clock();
		cout << "\n\tVariables created... " << fixed << setprecision(6) << ((double)(t7 - t2))/CLOCKS_PER_SEC << endl;
		uint128_t p, q;
                uint64_t r, idx;
                for(uint64_t j = 0; j < f; j++)
                {
			//cout << j << endl;
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
		t8 = clock();
		cout << "\tObjective and constraints expressions created... " << fixed << setprecision(6) << ((double)(t8 - t7))/CLOCKS_PER_SEC << endl;
		infile.close();
		t4 = clock();

		cout << "Loading objective and constraints... " << flush;
		model.setObjective(objective, GRB_MINIMIZE);
                for(uint64_t i = 0; i < m; i++)
                        model.addConstr(constraints[i], GRB_EQUAL, 1, to_string(i).c_str());
		t5 = clock();
                cout << fixed << setprecision(6) << ((double)(t5 - t4))/CLOCKS_PER_SEC << endl;

		cout << "Solving the model... " << flush;
		model.optimize();
		t6 = clock();
		cout << fixed << setprecision(6) << ((double)(t6 - t5))/CLOCKS_PER_SEC << endl;


                cout << "Retrieving solution... " << flush;
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
		t7 = clock();
                cout << fixed << setprecision(6) << ((double)(t7 - t6))/CLOCKS_PER_SEC << endl;

		cout << "Cleaning dynamic allocated data... " << flush;
		delete[] vars;
		t8 = clock();
                cout << fixed << setprecision(6) << ((double)(t8 - t7))/CLOCKS_PER_SEC << endl;		

		cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t0\t" << mc << "\t" << fixed << setprecision(6) << z << "\t" << ((double)(t7 - t1))/CLOCKS_PER_SEC << endl;
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
