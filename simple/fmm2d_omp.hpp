/*******************************************************************************
File fmm2d.h is an include file for program fmm2d.c.
*******************************************************************************/
#ifndef FMM2DOMP_H
#define FMM2DOMP_H

#include <vector>

#define BOX 1.0          /* Cubic simulation-box size */
#define Npar  16000          /* # of charged particles */
#define L 6              /* Max level of refinement = quadtree height */
#define P 6              /* Order of multipole & local expansions */
#define NUM_THREADS 8    /* # of threads for OpenMP */
#define Max_par 1000000  /* Array size for particles */
#define Max_cell 1000000 /* Array size for quadtree cells */
#define Max_level 20     /* Array size for quadtree levels (>= L+1) */
#define Max_term 20      /* Array size for multipole|local terms (>= P) */
#define EMPTY -1         /* NULL pointer in linked lists */

/* Variables for timing measurement *******************************************/

double tfmm;             /* Wall-clock time for FMM */
double tdirect;          /* Wall-clock time for direct calculation */

/* Functions & function prototypes ********************************************/

void initialize();
void mp_leaf();
void upward();
void downward();
void nn_direct();
void all_direct();

/* Complex-arithmetic functions */
void cadd(double s, double* a, double t, double* b, double* c);
void cadd(double s, std::vector<double>& a, double t, double* b, std::vector<double>& c);
void cadd(double s, std::vector<double>& a, double t, std::vector<double>& b, std::vector<double>& c);
void smul(double* a, double s, double* c);
void smul(std::vector<double>& a, double s, double* c);
void cmul(double* a, double* b, double* c);
void cmul(std::vector<double>& a, double* b, double* c);
void cinv(double* a, double* ai);
void clgn(double* a, double* l);
void cini(double s, double t, double* a);
void cini(double s, double t, std::vector<double>& a);

/* Combinatorics functions */
int fact(int n);
int comb(int n, int k);

/* Variables ******************************************************************/
extern std::vector<std::vector<double>> z;                  /* z[j][0|1] is the x|y coordinate of particle j */
extern std::vector<double> q;                               /* q[j] is the charge of particle j */
extern std::vector<std::vector<std::vector<double>>> phi;   /* phi[c][a][0|1] is the real|imaginary part of the
                                                               a-th order multipole of cell c */
extern std::vector<std::vector<std::vector<double>>> psi;   /* psi[c][a][0|1] is the real|imaginary part of the
                                                               a-th order local-expansion term of cell c */
extern std::vector<int> c0;                                 /* c0[l] is the cell-index offset at level l */
extern std::vector<double> pot;                             /* Potential at j-th particle position */
extern std::vector<double> pot_direct;                      /* Potential by all-pair direct calculation */
extern std::vector<int> head;                               /* Headers for linked lists */
extern std::vector<int> lscl;                               /* Linked lists for particles in the leaf cells */

double eng;           /* Total electrostatic energy */
double eng_direct;    /* Electrostatic energy by all-pair direct calculation */
/******************************************************************************/
#endif