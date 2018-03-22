//
// Created by swee on 3/21/18.
//

#include "Astar.h"
#include <algorithm>

Astar::Astar(int lenh,double* heading,int lenv,double *vs,double deltaT,double eps){
    closestDist = MAXDOUBLE;
    HEADING = heading;
    VS = vs;
    dt = deltaT;
    lenH = lenh;
    lenV = lenv;
    nhood = eps;
}

void Astar::SetBoundary(Poly3D *boundary) {
    keepInFence = *boundary;
}

void Astar::InputObstacle(Poly3D *obs) {
    keepOutFence.push_back(*obs);
}

void Astar::SetRoot(double x, double y, double z, double psi) {
    Root.index = 0;
    Root.x = x;
    Root.y = y;
    Root.z = z;
    Root.psi = psi;
    Root.vs = 0;
    Root.vx = 0;
    Root.vy = 0;
    Root.vz = 0;
    Root.g = 0;
    Root.h = 0;
    Root.neighborhood = nhood;
    Root.parent = NULL;
}

void Astar::SetGoal(double x, double y, double z) {
    Goal.x = x;
    Goal.y = y;
    Goal.z = z;
}

bool Astar::Visited(Node *qnode) {
    std::list<Node>::iterator it;
    for(it = VisitedList.begin(); it != VisitedList.end(); ++it){
        double dist = qnode->NodeDist(*it);
        if (dist < qnode->neighborhood){
            return true;
        }
    }
}

bool Astar::CheckConstraints(Node qnode) {

    Vect3 A(qnode.x,qnode.y,qnode.z);
    bool val;
    if (keepInFence.size() > 0){
        val = geoPolyCarp.definitelyInside(A,keepInFence);

        if (!val){
            return false;
        }
    }

    if (keepOutFence.size() > 0){
        std::list<Poly3D>::iterator it;
        for(it = keepOutFence.begin();it != keepOutFence.end();++it){
            val = geoPolyCarp.definitelyOutside(A,*it);

            if (!val){
                return false;
            }
        }
    }

    // Check collision on points in between parent and child.


}

void Astar::GetPath() {
    while (currentNode->GoalCheck(Goal)){

        if (currentNode->h < closestDist){
            closestDist = currentNode->h;
        }

        if(Visited(currentNode)){
            if(Frontier.size() > 0){
                currentNode = &Frontier.front();
                Frontier.pop_front();
                continue;
            }
            else{
                printf("No more nodes on the frontier\n");
                break;
            }
        }

        VisitedList.push_back(*currentNode);

        currentNode->GenerateChildren(lenH,lenV,HEADING,VS,dt,&nodeList);

        std::list<Node>::iterator it;
        for(it = currentNode->children.begin(); it != currentNode->children.end();++it){
            Node *_node = (Node*)&(*it);
            if(!CheckConstraints(*_node)){
                continue;
            }


            _node->g = currentNode->g + currentNode->NodeDist(*_node) + 0.1*fabs(currentNode->psi - _node->psi)
                                                                + 0.1*fabs(currentNode->vs - _node->vs);

            _node->h = _node->NodeDist(Goal);
            Frontier.push_back(*_node);
        }

        Frontier.sort();

        if (Frontier.size() > 0){
            currentNode = &Frontier.front();
            Frontier.pop_front();
        }else{
            printf("End of frontier\n");
        }
    }

    Path.push_back(*currentNode);
    while(currentNode->parent){
        Path.push_back(*(currentNode->parent));
        currentNode = currentNode->parent;
    }
}