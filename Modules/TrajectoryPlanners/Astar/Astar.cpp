//
// Created by swee on 3/21/18.
//

#include "Astar.h"

Astar::Astar(Node root, Node goal):
        Root(root.index,root.x,root.y,root.z,root.psi,root.vs,root.speed),
        Goal(goal.index,goal.x,goal.y,goal.z,goal.psi,goal.vs,goal.speed){
    currentNode = &root;
    closestDist = MAXDOUBLE;
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

        
    }
}