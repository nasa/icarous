#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <vector>
// MAX_SLOTS 
#define MAX_SLOTS 5 
#define INTMAX 255

double  within(int n,double  x,std::vector<std::vector<double> > &F,bool closed,bool min);
void swapD(double * a, double * b);
void swapI(uint8_t * a, uint8_t * b);
int partition (int low, int high,std::vector<double> &arrA,std::vector<double> &arrB,std::vector<uint8_t> &arrC);
void quickSort( int low, int high,std::vector<double>  &arrA,std::vector<double>  &arrB,std::vector<uint8_t> &arrC);
int8_t LeastDeadlineAtT(int n,std::vector<double> &R,std::vector<double> &D,std::vector<uint8_t>  &id,double  t, uint8_t* ScheduledJobs,int numSchJobs);
bool Scheduler(int n,double *R,double *D,double * ScheduledT,uint8_t* ScheduledJobs);


#endif
