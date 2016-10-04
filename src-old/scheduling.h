#ifndef scheduling_h
#define scheduling_h

#define MAX_NODES 1000

#define MAX_LINKS MAX_NODES*MAX_NODES

#define false 0

#define true 1

double PtdBm;			// Transmission power (dBm)
double PtmW;			// Transmission power (mW)
double freq;			// Central frequency (MHz)
double bandwidth;		// Bandwidth (MHz)
double noiseFloor;		// Noise-floor
double noiseFloordBm;		// Noise-floor (dBm)
double maxRange;		// Maximum communication range (m) (considering only noise-floor)
double alpha;		        // Path loss exponent of Log-distance propagation model
double d0;			// Reference distance of Log-distance propagation model (m)
double L0dB;                    // Loss at the reference distance (dB)   
double beta;                    // Minimum SINR needed to decode a transmission 
double betadB;			// Minimum SINR needed to decode a transmission (dB)

int numNodes;			// Number of nodes in the area
double areaSide;		// Square area side size (meters)

char* outputfile1;		// Name of the output file

char* outputfile2;		// Name of the output file

double delta_mi;                // increment of mi

unsigned int run;		// Number of simulation run

float posX[MAX_NODES];	        // Array of the x coordinates
float posY[MAX_NODES];	        // Array of the y coordinates

//  scheduling algorithms inputs
float dist[MAX_NODES][MAX_NODES];		// Distance matrix
float PrmW[MAX_NODES][MAX_NODES];		// Received power/interference matrix (mW)

// estruturas de dados do santi
typedef struct {
  int sender;
  int recver;
  unsigned int scheduled;
  double weight;
  double IN;
  int id;
} l;

l links[MAX_LINKS];

l ltmp[MAX_LINKS];

float interference[MAX_NODES];

int edges;

int grau[MAX_NODES];

char K[MAX_NODES][MAX_NODES];

int addLink(int id);
void adjustInterference(int sender, int recver);
int isFeasible();
void initialize(void);

#endif
