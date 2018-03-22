//
// Created by swee on 3/21/18.
//

#include "Astar.h"

Astar::Astar(Node root, Node goal,int lenh,double* heading,int lenv,double *vs,double deltaT):
        Root(NULL,root.index,root.x,root.y,root.z,root.psi,root.vs,root.speed),
        Goal(NULL,goal.index,goal.x,goal.y,goal.z,goal.psi,goal.vs,goal.speed){
    currentNode = &root;
    closestDist = MAXDOUBLE;
    HEADING = heading;
    VS = vs;
    dt = deltaT;
    lenH = lenh;
    lenV = lenv;
}

void Astar::SetBoundary(Poly3D *boundary) {
    keepInFence = *boundary;
}

void Astar::InputObstacle(Poly3D *obs) {
    keepOutFence.push_back(*obs);
}

bool Astar::Visited(Node *qnode) {
    std::list::iterator it;
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
        std::list::iterator it;
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
                Frontier.pop();
                continue;
            }
            else{
                printf("No more nodes on the frontier\n");
                break;
            }
        }

        VisitedList.push_back(*currentNode);

        currentNode->GenerateChildren(lenH,lenV,HEADING,VS,dt,&nodeList);

        std::list::iterator it;
        for(it = currentNode->children.begin(); it != currentNode->children.end();++it){
            if(!CheckConstraints(*it)){
                continue;
            }


            it->g = currentNode->g + currentNode->NodeDist(*it) + 0.1*fabs(currentNode->psi - it->psi)
                                                                + 0.1*fabs(currentNode->vs - it->vs);

            it->h = it->NodeDist(Goal);
            Frontier.push(*it);
        }


    }
}