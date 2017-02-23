#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <fstream>

#include "Network.h"
#include "Enumerator.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cout << "Missing arguments!" << endl;
		cout << "UASGE: ./main <number of nodes> <area side> <round>" << endl;
	
		return 0;
	}

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
                cout << n << "\t" << area << "\t" << run << "\t" << m << "\t0" << endl;
                return 0;
        }
        if(m > 128)
        {
                cout << n << "\t" << area << "\t" << run << "\t" << m << "\t0" << endl;
                return 0;
        }
	
	string name = "/home/guilherme/networks/n" + to_string(n) + "a" + to_string((uint64_t)area) + "r" + to_string(run) + ".dat";
	ofstream outfile;
	outfile.open(name, ios::binary | ios::out);
	enumerator = new Enumerator(network, &outfile);
        enumerator->find_fset(0);
        f = enumerator->get_fset().size();
	outfile.close();
	cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << endl;

	return 0;
}
