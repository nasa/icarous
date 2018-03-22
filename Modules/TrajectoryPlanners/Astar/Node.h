//
// Created by swee on 3/21/18.
//

#ifndef ASTAR_NODE_H
#define ASTAR_NODE_H

#include <list>

class Node {
public:
    int index;
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;

    double psi;
    double vs;
    double g;
    double h;
    double speed;
    std::list<Node> children;
    double neighborhood;
    Node* parent;
    Node();
    Node(Node* parent,int index,double x,double y,double z,double psi,double vs,double speed);
    bool GoalCheck(Node goal);
    bool AddChild(Node child);
    void GenerateChildren(int lenH,int lenV,double* heading, double* vspeed, double dt, std::list<Node> *nodeList);
    double NodeDist(Node B);

    bool operator<(Node& B);
    bool operator!=(Node& B);

};


#endif //ASTAR_NODE_H
