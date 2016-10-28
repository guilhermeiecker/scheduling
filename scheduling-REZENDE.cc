// scheduling3 10 1000 4 25 300 f1 f2 0/1 4
// scheduling3 10 1000 4 25 300 f1 f2 0/1 5

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <glpk.h>
#include "scheduling.h"

#include <iostream>

#include <soplex.h>

using namespace soplex;

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

  // delta_mi = atof(getParam(argc, argv));
  
  outputfile1 = getParam(argc, argv); // Name of the output file 1

  outputfile2 = getParam(argc, argv); // Name of the output file 2

  lptool = atoi(getParam(argc, argv)); // LP tool to be used (0 = glpk - 1 = soplex)
  
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

  int i,j;
  float PrdBm;

  edges=0;
  for (i = 0; i < numNodes; i++) {
    grau[i]=0;
    for (j = 0; j < numNodes; j++) {
      if (i!=j) {
	dist[i][j] = sqrt(pow(posX[i]-posX[j],2) + pow(posY[i]-posY[j],2));
	if (dist[i][j] > d0) {
	  PrdBm = PtdBm - L0dB - 10*alpha*log10(dist[i][j]/d0);
	  PrmW[i][j] = pow(10.0, PrdBm/10.0);
	} else {
	  PrdBm = PtdBm - L0dB;
	  PrmW[i][j] = pow(10.0, PrdBm/10.0);
	}
	if (dist[i][j] <= maxRange) {
	  if (i<j) {
	    links[edges].sender = i;
	    links[edges].recver = j;
	    links[edges].weight = 0.0;
	    links[edges].scheduled = 0;
	    ++edges;
	    ++grau[i];
	  }
	}
      }	else {
	PrmW[i][j] = 0.0;
	dist[i][j] = 0.0;
      }
    }
  }

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
	//fprintf(stderr, "Testando link %d: K[%d,%d]: %d\n",id,i,j,K[i][j]);
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
	    // Obs: Descomentar a linha abaixo para a physical feasibility
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
	    //fprintf(stderr,"Link added %d\n",j);
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

void addmatching(double *e, int id) {

  int i;
  
  /* add constraints one by one */
  fprintf(stderr,"matching %d:\t",id);
  for (i=0; i<edges; i++) {
    matchings[id][i] = (unsigned int) e[i];
    if (e[i])
      fprintf(stderr,"%d\t",i);
  }
  fprintf(stderr,"\n");
}

int combinations (int v[], int start, int n, int k, int maxk, int init_id) {

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
      // fprintf(stderr,"%d\t",v[i]);
      sched = v[i];
      ltmp[sched].scheduled = 1;
      ltmp[sched].sender = links[sched].sender;
      ltmp[sched].recver = links[sched].recver;
    }

    isfeasible = isFeasible();

    if (isfeasible) {
      //fprintf(stderr,"feasible\n");

      for (i = 0; i < maxk; i++) {
	sched = v[i];
	edge[sched] = 1.0;
      }
      addmatching(edge,init_id);
      ++init_id;
    }
    //else fprintf(stderr,"unfeasible\n");
  
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
    init_id = combinations (v, i+1, n, k+1, maxk, init_id);
    
  }

  return init_id;
}

int addConstraints(int init_id,int msize) {

  int p[edges];
  
  int c_id;

  fprintf(stderr,"Combination of %d links\n",msize);
  
  c_id = combinations (p, 0, edges-1, 0, msize, init_id);

  return c_id;
}

int generateSoplexProblem(SoPlex* m, int nrofmatchings) {

  int i,j;
  
  /* set the objective sense */
  m->setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);
  m->setIntParam(SoPlex::SYNCMODE, SoPlex::SYNCMODE_AUTO);
  m->setIntParam(SoPlex::SOLVEMODE, SoPlex::SOLVEMODE_RATIONAL);
    
  /* we first add variables */
  DSVector dummycol(0);

  for (i=0; i<nrofmatchings; i++) {
    m->addColReal(LPCol(1.0, dummycol, infinity, 0));
  }

  for (i=0; i<edges; i++) {
    DSVector row(1);
    for (j=0; j<nrofmatchings; j++) {
      if (matchings[j][i] == 1.0) {
	/* add constraints one by one */
	row.add(j,1.0);
      }
    }
    // == 1
    m->addRowReal(LPRow(1.0, row, 1.0));
    // >= 1
    //m->addRowReal(LPRow(1.0, row, infinity));
  }
}

glp_prob* generateGLPKProblem(int nrofmatchings) {

  glp_prob* lp;

  int i;

  int id=1;
  
  char buf[100];

  // create problem
  lp = glp_create_prob();
  glp_set_prob_name(lp, "multicoloring");

  // minimization
  glp_set_obj_dir(lp, GLP_MIN);

  // number of matchings/columns
  glp_add_cols(lp, nrofmatchings);

  for (i=1; i<=nrofmatchings; i++) {
    sprintf(buf,"%s%d","x",i);
    glp_set_col_name(lp, i, buf);
    
    glp_set_obj_coef(lp, i, 1.0);

    // variables >= 0
    glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0);
  }

  int l,m;
  int ja[1+nrofmatchings];
  double ar[1+nrofmatchings];
  
  for (i=0; i<edges; i++) {
    
    // new row
    int row = glp_add_rows(lp, 1);
    fprintf(stderr,"%d\t\t",row);

    l=1;
    for (m=1; m<=nrofmatchings; m++) {
      ja[m] = l, ar[m] =  matchings[m-1][i];
      if (ar[m] == 1.0)
      	fprintf(stderr,"%d\t", l);
      ++l;
    }
    fprintf(stderr,"\n\n\n");
    
    // fixed
    // sum == 1
    glp_set_row_bnds(lp, id, GLP_FX, 1.0, 1.0);

    // with lower bound
    // sum >= 1
    // glp_set_row_bnds(lp, id, GLP_LO, 1.0, 1.0);
    
    char buf[100];
    sprintf(buf,"%s%d","c",id);
    glp_set_row_name(lp, id, buf);
    
    //fprintf(stderr,"Adding constraint %d - %s\n",id,buf);

    glp_set_mat_row(lp, id, nrofmatchings, ja, ar);

    ++id;
  }

  return lp;
}

// Main procedure
int main(int argc, char** argv) {

  int i,j;

  // global constraints id
  int c_id=0;
  
  int msize=2;
  
  readParams(argc, argv);

  placeNodes();

  generateStructures();

  double edge[1+1000];

  // adding initial constraints: one per link
  for (i = 0; i < edges; i++) {
    for (j = 0; j < edges; j++) {
      if (j == i) edge[j]=1.0;
      else edge[j]=0.0;
    }
    addmatching(edge,c_id);
    ++c_id;
  }

  int oldid = c_id;

  while (msize < edges+1) {
    
    c_id = addConstraints(oldid,msize);
    ++msize;
    if (oldid != c_id) {
      oldid = c_id;
    }
  }

  int nrofmatchings = c_id-1;
  
  fprintf(stderr,"nr of matchings = %d\n",nrofmatchings);

  if (lptool) {

    SoPlex lp;

    generateSoplexProblem(&lp, nrofmatchings);

    /* write LP in .lp format */
    lp.writeFileReal("dump.lp", NULL, NULL, NULL);

    /* solve LP */
    SPxSolver::Status stat;
    stat = lp.solve();
    double o, objvalue;

    DVector prim(nrofmatchings), dual(edges);
    DVectorRational primR(nrofmatchings), dualR(edges);
    
    LPRowReal matching;
  
    /* get solution */
    if( stat == SPxSolver::OPTIMAL )  {
      std::cout << "LP solved to optimality.\n";
      objvalue = lp.objValueReal();
      o = 0.0;
      std::cout << "Objective value is " << objvalue << ".\n";
      lp.getPrimalReal(prim);
      lp.getPrimalRational(primR);
      
      std::cout << "Primal solution is [";
      for (i = 0; i < nrofmatchings; i++) {
	std::cout << i << "\t" << prim[i] << "\t" << "\t";
      }
      std::cout << "].\n";
      
      for (i = 0; i < nrofmatchings; i++) {
	if (prim[i] != 0) {
	  printf("%d\t%.20f \t ou \t",i,prim[i]);
	  std::cout << primR[i] << "\t -----> \t";
	  for (j=0; j<edges; j++) {
	    if (matchings[i][j])
	      std::cout << j << "\t";
	  }
	  std::cout << "\n";
	}
      }

      lp.getDualRational(dualR);
      lp.getDualReal(dual);
      std::cout << "Dual solution is:\n";
      
      for (i = 0; i < edges; i++) {
	if (dual[i] != 0) {
	  o = o + (double) dual[i];
	  printf("%d\t%.20f\t",i,dual[i]);
	  std::cout << "ou \t" << dualR[i] << "\n";
	  if (dual[i] < 0)
	    printf("ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRO negativo %.12f\n",dual[i]);
	}
      }
      
      if (o != objvalue)
	std::cout << "ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRO " << o << "\t" << objvalue << "\n";
      else
	std::cout << "\n";
    }
  } else {

    glp_prob *lp = generateGLPKProblem(nrofmatchings);
    
    // define simplex method
    glp_smcp p;
    glp_init_smcp(&p);
    p.meth = GLP_PRIMAL;
    p.presolve = GLP_ON;

    double w, y, z, primal;
    
    int solution, status, status_prim, status_dual;

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
    for (i=1; i<=nrofmatchings; i++) {
      y  = glp_get_col_prim(lp, i);
      w += y;
      fprintf(stderr,"x%d = %f\t",i, y);
    }
    fprintf(stderr,"\n");
    
    fprintf(stderr,"w = %2.16f\n",w);
    
    primal = w;
    
    for (i=1; i<=nrofmatchings; i++) {
      y  = glp_get_col_prim(lp, i);
      if (y != 0) {
	fprintf(stderr,"%d \t %f \t -----> \t",i, y);
	for (j=0; j<edges; j++) {
	  if (matchings[i-1][j])
	    fprintf(stderr,"%d \t",j);
	}
	fprintf(stderr,"\n");
      }
    }

    fprintf(stderr,"Dual solution is:\n");
    
    w=0.0;
    // getting LP results 
    for (i=1; i<=edges; i++) {
      y = glp_get_row_dual(lp, i);
      w +=y;
      if (y != 0) fprintf(stderr,"y%d = %f\t",i, y);
    }
    fprintf(stderr,"\n");
    
    fprintf(stderr,"w = %2.16f\n",w);
    
    int a = round(1000.0*primal);
    int b = round(1000.0*w);
    fprintf(stderr,"A = %d : B = %d\n",a,b);

    if (a != b)
      fprintf(stderr,"ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");
    
    glp_delete_prob(lp);
  }
}
