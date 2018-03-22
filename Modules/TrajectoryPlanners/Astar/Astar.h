//
// Created by swee on 3/21/18.
//

#ifndef ASTAR_ASTAR_H
#define ASTAR_ASTAR_H

#include <list>
#include <queue>
#include "Node.h"
#include "CDPolycarp.h"

using namespace larcfm;


class Astar {
private:
    double *HEADING;
    double *VS;
    double dt;
    int lenH;
    int lenV;
    double nhood;
    std::list<Node> VisitedList;
    std::list<Node> Frontier;
    std::list<Node> Path;
    std::list<Node> nodeList;
    Node Root;
    Node Goal;
    Node* currentNode;
    double closestDist;
    CDPolycarp geoPolyCarp;
    Poly3D keepInFence;
    std::list<Poly3D> keepOutFence;

public:
    Astar(int lenh,double* heading,int lenv,double *vs,double dt,double eps);
    void SetRoot(double x,double y,double z,double psi);
    void SetGoal(double x,double y,double z);
    void SetBoundary(Poly3D* boundary);
    void InputObstacle(Poly3D* obs);
    void GetPath();
    bool Visited(Node *qnode);
    bool CheckConstraints(Node qnode);

};


#endif //ASTAR_ASTAR_H
