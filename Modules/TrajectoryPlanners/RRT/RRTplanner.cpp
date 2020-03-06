//
// Created by Swee Balachandran on 12/7/17.
//

#include "RRTplanner.h"

RRTplanner::RRTplanner(Poly3D &boundary,
                       int stepT,double dt,double maxD,double maxNorm,
                       void (*PlantDynamics)(double [],double [],double [],int),
                       void (*ControlInput)(node_t&, node_t&,double[]),
                                            const char daaConfig[]){

    dTsteps = stepT;
    dT    = dt;

    double avgRadius = boundary.poly2D().apBoundingRadius();

    xmin = ymin = MAXDOUBLE;
	xmax = ymax = -MAXDOUBLE;
    int n = boundary.poly2D().size();
	for (int i = 0; i < n; ++i) {  // copy
		xmax = Util::max(boundary.poly2D().get(i).x, xmax);
		ymax = Util::max(boundary.poly2D().get(i).y, ymax);
		xmin = Util::min(boundary.poly2D().get(i).x, xmin);
		ymin = Util::min(boundary.poly2D().get(i).y, ymin);
	}

    zmin = boundary.getBottom();
    zmax = boundary.getTop();

    goalreached = false;

    DAA.loadFromFile(daaConfig);
    daaLookAhead = DAA.getLookaheadTime("s");
    time(&startTime);

    maxInputNorm = maxNorm;

    F = PlantDynamics;
    InputFunction = ControlInput;
    nodeCount = 0;

    boundingBox = boundary;
}

void RRTplanner::SetParameters(int stepT, double dt, double maxD, double maxInputNorm) {
    dTsteps = stepT;
    dT    = dt;
    maxInputNorm = maxInputNorm;
}

void RRTplanner::SetDAAParameters(std::string parameterList) {
    larcfm::ParameterData newParams;
    newParams.parseParameterList(";",parameterList);
    DAA.setParameterData(newParams);
}


void RRTplanner::Initialize(Vect3& Pos, Vect3& Vel,std::list<Poly3D> &obstacles, std::vector<Vect3>& TrafficPos, std::vector<Vect3>& TrafficVel,node_t& goal) {


    root.pos = Pos;
    root.vel = Vel;
    root.trafficPos = TrafficPos;
    root.trafficVel = TrafficVel;
    root.id = nodeCount;
    root.parent = NULL;


    nodeList.push_back(root);
    trafficSize = TrafficPos.size();


    closestDist = MAXDOUBLE;
    closestNode = &root;

    obstacleList = obstacles;

    goalNode.goal = true;
    goalNode.pos = goal.pos;
    goalNode.vel = goal.vel;

}


double RRTplanner::NodeDistance(node_t& A, node_t& B){
    return sqrt(pow((A.pos.x - B.pos.x),2) + pow((A.pos.y - B.pos.y),2));
}


node_t* RRTplanner::FindNearest(node_t& query){
    double minDist = MAXDOUBLE;
    double dist;
    node_t* nearest;

    for(ndit = nodeList.begin();ndit != nodeList.end(); ++ndit){
        dist = NodeDistance(*ndit,query);

        if(dist < minDist){
            minDist = dist;
            nearest = &(*ndit);
        }
    }

    return nearest;
}

void RRTplanner::MotionModel(node_t& nearest,node_t& outputNode, double U[]){

    Vect3 pos = nearest.pos;
    Vect3 vel = nearest.vel;
    std::vector<Vect3> trafficPos = nearest.trafficPos;
    std::vector<Vect3> trafficVel = nearest.trafficVel;

    int Xsize = 6+trafficSize*6;

    double *X   = new double[Xsize];
    double *X_p = new double[Xsize];
    double *Y   = new double[Xsize];
    double *k1  = new double[Xsize];
    double *k2  = new double[Xsize];

    memset(Y,0,sizeof(double)*Xsize);
    memset(k1,0,sizeof(double)*Xsize);
    memset(k2,0,sizeof(double)*Xsize);

    X[0] = pos.x;
    X[1] = vel.x;
    X[2] = pos.y;
    X[3] = vel.y;
    X[4] = pos.z;
    X[5] = vel.z;

    std::vector<Vect3>::iterator vecItP;
    std::vector<Vect3>::iterator vecItV;
    int i=0;
    for(vecItP = trafficPos.begin(),vecItV = trafficVel.begin();
        vecItP != trafficPos.end() && vecItV != trafficVel.end();
        ++vecItP,++vecItV){
        X[6+(6*i)+0] =  vecItP->x;
        X[6+(6*i)+1] =  vecItV->x;
        X[6+(6*i)+2] =  vecItP->y;
        X[6+(6*i)+3] =  vecItV->y;
        X[6+(6*i)+4] =  vecItP->z;
        X[6+(6*i)+5] =  vecItV->z;
        i++;
    }

    Vect3 newPos;
    Vect3 newVel;
    std::vector<Vect3> newTrafficPos;
    bool fenceConflict = false;
    bool trafficConflict = false;

    for(int i=0;i<dTsteps;++i){
        F(X,U,Y,trafficSize);

        for(int j=0;j<Xsize;j++){
            k1[j] = Y[j]*dT;
            X_p[j] = X[j] + k1[j];
        }

        F(X_p,U,Y,trafficSize);
        for(int j=0;j<Xsize;j++){
            k2[j] = Y[j]*dT;
        }

        for(int j=0;j<Xsize;j++){
            X[j] = X[j] + 0.5*(k1[j] + k2[j]);
        }

        newPos.x = X[0]; newPos.y = X[2]; newPos.z = X[4];
        newVel.x = X[1]; newVel.y = X[3]; newVel.z = X[5];

        if(CheckFenceCollision(newPos)){
            fenceConflict = true;
        }

        newTrafficPos.clear();
        for(int j=0;j<trafficSize;++j){
            Vect3 newTraffic(X[6+(6*j)+0],X[6+(6*j)+2],X[6+(6*j)+4]);
            newTrafficPos.push_back(newTraffic);
        }

        if(trafficSize > 0 && (i == dTsteps - 1)){
            trafficConflict = CheckTrafficCollision(newPos,newVel,newTrafficPos,trafficVel);
        }

        if(fenceConflict || trafficConflict){
            outputNode.id = -1;
            return;
        }
    }

    nodeCount++;
    outputNode.id  = nodeCount;
    outputNode.pos = newPos;
    outputNode.vel = newVel;
    outputNode.trafficPos = newTrafficPos;
    outputNode.trafficVel = trafficVel;
    outputNode.waitTime = 0.0;

    delete[] X;
    delete[] X_p;
    delete[] Y;
    delete[] k1;
    delete[] k2;
}



void RRTplanner::RRTStep(){

    double X[2];
    // Generate random number
    int rangeX = xmax - xmin;
    int rangeY = ymax - ymin;

    X[0] = xmin + (rand() % rangeX);
    X[1] = ymin + (rand() % rangeY);


    node_t rd;
    rd.pos.x = X[0];
    rd.pos.y = X[1];
    rd.pos.z = root.pos.z;    // should  be set appropriately for 3D plans

    node_t *nearest = FindNearest(rd);
    node_t newNode;

    double U[3];
    InputFunction(*nearest,rd,U);
    U[0] *= maxInputNorm;
    U[1] *= maxInputNorm;
    U[2] *= maxInputNorm;


    if(CheckDirectPath2Goal(nearest)){
        nodeCount++;
        newNode.id = nodeCount;
        newNode.pos = goalNode.pos;
    }else{
        MotionModel(*nearest,newNode,U);

        if(newNode.id < 0){
            return;
        }
    }

    newNode.parent = nearest;
    nearest->children.push_back(newNode);
    nodeList.push_back(newNode);

}

bool RRTplanner::CheckFenceCollision(Vect3& qPos){
    std::list<Poly3D>::iterator it;
    for(it = obstacleList.begin();it != obstacleList.end(); ++it){
        if(geoPolycarp.definitelyInside(qPos,*it)){
            return true;
        }
    }

    if(boundingBox.size() > 2){
        if(!geoPolycarp.definitelyInside(qPos,boundingBox)){
            return true;
        }
    }
    return false;
}

bool RRTplanner::CheckProjectedFenceConflict(node_t* qnode,node_t* goal){
    std::list<Poly3D>::iterator it;
    for(it = obstacleList.begin();it != obstacleList.end(); ++it){
        int sizePoly = it->size();
        for(int i=0;i<sizePoly;i++){
            int j;
            if(i == sizePoly - 1){
                j = 0;
            }
            else{
                j = i+1;
            }

            Vect2 A = it->get2D(i);
            Vect2 B = it->get2D(j);
            bool intCheck = LinePlanIntersection(A,B,
                                                 it->getBottom(),it->getTop(),
                                                 qnode->pos,goal->pos);

            if(intCheck){
                return true;
            }
        }
    }

    return false;
}

bool RRTplanner::CheckTrafficCollision(Vect3& qPos,Vect3& qVel,std::vector<Vect3>& TrafficPos,std::vector<Vect3>& TrafficVel){
    time_t currentTime;
    time(&currentTime);
    double elapsedTime = difftime(currentTime,startTime);

    Position so  = Position::makeXYZ(qPos.x,"m",qPos.y,"m",qPos.z,"m");
    Velocity vo  = Velocity::makeVxyz(qVel.x,qVel.y,"m/s",qVel.z,"m/s");

    DAA.setOwnshipState("Ownship",so,vo,elapsedTime);


    std::vector<Vect3>::iterator itP;
    std::vector<Vect3>::iterator itV;
    int i=0;
    //DAA.stale(true);
    for(itP = TrafficPos.begin(),itV = TrafficVel.begin();
        itP != TrafficPos.end() && itV != TrafficVel.end();
        ++itP,++itV){
        Position si = Position::makeXYZ(itP->x,"m",itP->y,"m",itP->z,"m");
        Velocity vi = Velocity::makeVxyz(itV->x,itV->y,"m/s",itV->z,"m/s");
        char name[10];
        sprintf(name,"Traffic%d",i);i++;
        int ac_idx = DAA.addTrafficState(name,si,vi);
        int alert_level = DAA.alertLevel(ac_idx);
        if(alert_level > 0){
            return true;
        }
    }
    return false;
}

bool RRTplanner::CheckTrafficCollisionWithBands(bool CheckTurn,Vect3& qPos,Vect3& qVel,
                                           std::vector<Vect3>& TrafficPos,std::vector<Vect3>& TrafficVel,Vect3& oldVel){


    time_t currentTime;
    time(&currentTime);
    double elapsedTime = difftime(currentTime,startTime);

    Position so  = Position::makeXYZ(qPos.x,"m",qPos.y,"m",qPos.z,"m");
    Velocity vo  = Velocity::makeVxyz(qVel.x,qVel.y,"m/s",qVel.z,"m/s");

    DAA.setOwnshipState("Ownship",so,vo,elapsedTime);

    std::vector<Vect3>::iterator itP;
    std::vector<Vect3>::iterator itV;
    int i=0;
    double trafficDist = MAXDOUBLE;
    //DAA.stale(true);
    for(itP = TrafficPos.begin(),itV = TrafficVel.begin();
        itP != TrafficPos.end() && itV != TrafficVel.end();
        ++itP,++itV){
        Position si = Position::makeXYZ(itP->x,"m",itP->y,"m",itP->z,"m");
        Velocity vi = Velocity::makeVxyz(itV->x,itV->y,"m/s",itV->z,"m/s");
        char name[10];
        sprintf(name,"Traffic%d",i);i++;
        DAA.addTrafficState(name,si,vi);

        //printf("Traffic pos:%f,%f\n",itP->x,itP->y);
        //printf("Traffic heading:%f\n",vi.track("degree"));

        double distH = so.distanceH(si);

        if(distH < trafficDist){
            trafficDist = distH;
        }
    }

    //TODO: change this number to a parameter
    if(trafficDist < maxD){
        //printf("In cylinder\n");
        return true;
    }

    double qHeading = vo.track("degree");


    //printf("curr heading:%f\n",qHeading);
    //printf("old heading:%f\n",oldHeading);

    if( BandsRegion::isConflictBand(DAA.regionOfHorizontalDirection(DAA.getOwnshipState().horizontalDirection()))){
        return true;
    }
    else if(CheckTurn){
        Velocity vo0 = Velocity::makeVxyz(oldVel.x,oldVel.y,"m/s",oldVel.z,"m/s");
        double oldHeading = vo0.track("degree");
        if(BandsRegion::isConflictBand(DAA.regionOfHorizontalDirection(vo0.trk()))){
            return true;
        }
        // Check collision with traffic based on current heading
        for(int ib=0;ib<DAA.horizontalDirectionBandsLength();++ib){
            if(DAA.horizontalDirectionRegionAt(ib) != larcfm::BandsRegion::NONE ){
                Interval ii = DAA.horizontalDirectionIntervalAt(ib,"deg");

                if(CheckTurnConflict(ii.low,ii.up,qHeading,oldHeading)){
                    //printf("RRT:turn conflict old:%f, new:%f [%f,%f]\n",oldHeading,qHeading,ii.low,ii.up);
                    return true;
                }
            }
        }
    }

    return false;
}

bool RRTplanner::CheckTurnConflict(double low,double high,double newHeading,double oldHeading){

    if(newHeading < 0){
        newHeading = 360 + newHeading;
    }

    if(oldHeading < 0){
        oldHeading = 360 + oldHeading;
    }

    // Get direction of turn
    double psi   = newHeading - oldHeading;
    double psi_c = 360 - std::abs(psi);
    bool rightTurn = false;
    if(psi > 0){
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = false;
        }
        else{
            rightTurn = true;
        }
    }else{
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = true;
        }
        else{
            rightTurn = false;
        }
    }

    double A,B,X,Y,diff;
    if(rightTurn){
        diff = oldHeading;
        A = oldHeading - diff;
        B = newHeading - diff;
        X = low - diff;
        Y = high - diff;

        if(B < 0){
            B = 360 + B;
        }

        if(X < 0){
            X = 360 + X;
        }

        if(Y < 0){
            Y = 360 + Y;
        }

        if(A < X && B > Y){
            return true;
        }
    }else{
        diff = 360 - oldHeading;
        A    = oldHeading + diff;
        B    = newHeading + diff;
        X = low + diff;
        Y = high + diff;

        if(B > 360){
            B = B - 360;
        }

        if(X > 360){
            X = X - 360;
        }

        if(Y > 360){
            Y = Y - 360;
        }

        if(A > Y && B < X){
            return true;
        }
    }

    return false;
}



bool RRTplanner::CheckDirectPath2Goal(node_t* qnode){

    Vect3 A = qnode->pos;
    Vect3 B = goalNode.pos;
    Vect3 AB = B.Sub(A);
    double norm = AB.norm();
    if(norm > 0){
        AB = AB.Scal(maxInputNorm/norm);
    }

    if(CheckProjectedFenceConflict(qnode,&goalNode)){
        return false;
    }

    if(trafficSize > 0){

        Vect3 vel(0,0,0);
        if(qnode->parent != NULL){
            node_t *parent = qnode->parent;
            vel = parent->vel;
        }

        bool CheckTurn = false;

        if(CheckTrafficCollision(qnode->pos,AB,qnode->trafficPos,qnode->trafficVel)){
            return false;
        }
        else{
            return true;
        }
    }
    else{
        return true;
    }
}

bool RRTplanner::CheckWaitAndGo(node_t& qnode){

    Vect3 pos = qnode.pos;
    Vect3 vel = qnode.vel;
    double speed = maxInputNorm;
    Vect3 A = qnode.pos;
    Vect3 B = goalNode.pos;
    Vect3 AB = B.Sub(A);
    AB = AB.mkZ(0);
    double norm = AB.norm();
    if(norm > 0){
        AB = AB.Scal(speed/norm);
    }

    std::vector<Vect3> trafficListPos = qnode.trafficPos;
    std::vector<Vect3> trafficListVel = qnode.trafficVel;
    std::vector<Vect3>::iterator itrP,itrV;
    double maxTime = 0;

    for(itrP=trafficListPos.begin(),itrV=trafficListVel.begin(); itrP != trafficListPos.end(); itrP++, itrV++){
        Vect3 st = *itrP;
        Vect3 vt = *itrV;
        Vect3 V  = vt.Sub(AB);
        Vect3 S  = pos.Sub(st);

        Vect3 xV = AB.cross(vt);

        // cross product of velocities are zero if aircraft heading in parallel directions.
        if(xV.norm() < 0.1){
            // If velocities are parallel
            double cosangle = AB.dot(S)/(AB.norm()*S.norm());
            double angle = acos(cosangle);

            if(angle > 90*M_PI/180){
                angle = M_PI - angle;
            }

            double perpdist = S.norm()*sin(angle);

            if (perpdist < 5){
                return false;
            }
        }

        if (V.norm() < 1e-2){
            return false;
        }
        double C = st.x * V.y - st.y*V.x;
        double E = vt.x*V.y - vt.y*V.x;

        if(E < 0.1){
            return false;
        }

        double D = 5; //TODO: remove this hard coded parameter
        double t0 = (-D* sqrt(V.dot(V)) - C)/E;
        double t1 = (D* sqrt(V.dot(V)) - C)/E;
        double t = 0;

        if (t0 > 0){
            t = t0;
        }
        else if(t1 > 0){
            t = t1;
        }

        if (t > maxTime){
            maxTime = t;
        }
    }

    if(maxTime > 0.0){
        qnode.waitTime = maxTime;
        //printf("Wait time: %f\n",maxTime);
        return true;
    }
    else{
        return false;
    }

}

bool RRTplanner::CheckGoal(){

    node_t *lastNode = &nodeList.back();

    Vect3 diff = lastNode->pos.Sub(goalNode.pos);
    double mag = diff.norm();

    if(mag <= closestDist){
        closestDist = mag;
        closestNode = lastNode;

        /* USE THIS FOR EARLY TERMINATION */
        if(CheckDirectPath2Goal(closestNode)){
            //printf("found direct path to goal\n");
            return true;
        }
    }

    if( mag < 2*maxInputNorm ){
        goalreached = true;
        return true;
    }else{
        goalreached = false;
        return false;
    }
}

void RRTplanner::SetGoal(node_t& goal){
    goalNode = goal;
}


bool RRTplanner::LinePlanIntersection(Vect2& A,Vect2& B,double floor,double ceiling,Vect3& CurrPos,Vect3& NextWP){

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

void RRTplanner::GetPlan(EuclideanProjection& proj,Plan& newRoute){

    double speed = maxInputNorm;
    node_t *node = closestNode;
    node_t parent;
    std::list<node_t> path;
    //printf("Node count:%d\n",nodeCount);
    //printf("Closest dist: %f\n",closestDist);
    //printf("goal: x,y:%f,%f\n",goalNode.pos.x,goalNode.pos.y);

    if(!goalreached){
        node = &goalNode;
        node->parent = closestNode;
    }

    while(node != NULL){
        //printf("x,y:%f,%f\n",node->pos.x,node->pos.y);
        path.push_front(*node);
        node = node->parent;
    }

    std::list<node_t>::iterator nodeIt;
    int count = 0;
    double ETA;
    for(nodeIt = path.begin(); nodeIt != path.end(); ++nodeIt){
        Position wp(proj.inverse(nodeIt->pos));
        if(count == 0){
            ETA = 0;
        }
        else{
            Position prevWP = newRoute.point(count-1).position();
            double distH    = wp.distanceH(prevWP);
            ETA             = ETA + distH/speed;
        }

        NavPoint np(wp,ETA);
        newRoute.addNavPoint(np);
        count++;
    }

    //std::cout<<newRoute.toString()<<std::endl;
}

std::list<node_t>* RRTplanner::GetNodeList() {
    return &nodeList;
}
