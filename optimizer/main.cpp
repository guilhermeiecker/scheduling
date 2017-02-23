#include <iostream>
#include <fstream>

typedef unsigned __int128 uint128_t;

using namespace std;

void print_bits(uint128_t*);

int main(int argc, char** argv)
{
	if(argc != 6)
        {
                cout << "Missing arguments!" << endl;
                cout << "USAGE: ./main <number of nodes> <area side> <round> <number of links> <number of feasible sets>" << endl;

                return 0;
        }

	uint64_t n, run, m, f;
	double area;

	m = atoi(argv[4]);
	f = atoi(argv[5]);

	if (f == 0)
	{
		cout << "Nothing to optimize" << endl;
		return 0;
	}

	string name;
	name.append("/home/guilherme/networks/n").append(argv[1]).append("a").append(argv[2]).append("r").append(argv[3]).append(".dat");
	ifstream infile(name, ios::binary);
	uint128_t fset;
	double y, z;
	int mc, drop;
		
	for(uint64_t i = 0; i < f; i++)
	{
		infile.read((char*)&fset, sizeof(uint128_t));
		print_bits(&fset);		
	}
	
	return 0;
}

void print_bits(uint128_t* x)
{
        uint128_t p = *x;
        uint128_t q;
        int r, count;
        count = 0;
        do {
                q = p / 2;
                r = p % 2;
                cout << (r == 1) ? 1 : 0;
                p = q;
                count++;
        } while(q > 0);
        cout << ":\t" << count << endl;
}
