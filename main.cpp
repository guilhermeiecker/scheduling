#include <stdint.h>	// uint64_t
#include <algorithm>	// fill
#include <iomanip>	// setprecision
#include <iostream>	// cout, endl
#include <fstream>	// ifstream, read
#include "gurobi_c++.h"

// Change this to point to the network binary files folder
#define NETWORKS_PATH "../enumerating/networks/"

typedef unsigned __int128 uint128_t;

using namespace std;

int main(int argc, char** argv)
{
	// Handler for wrong arguments set
	// Should be 3 arguments: number of nodes, area side, and experiment round number
	if(argc!=5)
	{
		cout << "Invalid arguments" << endl;
		cout << "USAGE: ./scheduling <NUMBER_OF_NODES> <AREA> <ROUND_NUMBER> <SOLVING METHOD>" << endl;
		return 0;
	}
		
	// Reads arguments and opens the binary file containing the feasible sets
	string n, area, run, method, filename;
	n = argv[1];
	area = argv[2];
	run = argv[3];
	method = argv[4];
		
	filename = NETWORKS_PATH + n + "-" + area + "-" + run + ".dat";
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
	
	try
        {
		// Sets some Gurobi's variables	
                GRBEnv env = GRBEnv();
                GRBModel model = GRBModel(env);
                //model.getEnv().set(GRB_IntParam_OutputFlag, 0);

		// Instantiates the LP components
                GRBVar variables[m];
                GRBLinExpr objective = 0;
                GRBLinExpr* constraints = new GRBLinExpr[f];

		fill(constraints, constraints + f, 0);
		
		uint64_t i, j;
		
		// Initializes the objective function
		for(i = 0; i < m; i++)
                {
                        variables[i] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 1.0, GRB_CONTINUOUS, to_string(i).c_str());
                        objective = objective + variables[i];
                }
		
		// Initializes the constraints based on the feasible sets found
		uint128_t p, q;
                int r;
                for(j = 0; j < f; j++)
                {
                	infile.read((char*)&p, sizeof(uint128_t));
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

		infile.close();
		
		// Sets the optimization direction, establishes boundaries to the constraints, and solve it
		model.setObjective(objective, GRB_MAXIMIZE);
                for(i = 0; i < f; i++)
                        model.addConstr(constraints[i], GRB_LESS_EQUAL, 1, to_string(i).c_str());

		model.set("Method", method);
                model.optimize();

		// Gets the result
                double z = model.get(GRB_DoubleAttr_ObjVal);

		// Calculates the multicoloring feasibilty based on the variables values
		bool mc = false;
		double y;
		int y_abs;


		//cout << "Dual variables:";
                for (i = 0; i < f; i++)
                {
                        y = model.getConstrByName(to_string(i)).get(GRB_DoubleAttr_Pi);
			y_abs = abs(y);
			if((double)y_abs != y)
                	{
                        	mc = true;
                        	break;
                	}
			//cout << "\t" << y;
                }
		//cout << endl;	

		// Prints the result	
		cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << "\t0\t" << mc << "\t" << fixed << setprecision(6) << z << endl;

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
