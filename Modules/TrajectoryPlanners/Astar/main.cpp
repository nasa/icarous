#include "Astar.h"



int main() {

    double heading[5] = {-90.0, -45.0, 0.0, 45.0, 90.0};
    double vs[1] = {0.0};
    int lenH = 5;
    int lenV = 1;

    Astar pathfinder(lenH,heading,lenV,vs,5.0,5.0);

    pathfinder.SetRoot(0.0,0.0,0.0,0.0);
    pathfinder.SetGoal(50.0,50.0,0.0);
    pathfinder.GetPath();


    return 0;
}