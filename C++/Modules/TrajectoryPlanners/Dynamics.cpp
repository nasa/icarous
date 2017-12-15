//
// Created by Swee Balachandran on 12/14/17.
//


#include "Dynamics.h"

void RRT_F(double X[], double U[],double Y[],int trafficSize){

    double Kc = 0.3;

    Y[0] = X[1];
    Y[1] = -Kc*(X[1] - U[0]);
    Y[2] = X[3];
    Y[3] = -Kc*(X[3] - U[1]);
    Y[4] = X[5];
    Y[5] = -Kc*(X[5] - U[2]);

    // Constant velocity for traffic
    for(int i=0;i<trafficSize;++i){
        Y[6+(6*i)+0]   = X[6+(6*i)+1];
        Y[6+(6*i)+1]   = 0;
        Y[6+(6*i)+2]   = X[6+(6*i)+3];
        Y[6+(6*i)+3]   = 0;
        Y[6+(6*i)+4]   = X[6+(6*i)+5];
        Y[6+(6*i)+5]   = 0;
    }
}

void RRT_U(node_t& nn, node_t& qn,double U[]){
    double dx, dy, dz;
    double norm;
    double maxInputNorm = 1.0;

    dx = qn.pos.x - nn.pos.x;
    dy = qn.pos.y - nn.pos.y;
    dz = qn.pos.z - nn.pos.z;

    norm = sqrt(pow(dx,2) + pow(dy,2) + pow(dz,2));

    if (norm > maxInputNorm){
        U[0] = dx/norm * maxInputNorm;
        U[1] = dy/norm * maxInputNorm;
        U[2] = dz/norm * maxInputNorm;
    }
    else{
        U[0] = dx;
        U[1] = dy;
        U[2] = dz;
    }
}
