#include <iostream>	// cout, endl
#include "gurobi_c++.h"

using namespace std;

int main(int argc, char** argv)
{
        GRBVar x;
	GRBLinExpr y;

	int sizeX, sizeY;

	sizeX = sizeof(x);
	sizeY = sizeof(y);

	cout << "GRBVar size:\t" << sizeX << "B" << endl;
	cout << "GRBLinExpr size:\t" << sizeY << "B" << endl;

	return 0;
}
