#include "Astar.hpp"

int main() {

    double heading[5] = {-90.0, -45.0, 0.0, 45.0, 90.0};
    double vs[1] = {0.0};
    int lenH = 5;
    int lenV = 1;

    Astar pathfinder(lenH,heading,lenV,vs,5.0,5.0);

    pathfinder.SetRoot(0.0,0.0,0.0,0.0,2.0);
    pathfinder.SetGoal(100.0,100.0,0.0);

    Poly2D _bbox;
    _bbox.add(-10.0,-10.0);
    _bbox.add(110.0,-10.0);
    _bbox.add(110.0,110.0);
    _bbox.add(-10.0,110.0);
    Poly3D bbox(_bbox,-10,50);

    Poly2D _obs1;
    _obs1.add(20,20);
    _obs1.add(60,20);
    _obs1.add(60,60);
    _obs1.add(20,60);
    Poly3D obs1(_obs1,0,30);

    Poly2D _obs2;
    _obs2.add(20,40);
    _obs2.add(40,40);
    _obs2.add(40,80);
    _obs2.add(20,80);
    Poly3D obs2(_obs2,0,30);

    pathfinder.SetBoundary(&bbox);
    pathfinder.InputObstacle(&obs1);
    pathfinder.InputObstacle(&obs2);

    pathfinder.ComputePath();

    std::list<Node> path = pathfinder.GetPath();
    std::list<Node>::iterator it;
    for(it = path.begin();it!=path.end();++it){
        printf("x = %f, y = %f\n",it->x,it->y);
    }

    return 0;
}