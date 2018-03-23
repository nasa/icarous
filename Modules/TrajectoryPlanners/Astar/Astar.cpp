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

void Astar::SetRoot(double x, double y, double z, double psi,double speed) {
    Root.index = 0;
    Root.x = x;
    Root.y = y;
    Root.z = z;
    Root.psi = psi;
    Root.vs = 0;
    Root.g = 0;
    Root.neighborhood = nhood;
    Root.parent = NULL;
    Root.speed = speed;
    currentNode = new Node(Root);

}

void Astar::SetGoal(double x, double y, double z) {
    Goal.x = x;
    Goal.y = y;
    Goal.z = z;
    Root.h = sqrt(pow((x - Root.x),2) + pow((y - Root.y),2) + pow((x - Root.x),2));
}

bool Astar::Visited(const Node qnode) {
    std::list<Node>::iterator it;
    for(it = VisitedList.begin(); it != VisitedList.end(); ++it){
        Node _node = *it;
        double dist = qnode.NodeDist(_node);
        if (dist < qnode.neighborhood){
            return true;
        }
    }
}

bool Astar::CheckConstraints(Node& qnode) {

    Vect3 A(qnode.x,qnode.y,qnode.z);
    bool val;

    Vect3 O(qnode.parent->x,qnode.parent->y,qnode.parent->z);
    Vect3 OA = A.Sub(O);
    double dist = OA.norm();

    Vect2 vel = OA.vect2();

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

            std::vector<Vect2> fenceVertices = it->poly2D().getVertices();
            Vect2 polyVel(0,0);
            val = geoPolyDetect.Static_Collision_Detector(0,1,fenceVertices,polyVel,A.vect2(),vel,0.1,true);

            if(val){
                return false;
            }
        }
    }

    return true;

}

bool Astar::ComputePath() {
    while (!currentNode->GoalCheck(Goal)){

        if (currentNode->h < closestDist){
            closestDist = currentNode->h;
        }

        if(Visited(*currentNode)){
            if(Frontier.size() > 0){
                currentNode = new Node(Frontier.front());
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
            Node _node = (*it);
            if(!CheckConstraints(_node)){
                continue;
            }


            _node.g = currentNode->g + currentNode->NodeDist(_node) + 0.1*fabs(currentNode->psi - _node.psi)
                                                                + 0.1*fabs(currentNode->vs - _node.vs);

            _node.h = _node.NodeDist(Goal);
            Frontier.push_back(_node);
        }

        Frontier.sort();


        if (Frontier.size() > 0){
            currentNode = new Node(Frontier.front());
            Frontier.pop_front();
        }else{
            printf("End of frontier\n");
        }
    }

    //printf("Found path\n");
    Node *_cn = currentNode;
    while(_cn){
        Path.push_back(*_cn);
        _cn = _cn->parent;
    }

    if (Path.size()>1){
        return true;
    }else{
        return false;
    }
}

std::list<Node> Astar::GetPath() {
    return Path;
}

Astar::~Astar() {
    Node *_cn = currentNode;
    currentNode = currentNode->parent;
    delete(_cn);
    while(currentNode){
        _cn = currentNode;
        currentNode = currentNode->parent;
        delete(_cn);
    }
}