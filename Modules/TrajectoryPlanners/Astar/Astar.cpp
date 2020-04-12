//
// Created by swee on 3/21/18.
//

#include "Astar.h"
#include <algorithm>
#include <cfloat>

Astar::Astar(int lenh,double* heading,int lenv,double *vs,double deltaT,double eps){
    closestDist = DBL_MAX;
    HEADING = heading;
    VS = vs;
    dt = deltaT;
    lenH = lenh;
    lenV = lenv;
    nhood = eps;
}

void Astar::SetParameters(int lenh,double* heading,int lenv,double *vs,double deltaT,double eps) {
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

}

void Astar::SetGoal(double x, double y, double z) {
    Goal.x = x;
    Goal.y = y;
    Goal.z = z;
    Root.h = sqrt(pow((x - Root.x),2) + pow((y - Root.y),2) + pow((x - Root.x),2));

    currentNode = new Node(Root);
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
    return false;
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

            std::vector<Vect2> fenceVertices = it->poly2D().getVerticesRef();
            Vect2 polyVel(0,0);
            val = geoPolyDetect.Static_Collision_Detector(0,1,fenceVertices,polyVel,O.vect2(),vel,0.1,true);

            if(val){
                return false;
            }
        }
    }

    return true;

}

bool Astar::ComputePath() {
    while (!currentNode->GoalCheck(Goal) ){

        if (currentNode->h < closestDist){
            closestDist = currentNode->h;
            if(!CheckProjectedFenceConflict(currentNode,&Goal)){
                break;
            }
        }

        if(Visited(*currentNode)){
            if(Frontier.size() > 0){
                currentNode = new Node(Frontier.front());
                Frontier.pop_front();
                continue;
            }
            else{
                printf("No more nodes on the frontier\n");
                printf("Total nodes expanded: %lu\n",VisitedList.size());
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


            _node.g = currentNode->g + currentNode->NodeDist(_node) + 1.5*fabs(currentNode->psi - _node.psi)
                                                                + 0.1*fabs(currentNode->vs - _node.vs);

            _node.h = _node.NodeDist(Goal);
            Frontier.push_back(_node);
        }

        Frontier.sort();


        if (Frontier.size() > 0){
            currentNode = new Node(Frontier.front());
            Frontier.pop_front();
            //printf("frontier size: %d\n",Frontier.size());
        }else{
            printf("End of frontier\n");
        }
    }

    //printf("Found path\n");
    Node *_cn = currentNode;
    Path.push_back(this->Goal);
    while(_cn){
        Path.push_back(*_cn);
        _cn = _cn->parent;
    }

    if (Path.size()>=2){
        return true;
    }else{
        return false;
    }
}

std::list<Node> Astar::GetPath() {
    return Path;
}

bool Astar::CheckProjectedFenceConflict(Node* qnode,Node* goal){

    // Ensuring direct line of sight to goal doesn't require a steep turn
    Vect2 AB(goal->x - qnode->x, goal->y - qnode->y);
    Vect2 vel = Vect2::mkTrkGs(qnode->psi * M_PI/180,qnode->speed);

    AB = AB.Scal(1/AB.norm());
    vel = vel.Scal(1/vel.norm());
    double dotprod = AB.dot(vel);

    if (nodeList.size() > 2 && dotprod < 0.7){
        return true;
    }

    std::list<Poly3D>::iterator it;
    for(it = keepOutFence.begin(); it != keepOutFence.end(); ++ it){
        int sizePoly = it->size();
        for(int i=0;i<sizePoly;i++){
            int j = (i + 1) % sizePoly;
            Vect2 A = it->get2D(i); 
            Vect2 B = it->get2D(j);
            Vect3 CurrPos(qnode->x,qnode->y,qnode->z);
            Vect3 NextWP(goal->x,goal->y,goal->z);
            bool intCheck = LinePlanIntersection(A,B,
                                                 it->getBottom(),it->getTop(),
                                                 CurrPos,NextWP);

            if(intCheck){
                return true;
            }
        }
    }

    return false;
}

bool Astar::LinePlanIntersection(Vect2& A,Vect2& B,double floor,double ceiling,Vect3& CurrPos,Vect3& NextWP){

    double x1 = A.x;
    double y1 = A.y;
    double z1 = floor;

    double x2 = B.x;
    double y2 = B.y;
    double z2 = ceiling;

    Vect3 l0 = Vect3(CurrPos.x, CurrPos.y, CurrPos.z);
    Vect3 p0 = Vect3(x1, y1, z1);

    Vect3 n = Vect3(-(z2 - z1) * (y2 - y1), (z2 - z1) * (x2 - x1), 0);
    Vect3 l = Vect3(NextWP.x - CurrPos.x, NextWP.y - CurrPos.y, NextWP.z - CurrPos.z);

    double d = (p0.Sub(l0).dot(n)) / (l.dot(n));

    Vect3 PntI = l0.Add(l.Scal(d));


    Vect3 OA = Vect3(x2 - x1, y2 - y1, 0);
    Vect3 OB = Vect3(0, 0, z2 - z1);
    Vect3 OP = PntI.Sub(p0);
    Vect3 CN = NextWP.Sub(CurrPos);
    Vect3 CP = PntI.Sub(CurrPos);

    double proj1 = OP.dot(OA) / pow(OA.norm(), 2);
    double proj2 = OP.dot(OB) / pow(OB.norm(), 2);
    double proj3 = CP.dot(CN) / pow(CN.norm(), 2);

    if (proj1 >= 0 && proj1 <= 1) {
        if (proj2 >= 0 && proj2 <= 1) {
            if (proj3 >= 0 && proj3 <= 1)
                return true;
        }
    }

    return false;
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
