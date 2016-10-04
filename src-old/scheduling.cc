#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <glpk.h>
#include "scheduling.h"
#include <lemon/matching.h>
#include <lemon/smart_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/concepts/maps.h>

using namespace std;
using namespace lemon;

// Procedure that returns each input parameter
int paramCount = 0;
char* getParam(int argc, char** argv) {
  paramCount++;
  return argv[paramCount];
}

// Procedure that generates a uniform random number between 0 and 1
double uniform() {
  double r = rand()/((double)RAND_MAX+1);
  return r;
}

// Procedure that reads all input parameters
void readParams(int argc, char** argv) {

  // Number of nodes in the area
  numNodes = atoi(getParam(argc, argv)); 

  // Square area side size (meters)
  areaSide = atof(getParam(argc, argv)); 

  // Central frequency (Hz)
  freq = 2400e06; 

  // Bandwidth (Hz)
  bandwidth = 20e06;

  // Noise-floor (dBm)
  // noise-floor = k*T*B, where: 
  //    k=1380e-23 mW/sK,
  //    T=290K, 
  //    B=bandwidth
  noiseFloor = 1380e-23*290*bandwidth;

  noiseFloordBm = 10*log10(noiseFloor);

  // Path loss exponent of log-distance propagation model
  alpha = atof(getParam(argc, argv)); 

  // Reference distance of log-distance propagation model (meters)
  d0 = 1.0; 

  // L0 -> Loss in dB at the reference distance (d0) using free-space model;
  // Pt/Pr = (4*PI*f*/c*d)**2
  // L0dB = 20*log10(4*M_PI*freq*d0/3e8);

  // This means Pr(d<=d0)=Pt;
  L0dB=0.0;

  // Minimum SINR needed to decode a transmission (in dB)
  betadB = atof(getParam(argc, argv)); 
  // Converting to ratio
  beta = pow(10,betadB/10.0);

  PtmW = atof(getParam(argc, argv));

  PtdBm = 10*log10(PtmW);
  
  // Loss in dB at the distance d
  // L = 10*alpha*log10(d/d0)
  // So, Pt (dBm) = L(dB) + noiseFloor(dBm) + beta(dB) + L0(dB) 
  //PtdBm = 10*alpha*log10(maxRange/d0) + noiseFloordBm + betadB + L0dB;

  maxRange = d0*pow(10,(PtdBm-noiseFloordBm-betadB-L0dB)/(10*alpha));

  // Loss in dB at the distance d
  // L = 10*alpha*log10(d/d0)
  // So, Pt (dBm) = L(dB) + noiseFloor(dBm) + beta(dB) + L0(dB) 
  // PtdBm = 10*alpha*log10(maxRange/d0) + noiseFloordBm + betadB + L0dB;

  // Plotting parameters
  // fprintf(stderr,"Pt=%lf (dBm) PtmW=%lf (mW) beta=%lf (dB) alpha=%lf (dimensionless) d0=%lf (m) noiseFloor=%lf (dBm) maxRange=%lf (m)\n", PtdBm, PtmW, betadB, alpha, d0, noiseFloordBm, maxRange);

  outputfile1 = getParam(argc, argv); // Name of the output file 1

  outputfile2 = getParam(argc, argv); // Name of the output file 2
  
  run = atoi(getParam(argc, argv)); // Number of simulation run

  // Setting the seed of random number generator as the simulation run
  srand(run);
}

void placeNodes() {

  int i;

  for (i=0; i < numNodes; ++i) {
    posX[i]= uniform()*areaSide;
    posY[i]= uniform()*areaSide;
  }
}

// Procedure that generate matrices needed by the scheduling algorithms
void generateStructures() {

  // Generating the distance and the power reception matrices
  // and links

  int i;

  edges=15;
  numNodes=10;

  for (i = 0; i < edges; i++) {
    links[i].weight = 0.0;
    links[i].scheduled = 0;
  }

  links[0].sender = 0;
  links[0].recver = 1;

  links[1].sender = 1;
  links[1].recver = 2;
  
  links[2].sender = 2;
  links[2].recver = 3;

  links[3].sender = 3;
  links[3].recver = 4;
  
  links[4].sender = 0;
  links[4].recver = 4;

  links[5].sender = 5;
  links[5].recver = 7;
  
  links[6].sender = 7;
  links[6].recver = 9;
  
  links[7].sender = 6;
  links[7].recver = 9;

  links[8].sender = 6;
  links[8].recver = 8;

  links[9].sender = 5;
  links[9].recver = 8;

  links[10].sender = 0;
  links[10].recver = 5;

  links[11].sender = 1;
  links[11].recver = 6;

  links[12].sender = 2;
  links[12].recver = 7;

  links[13].sender = 3;
  links[13].recver = 8;

  links[14].sender = 4;
  links[14].recver = 9;

  if (edges > 0) {
    fprintf(stderr,"nr of edges: %d\n",edges);
    for (i = 0; i < edges; i++) {
      fprintf(stderr,"link %d: %d-%d - on=%d - w=%f\n",i, links[i].sender, links[i].recver, links[i].weight, links[i].scheduled);
    }
  } else {
    exit(0);
  }
}

void initialize(void) {

  int i,j;
  
  for(i = 0; i < numNodes; i++) {
    for(j = 0; j < numNodes; j++) {
      K[i][j]=0;
      interference[j]=0.0;
    }
  }
}

void adjustInterference(int sender, int recver) {

  int i;
  
  for(i = 0; i < numNodes; i++) {
    if ( i != sender && i != recver ) {
      interference[i] = interference[i] + PrmW[sender][i];
    }
  }		
}

int addLink(int id) {

  int i, j;

  // Is the links(id) feasible in this slot?

  for(i = 0; i < numNodes; i++) {
    for(j = 0; j < numNodes; j++) {
      // foreach edge scheduled in this slot...
      if (K[i][j] == 1) {
	// if ltmp(id) has same endpoint...
	if (ltmp[id].sender == i || ltmp[id].sender == j || ltmp[id].recver == i || ltmp[id].recver == j) {
	  // is not feasible
	  //fprintf(stderr, "%d: %d-%d is not feasible - same endpoint that %d-%d \n",id,ltmp[id].sender, ltmp[id].recver, i, j);
	  return false;
	} else {
	  // or if ltmp(id) power at some receiver added to interference is greater than minSINR or vice-versa
	  if (PrmW[i][j]/(interference[j] + PrmW[ltmp[id].sender][j])  < beta  || PrmW[ltmp[id].sender][ltmp[id].recver]/interference[ltmp[id].recver] < beta )  {
	    // is not feasible
	    //fprintf(stderr, "%d: %d-%d is not feasible - PrmWij[%d,%d] = %e - PrmWe[%d,%d] = %e, interference[%d] = %e, PrmWsenderj = %e \n",id,ltmp[id].sender, ltmp[id].recver,i,j,PrmW[i][j], ltmp[id].sender, ltmp[id].recver,PrmW[ltmp[id].sender][ltmp[id].recver], j, interference[j], PrmW[ltmp[id].sender][j]);
	    // return false;
	  } else {
	    // fprintf(stderr, "%d: %d-%d is feasible\n",id,ltmp[id].sender, ltmp[id].recver);
	  }
	}
      }
    }
  }
  // otherwise is feasible
  return true;
}

int isFeasible() {
	 
  int i,j;

  initialize();

  for(i = 0; i < edges; i++) {
    
    if (ltmp[i].scheduled) {
      // first edge
      K[ltmp[i].sender][ltmp[i].recver]=1;
      adjustInterference(ltmp[i].sender, ltmp[i].recver);
    
      for (j = 0; j < edges; j++) {
      
	if (ltmp[j].scheduled && (j!=i)) {
	  if (addLink(j)) {
	    K[ltmp[j].sender][ltmp[j].recver]=1;
	    adjustInterference(ltmp[j].sender, ltmp[j].recver);
	  } else 
	    return false;
	}
      }
      return true;
    }
  }
  return true;
}

glp_prob* generateLPProblem() {

  glp_prob* lp;

  int i;

  char buf[100];

  // create problem
  lp = glp_create_prob();
  glp_set_prob_name(lp, "multicoloring");

  // maximization
  glp_set_obj_dir(lp, GLP_MAX);

  // number of edges/columns
  glp_add_cols(lp, edges);

  for (i=1; i<=edges; i++) {
    sprintf(buf,"%s%d","x",i);
    glp_set_col_name(lp, i, buf);
    
    glp_set_obj_coef(lp, i, 1.0);
    glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0);
  }

  return lp;
}

void addConstraints2C(glp_prob* lp,int id,double *e) {

  fprintf(stderr,"Adding constraint %d to C\n",id);

  int l,m;
  int ja[1+edges];
  double ar[1+edges];

  // new row
  int row = glp_add_rows(lp, 1);
  // fprintf(stderr,"%d\t\t",row);

  l=1;
  for (m=1; m<=edges; m++) {
    ja[m] = l, ar[m] =  e[m-1];
    // fprintf(stderr,"%d: 1,%d --> %f\t", m,l,ar[m]);
    ++l;
  }
  fprintf(stderr,"\n");
  
  // variable with upper bound
  glp_set_row_bnds(lp, id, GLP_UP, 0.0, 1.0);

  char buf[100];
  sprintf(buf,"%s%d","c",id);
  glp_set_row_name(lp, id, buf);

  glp_set_mat_row(lp, id, edges, ja, ar);
}

void print_results(glp_prob* lp) {

  fprintf(stderr,"========= Printing results ========\n");

  int i, j, len, ind[1+1000];

  double val[1+1000];
  
  unsigned int m=0;
  unsigned int colors=0;

  for (j=0; j<edges; j++) {
    links[j].scheduled=0;
  }

  int status = glp_get_status(lp);
  int status_prim = glp_get_prim_stat(lp);
  int status_dual = glp_get_dual_stat(lp);

  if (status != GLP_OPT) exit(1);
      
  if ((status_prim == GLP_FEAS) && (status_dual == GLP_FEAS)) {

    int nr_rows = glp_get_num_rows(lp);
    fprintf(stderr,"nr of rows %d\n",nr_rows);
    for (i=1; i<=nr_rows; i++) {
      if ((glp_get_row_stat(lp,i) != GLP_BS) && (glp_get_row_dual(lp, i) != 0)) {
	++m;
	for (j=0; j<edges; j++) {
	  ltmp[j].scheduled=0;
	}
	fprintf(stderr,"matching %d: %f\n",m, glp_get_row_dual(lp, i));
	len = glp_get_mat_row(lp,i,ind,val);
	fprintf(stderr,"links:");
	for (j=1; j<=len; j++) {
	  fprintf(stderr,"\t%d",ind[j]-1);
	  links[ind[j]-1].scheduled = links[ind[j]-1].scheduled + 1;
	  if (colors < links[ind[j]-1].scheduled)
	    colors = links[ind[j]-1].scheduled;
	  ltmp[ind[j]-1].scheduled = 1;
	  ltmp[ind[j]-1].sender = links[ind[j]-1].sender;
	  ltmp[ind[j]-1].recver = links[ind[j]-1].recver;
	}
	int isfeasible = isFeasible();
	fprintf(stderr,"\tfeasibility %d\n",isfeasible);
      }
    }
    
    for (j=0; j<edges; j++) {
      fprintf(stderr,"link %d: scheduled %d times from %d expected\n",j,links[j].scheduled,colors);
    }

    glp_print_sol(lp, outputfile1);

    FILE* f1 = fopen(outputfile2, "w+");

    // unbuffered mode
    setvbuf (f1, NULL, _IONBF, 0);

    double gain = m * 1.0/edges/colors;
   
    fprintf(f1, "%d %d %d %d %f \n", numNodes, edges, m, colors, gain);

    fclose(f1);
    
  } else if (status_prim = GLP_UNDEF)
    fprintf(stderr,"Undefined Solution\n");
}

int combinations (int v[], int start, int n, int k, int maxk, int init_id, glp_prob* lp) {

  int i;

  int isfeasible;
  
  if (k == maxk) {

    double edge[edges];
    
    int sched;

    // nenhum link no conjunto
    for (i = 0; i < edges; i++) {
      ltmp[i].scheduled = 0;
      edge[i] = 0.0;
    }  

    // crio conjunto de links a serem escalonados
    for (i = 0; i < maxk; i++) {
      fprintf(stderr,"%d\t",v[i]);
      sched = v[i];
      ltmp[sched].scheduled = 1;
      ltmp[sched].sender = links[sched].sender;
      ltmp[sched].recver = links[sched].recver;
    }

    isfeasible = isFeasible();

    if (isfeasible) {
      fprintf(stderr,"feasible\t");

      for (i = 0; i < maxk; i++) {
	sched = v[i];
	edge[sched] = 1.0;
      }
      addConstraints2C(lp,init_id,edge);
      ++init_id;
    } else fprintf(stderr,"unfeasible\n");
  
    return init_id;
  }
  
  /* for this k'th element of the v, try all start..n
   * elements in that position
   */
  for (i=start; i<=n; i++) {
    
    v[k] = i;
   /* recursively generate combinations of integers
     * from i+1..n
     */
    init_id = combinations (v, i+1, n, k+1, maxk, init_id, lp);
    
  }

  return init_id;
}

int addNewConstraints(glp_prob* lp,int init_id,int msize) {

  int p[edges];
  
  int c_id;

  fprintf(stderr,"Combination of %d links\n",msize);
  
  c_id = combinations (p, 0, edges-1, 0, msize, init_id, lp);

  return c_id;
}

// Main procedure
int main(int argc, char** argv) {

  int i,c;

  double edge[1+1000];

  int msize;
  
  readParams(argc, argv);

  //  placeNodes();

  generateStructures();

  glp_prob *lp = generateLPProblem();

  // define simplex method
  glp_smcp p;
  glp_init_smcp(&p);
  p.meth = GLP_PRIMAL;
  p.presolve = GLP_ON;

  // adding initial constraints to C: one per link
  for (c = 0; c < edges; c++) {
    for (i = 0; i < edges; i++) {
      if (i == c) edge[i]=1.0;
      else edge[i]=0.0;
    }
    addConstraints2C(lp,c+1,edge);
  }

  //  msize = edges;
  msize=2;

  // global constraints id
  int c_id = edges+1;

  int solution;

  int oldid = c_id;

  double w, y, z;
    
  int status, status_prim, status_dual;

  double primal;
  
  while (msize < edges+1) {
    
    c_id = addNewConstraints(lp,oldid,msize);
    ++msize;

    if (oldid != c_id) {

      oldid = c_id;
      
      // running LP
      glp_std_basis(lp);
      
      solution = glp_simplex(lp, &p);
      if (solution == 0) { //successful
	// primal two-phase simplex method based on exact (rational) arithmetic
	solution = glp_exact(lp,&p);
      }
      
      status = glp_get_status(lp);
      status_prim = glp_get_prim_stat(lp);
      status_dual = glp_get_dual_stat(lp);
      
      fprintf(stderr,"STATUS: %d %d %d %d\n",solution, status, status_prim, status_dual);
      
      w=0.0;
      // getting LP results 
      z = glp_get_obj_val(lp);
      fprintf(stderr,"\nz = %f;\t",z);
      for (i=1; i<=edges; i++) {
	y  = glp_get_col_prim(lp, i);
	w += y;
	fprintf(stderr,"x%d = %f\t",i, y);
      }
      fprintf(stderr,"\n");
      
      fprintf(stderr,"w = %2.16f\n",w);
      
      primal = w;
      
      w=0.0;
      // getting LP results 
      for (i=1; i<c_id; i++) {
	y = glp_get_row_dual(lp, i);
	w +=y;
	if (y != 0) fprintf(stderr,"y%d = %f\t",i, y);
      }
      fprintf(stderr,"\n");
      
      fprintf(stderr,"w = %2.16f\n",w);
      
      int a = round(1000.0*primal);
      int b = round(1000.0*w);
      fprintf(stderr,"A = %d : B = %d\n",a,b);
      
      if (a != b) return 0;
    }
  }
  
  print_results(lp);
  
  glp_delete_prob(lp);
}

