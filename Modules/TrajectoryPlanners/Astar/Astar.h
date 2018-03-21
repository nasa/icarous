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
    std::list<Node> VisitedList;
    std::queue<Node> Frontier;
    std::list<Node> Path;
    Node Root;
    Node Goal;
    Node* currentNode;
    double closestDist;
    CDPolycarp geoPolyCarp;
    Poly3D keepInFence;
    std::list<Poly3D> keepOutFence;

public:
    Astar(Node root,Node goal);
    void SetBoundary(Poly3D* boundary);
    void InputObstacle(Poly3D* obs);
    void GetPath();
    bool Visited(Node *qnode);

};


#endif //ASTAR_ASTAR_H
