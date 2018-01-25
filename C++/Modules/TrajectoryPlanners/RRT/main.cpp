#include <iostream>
#include <stdlib.h>
#include "RRTplanner.h"

void F(double X[], double U[],double Y[],int trafficSize){

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

void U(node_t& nn, node_t& qn,double U[]){
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


int main(int argc,char* argv[]){

    srand(time(NULL));


    // create bounding box
    Poly2D box;
    box.addVertex(0,0);
    box.addVertex(100,0);
    box.addVertex(100,100);
    box.addVertex(0,100);
    Poly3D bbox(box,0,100);

    // obstacles
    Poly2D obs2D;
    obs2D.addVertex(10,10);
    obs2D.addVertex(20,10);
    obs2D.addVertex(20,20);
    obs2D.addVertex(10,20);
    Poly3D obs1(obs2D,-100,100);

    Poly2D obs2D_2;
    obs2D_2.addVertex(30,0);
    obs2D_2.addVertex(60,0);
    obs2D_2.addVertex(60,30);
    obs2D_2.addVertex(30,30);
    Poly3D obs2(obs2D_2,-100,100);

    std::list<Poly3D> obstacleList;
    obstacleList.push_back(obs1);

    int Nsteps = 2000;
    int dT = 1;
    int dTsteps = 5;
    double maxD = 5.0;
    double maxInputNorm = 1.0;


    RRTplanner RRT(bbox,dTsteps,dT,maxD,maxInputNorm,F,U,"../DaidalusQuadConfig.txt");


    Vect3 pos(1,1,0);
    Vect3 vel(1,0,0);
    Vect3 trafficPos1(90,0,0);
    Vect3 trafficVel1(-1,0,0);

    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;

    TrafficPos.push_back(trafficPos1);
    TrafficVel.push_back(trafficVel1);

    Vect3 gpos(90,90,0);
    node_t goal;
    goal.pos = gpos;


    RRT.Initialize(pos,vel,obstacleList,TrafficPos,TrafficVel,goal);


    for(int i=0;i<Nsteps;i++){

        RRT.RRTStep();

        if(RRT.CheckGoal()){
            //printf("%f,%f\n",0.0,0.0);
            break;
        }
    }

    node_t* node = &RRT.GetNodeList()->back();
    node_t* parent;
    while(node->parent != NULL){
        printf("%f,%f,%f,%f\n",node->pos.x,node->pos.y,
               node->trafficPos.front().x,node->trafficPos.front().y);
        parent = node->parent;
        node = parent;
    }

}