//
// Created by Swee Balachandran on 3/23/18.
//
#include "PathPlanner.h"
#include "Astar.h"

void PathPlanner::InitializeAstarParameters(bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[]){
    _astar_daaConfig = string(daaConfig);
    _astar_enable3D = enable3D;
    _astar_gridSize = gridSize;
    _astar_lookahead = lookahead;
    _astar_resSpeed = resSpeed;
}

void PathPlanner::UpdateAstarParameters(bool enable3D, double gridSize, double resSpeed, double lookahead,char daaConfig[]) {
    _astar_daaConfig = string(daaConfig);
    _astar_enable3D = enable3D;
    _astar_gridSize = gridSize;
    _astar_lookahead = lookahead;
    _astar_resSpeed = resSpeed;
}

int64_t PathPlanner::FindPathAstar(char planID[],double fromPosition[],double toPosition[],double velocity[]) {

    double trk = velocity[0];
    double gs = velocity[1];
    double vs = velocity[2];

    // Reroute flight plan
    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;

    Position currentPos = Position::makeLatLonAlt(fromPosition[0], "degree",fromPosition[1], "degree", fromPosition[2], "m");
    Position endPos     = Position::makeLatLonAlt(toPosition[0], "degree", toPosition[1], "degree", toPosition[2], "m");
    Velocity currentVel = Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");


    EuclideanProjection proj = Projection::createProjection(currentPos.mkAlt(0));

    double computationTime = 1.0;

    for(GenericObject tf: trafficList){
        double x,y,z,vx,vy,vz;
        Velocity Vel = tf.vel;
        Position Pos = tf.pos;
        Vect3 tPos = proj.project(Pos);
        Vect3 tVel = Vect3(Vel.x,Vel.y,Vel.z);
        tPos.linear(tVel,computationTime);
        TrafficPos.push_back(tPos);
        TrafficVel.push_back(tVel);
    }

    Plan currentFP;
    Position prevWP;
    Position nextWP;
    double dist = currentVel.gs()*computationTime;

    Position start = currentPos.linearDist2D(currentVel.trk(), dist);
    Position goalPos = endPos;

    Vect3 initPosR3 = proj.project(currentPos);
    Vect3 gpos = proj.project(goalPos);


    //TODO: Make these user defined parameters?
    bool search3d = false;
    search3d = _astar_enable3D;

    double HEADING[9] = {-90.0, -60.0, -45.0, -30.0, 0.0, 30.0, 45.0, 60.0, 90.0};
    double VS[3] = {0.0,0.0,0.0};
    int lenH = 9;
    int lenV = 1;

    if(search3d){
        VS[0] = -1.0;
        VS[2] = 1.0;
        lenV  = 3;
    }
    
    double res_speed = _astar_resSpeed;
    double horizon = _astar_lookahead;
    double eps = res_speed*horizon*0.5;
    Astar pathfinder(lenH,HEADING,lenV,VS,horizon,eps);

    pathfinder.SetRoot(initPosR3.x,initPosR3.y,initPosR3.z,trk,res_speed);
    pathfinder.SetGoal(gpos.x,gpos.y,gpos.z);

    for(fence gf: fenceList){
        if (gf.GetType() == KEEP_IN){
            Poly3D bbox = gf.GetPoly()->poly3D(proj);
            pathfinder.SetBoundary(&bbox);
        }else{
            Poly3D obs = gf.GetPolyMod()->poly3D(proj);
            pathfinder.InputObstacle(&obs);
        }
    }

    bool goalFound = pathfinder.ComputePath();

    if(goalFound){
        std::list<Node> _planOutput = pathfinder.GetPath();
        _planOutput.reverse();
        std::list<Node>::iterator it;
        Plan output;
        int count = 0;
        double ETA = 0.0;
        double speed = 1.0; //TODO: maybe change this speed to something based on mission?
        for(it = _planOutput.begin(); it != _planOutput.end(); ++it){
            Vect3 _locxyz(it->x,it->y,it->z);
            Position wp(proj.inverse(_locxyz));
            if(count == 0){
                ETA = 0;
            }
            else{
                Position prevWP = output.point(count-1).position();
                double distH    = wp.distanceH(prevWP);
                ETA             = ETA + distH/speed;
            }
            NavPoint np(wp,ETA);
            output.addNavPoint(np);
            count++;
        }
        output.setName(string(planID));
        flightPlans.push_back(output);
        return output.size();
    }else{
        return -1;
    }
}