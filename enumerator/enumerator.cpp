#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <fstream>

#include "Network.h"
#include "Enumerator.h"

#define NETWORKS_PATH "networks/"

using namespace std;

int main(int argc, char** argv)
{
	// Handler for wrong arguments set
	// Should be 3 arguments: number of nodes, area side, and experiment round number
	if(argc != 4)
	{
		cout << "Missing arguments!" << endl;
		cout << "USAGE: ./main <number of nodes> <area side> <round>" << endl;
	
		return 0;
	}
	// Read basic model parameters from arguments
	uint64_t n = atoi(argv[1]);
        double area = (double)atof(argv[2]);
        uint64_t run = atoi(argv[3]);
        double tpower = 300.0;
	
	srand(run);
	uint64_t m, f;
	m = f = 0;
	
	// Builds network and checks the number of links
	Network* network;
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
	
	// Opens binary file to write feasible sets
	// Sets the first 128 bits of the binary file to 0s in order to receive the value of m and f after enumeration process
	string name;
	name.append(NETWORKS_PATH).append("n").append(argv[1]).append("a").append(argv[2]).append("r").append(argv[3]).append(".dat");
	ofstream outfile;
	outfile.open(name, ios::binary | ios::out);
	outfile.write((char*)&f, sizeof(uint128_t));

	// Feasible sets enumeration
	Enumerator* enumerator;
	enumerator = new Enumerator(network, &outfile);
        enumerator->find_fset(0);
        f = enumerator->get_fset().size();

	// Returns to the begining of the file and replace the 0s with the actual value of m and f
	// Closes the file
	outfile.seekp(ios_base::beg);
	outfile.write((char*)&m, sizeof(uint64_t));
	outfile.write((char*)&f, sizeof(uint64_t));

	outfile.close();

	// Prints the results to the screen
        cout << n << "\t" << area << "\t" << run << "\t" << m << "\t" << f << endl;	
	
	return 0;
}
