#include <map>
#include "DubinsPlanner.hpp"


void DubinsPlanner::SetParameters(DubinsParams_t &prms){
    params = prms;
}

void DubinsPlanner::ShrinkTrafficVolume(double nfac){
    params.wellClearDistH *= nfac;
    params.wellClearDistV *= nfac;
}

void DubinsPlanner::Reset(){
    trafficPlans.clear();
    trafficPosition.clear();
    trafficVelocity.clear();
    path.clear();
}

void DubinsPlanner::SetVehicleInitialConditions(larcfm::Vect3& pos, larcfm::Velocity& vel){
    rootFix = pos; 
    rootVel = vel;
}

void DubinsPlanner::SetBoundary(larcfm::Poly3D& bBox){
    SetZBoundary(bBox.getBottom(),bBox.getTop());
    auto origVertices = bBox.getVerticesRef();
    shrunkbbox = larcfm::PolycarpResolution::contract_polygon_2D(0.1,0.5,origVertices);
    boundingBox = larcfm::Poly3D(larcfm::Poly2D(shrunkbbox),bBox.getBottom(),bBox.getTop());
}

void DubinsPlanner::SetZBoundary(double zMin,double zMax){
    return;
}

void DubinsPlanner::SetTraffic(std::vector<larcfm::Vect3> &tfPos, std::vector<larcfm::Velocity>& tfVel){
    trafficPosition = tfPos;
    trafficVelocity = tfVel;
    for(int i=0;i<tfPos.size();++i){
         double tend = 1E4;
         larcfm::Velocity vel = tfVel[i];
         larcfm::Position pos0 = larcfm::Position::makeXYZ(tfPos[i].x,"m",tfPos[i].y,"m",tfPos[i].z,"m");
         larcfm::Position pos1 = pos0.linearEst(vel,tend);
         larcfm::Plan plan(std::string("Tfplan" + std::to_string(i)));
         plan.add(pos0,0);
         plan.add(pos1,tend);
         trafficPlans.push_back(plan);
    }
}

void DubinsPlanner::SetTraffic(larcfm::Plan& fp){
    trafficPlans.push_back(fp);
}

void DubinsPlanner::SetObstacles(std::list<larcfm::Poly3D> &obsList){
    obstacleList = obsList;
}

void DubinsPlanner::SetGoal(larcfm::Vect3& goal,larcfm::Velocity vel){
    goalFix = goal;
    goalVel = vel;
}

void DubinsPlanner::GetPotentialFixes(){
    /// Root and Goal nodes are always assumed feasible locations
    node root,goal;
    root.pos = rootFix;
    root.vel = rootVel;
    root.goal = false;
    root.id = 0;
    goal.pos = goalFix;
    goal.vel = goalVel;
    root.dist2goal = MAXDOUBLE;
    goal.dist2goal = 0.0;
    goal.goal = true;
    goal.id = 1;
    potentialFixes.push_back(root);
    potentialFixes.push_back(goal);
    int count =2; 

    /// Use POLYCarp class for detecting obstacle violations
    larcfm::CDPolycarp geoPolycarp;

    /// Get positons close to each obstacle vertex as a potential fix
    double turnRadius = rootVel.gs()/(params.turnRate);
    for(auto &obs: obstacleList){
        /// Expand each obstacle by 2.1 times the turn radius
        double exp = std::max(params.vertexBuffer,2.1*turnRadius);
        auto origVertices = obs.getVerticesRef();
        std::vector<larcfm::Vect2> expVert = larcfm::PolycarpResolution::expand_polygon_2D(0.1,exp,origVertices);
        double floor = obs.getBottom();
        double roof  = obs.getTop();
        double dH = (roof - floor)/params.zSections; 
        /// Add each expanded vertex to potential fixes
        int n = expVert.size();
        for(int k=0;k<n; ++k){
            larcfm::Vect2 vertA = expVert[k%n];
            larcfm::Vect2 vertB = expVert[(k+1)%n];
            double side = vertA.distance(vertB);
            double dt = 1;
            int Npart = 5;
            if (side > Npart){
                 dt = side/Npart; 
            }
            for (int l = 0; l < Npart; ++l)
            {
                larcfm::Vect2 vert = vertA + (vertB - vertA).Hat().Scal(dt * l);
                for (int i = 1; i <= params.zSections; ++i)
                {
                    double zh = floor + dH * i;
                    /// add points dH above floor and root points
                    if (zh < params.maxH)
                    {
                        node fix;
                        fix.pos = larcfm::Vect3(vert, zh);
                        fix.goal = false;
                        fix.id = count;
                        potentialFixes.push_back(fix);
                        count++;
                    }

                    /// add points at goal altitude (after checking to see if free from obstacle conflict)
                    node gfix;
                    double gzh = goal.pos.z;
                    gfix.pos = larcfm::Vect3(vert, gzh);
                    gfix.goal = false;
                    gfix.id = count;
                    if (geoPolycarp.definitelyOutside(gfix.pos, obs) && geoPolycarp.definitelyInside(gfix.pos, boundingBox))
                    {
                        potentialFixes.push_back(gfix);
                        count++;
                    }

                    /// add points at current altitude
                    node sfix;
                    double szh = root.pos.z;
                    sfix.goal = false;
                    sfix.id = count;
                    if (fabs(szh - gzh) > 1)
                    {
                        sfix.pos = larcfm::Vect3(vert, szh);
                        if (geoPolycarp.definitelyOutside(sfix.pos, obs) && geoPolycarp.definitelyInside(gfix.pos, boundingBox))
                        {
                            potentialFixes.push_back(sfix);
                            count++;
                        }
                    }
                }
            }
        }
    }

    /// Shrink bounding box and it's vertices to potential fixes. Shrink by 2.1 x turn radius
    double con = std::max(params.vertexBuffer,2.1*turnRadius);
    auto origVertices = boundingBox.getVerticesRef();
    std::vector<larcfm::Vect2> conVert = larcfm::PolycarpResolution::contract_polygon_2D(0.1,con,origVertices);
    for(auto &vert: conVert){
        node fix;
        fix.pos = larcfm::Vect3(vert, root.pos.z);
        fix.goal = false;
        fix.id = count;
        count++;
        potentialFixes.push_back(fix);
    }

    /// Characteristic length used to radially sample points around root and goal nodes
    double l = root.vel.gs()*10;
    double trk = root.vel.angle();
    
    /// Generate points radially from the root position
    /// N defines the number of paritions of [0,360]
    int N = 5;
    for(int i=0; i<N; ++i){
        for(int j=2;j<=5;++j){
            double x1 = root.pos.x + j*l * cos(trk + i * M_PI * 2/ N);
            double y1 = root.pos.y + j*l * sin(trk + i * M_PI * 2/ N);
            double z1 = root.pos.z;
            double x2 = goal.pos.x + j*l * cos(trk + i * M_PI * 2/ N);
            double y2 = goal.pos.y + j*l * sin(trk + i * M_PI * 2/ N);
            double z2 = goal.pos.z;
            node fix1, fix2;
            fix1.pos = larcfm::Vect3(larcfm::Vect2(x1, y1), z1);
            fix2.pos = larcfm::Vect3(larcfm::Vect2(x2, y2), z2);
            fix1.goal = false;
            fix2.goal = false;

            fix1.id = count;
            potentialFixes.push_back(fix1);
            count++;

            fix2.id = count;
            potentialFixes.push_back(fix2);
            count++;
        }
    }
}

void DubinsPlanner::BuildTree(node* rd){
   /// Build a directed graph by connecting fixes
   /// based on line of sight constraints
   for(auto &fix: potentialFixes){
       if(rd->id == fix.id){
            /// Can ignore or include reflexive transition if needed
            //fix.parents.push_back(rd);
            //rd->children.push_back(&fix);
            continue;
       } 

       /// Add fix as a child only if line of sight is available 
       if(!CheckProjectedFenceConflict(rd,&fix)){
           fix.parents.push_back(rd);
           rd->children.push_back(&fix);
       }
   }

   /// Recursively find suitlable child fixes
   for(auto nd: rd->children){
       if(!nd->goal && nd->children.size() == 0 && (rd->id != nd->id)){
         BuildTree(nd);
       }
   }

}

bool DubinsPlanner::GetNextTrkVs(node& qnode,double &trk,double &vs){
    double totalDist = MAXDOUBLE;
    double r;
    node* target = nullptr;
    for(auto child: qnode.children){
        r = qnode.pos.distanceH(child->pos);
        double dist = r + child->dist2goal;
        if(dist < totalDist){
            totalDist = dist;
            target = child;
        }
    }
    if(target != nullptr){
        larcfm::Vect3 vel = (target->pos - qnode.pos).Hat();
        trk = vel.vect2().trk() * 180/M_PI;
        vs = vel.z*params.maxVS; 
        return true;
    }else{
        trk = qnode.vel.vect2().trk() * 180/M_PI;
        vs  = qnode.vel.vs();
        return true;
    }
    return false;
}

bool DubinsPlanner::GetDubinsParams(node* start,node* end){
    /// Dubins parameter computation based on: Small Unmanned Aircraft: Theory and Practice. Randal W. Beard and Timothy W. McLain
    auto rotateZ = [] (larcfm::Vect2 vec,double angle){
        double x = vec.x*cos(angle) - vec.y*sin(angle);
        double y = vec.x*sin(angle) + vec.y*cos(angle);
        return larcfm::Vect2(x,y);
    };

    larcfm::Vect3 startPos = start->pos;
    larcfm::Vect3 endPos = end->pos;
    larcfm::Velocity startVel = start->vel;
    larcfm::Velocity endVel;
    double t0 = start->time;
    double trk;
    double gs = startVel.gs();
    double vs = startVel.vs();
    //bool valid = GetNextTrkVs(*end,trk,vs);
    if(!end->goal && start->id != end->id){
        if(CheckProjectedFenceConflict(end,&potentialFixes[1])){
            trk = (endPos - startPos).vect2().trk() * 180/M_PI;
        }else{
            trk = (potentialFixes[1].pos - endPos).vect2().trk() * 180/M_PI;
        }
        vs  = 0;
        endVel = larcfm::Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
        end->vel = endVel;
    }else if(start->id == end->id){
        endVel = startVel;
        end->vel = endVel;
    }else{
        endVel = end->vel;
    }

    double R = gs/(params.turnRate);

    double R2 = larcfm::Kinematics::turnRadiusByRate(gs,params.turnRate);
    double currTrk = startVel.trk();
    double bankAngle = larcfm::Kinematics::bankAngleRadius(gs,R);
    double turnRateC = larcfm::Kinematics::turnRate(gs,bankAngle);
    double alt1 = startPos.z; 
    double alt2 = endPos.z;

    larcfm::Vect2 cls = larcfm::Kinematics::centerOfTurn(startPos.vect2(),startVel.vect2(),R,-1);
    larcfm::Vect2 crs = larcfm::Kinematics::centerOfTurn(startPos.vect2(),startVel.vect2(),R,+1);
    larcfm::Vect2 cle = larcfm::Kinematics::centerOfTurn(endPos.vect2(),endVel.vect2(),R,-1);
    larcfm::Vect2 cre = larcfm::Kinematics::centerOfTurn(endPos.vect2(),endVel.vect2(),R,+1);

    std::map<std::string,tcpData_t> tcp;
    std::map<std::string,std::vector<double>> segmentLength;
    double vnu,vnu2,sl;
    larcfm::Vect2 q1,z1,z2;
    larcfm::Vect2 e1(1,0);

    // Lambda function to compute turn times
    auto GetTurnTime = [&] (larcfm::Vect2 posA,larcfm::Vect2 posB,int r){
        double angle = larcfm::Util::turnDelta(posA.trk(),posB.trk(),r);
        double dt = angle/(params.turnRate);
        return dt;
    };

    auto GetSegmentLength = [&](larcfm::Vect2 c1,larcfm::Vect2 c2,larcfm::Vect2 z1,larcfm::Vect2 z2,int d1,int d2){
        double lt1 = R*larcfm::Util::turnDelta((startPos.vect2()-c1).trk(),(z1-c1).trk(),d1);
        double lt2 = R*larcfm::Util::turnDelta((z2-c2).trk(),(endPos.vect2()-c2).trk(),d2);
        return lt1 + lt2 + (z2-z1).norm();
    };

    // Lambda function to gather tcp data
    auto PackTCPdata = [&](std::string pathType,larcfm::Vect2 cs, larcfm::Vect2 ce, larcfm::Vect2 z1, larcfm::Vect2 z2, int r1, int r2, double length) {
        double turnTime1 = GetTurnTime(startPos.vect2() - cs, z1 - cs, r1);
        double turnTime2 = GetTurnTime(z2 - ce, endPos.vect2() - ce, r2);
        double distz1z2 = (z2 - z1).norm();
        double straightTime =  distz1z2/ gs;

        // Check alt feasibility. Pull down z2 to z1 if alt not reachable
        bool feas = CheckAltFeasibility(alt1,alt2,distz1z2,startVel.gs(),endVel.gs());
        double alt2feasible = alt2;
        if(!end->goal && !feas){
            alt2feasible = alt1;
        }

        larcfm::TcpData tcp_1,tcp_2,tcp_3,tcp_4; 
        if(turnTime1 > 1e-3){
            tcp_1 = larcfm::TcpData().setBOT(R*r1, larcfm::Position(larcfm::Vect3(cs, alt1)));
            tcp_2 = larcfm::TcpData().setEOT();
        }

        if(turnTime2 > 1e-3){
            tcp_3 = larcfm::TcpData().setBOT(R*r2, larcfm::Position(larcfm::Vect3(ce, alt2feasible)));
            tcp_4 = larcfm::TcpData().setEOT();
        }

        larcfm::NavPoint pt1 = larcfm::NavPoint(larcfm::Position(startPos), t0);
        larcfm::NavPoint pt2 = larcfm::NavPoint(larcfm::Position(larcfm::Vect3(z1, alt1)),t0 + turnTime1);
        larcfm::NavPoint pt3 = larcfm::NavPoint(larcfm::Position(larcfm::Vect3(z2, alt2feasible)),t0 + turnTime1 + straightTime);
        larcfm::NavPoint pt4 = larcfm::NavPoint(larcfm::Position(endPos.mkZ(alt2feasible)),t0 + turnTime1 + straightTime + turnTime2);

        tcp[pathType].push_back(std::make_pair(pt1, tcp_1));
        if(straightTime > 0){
            tcp[pathType].push_back(std::make_pair(pt2, tcp_2));
            tcp[pathType].push_back(std::make_pair(pt3, tcp_3));
        }else{
            // merge tcp2 and tcp3 if there is no straight segment
            // and and only a single point.
            tcp_2 = tcp_3.mergeTCPData(tcp_2);
            tcp[pathType].push_back(std::make_pair(pt2, tcp_2));
        }
        tcp[pathType].push_back(std::make_pair(pt4, tcp_4));
        // Add altitude to length
        length += fabs(alt2feasible - alt1);
        segmentLength[pathType].push_back(length);
    };

    /// Parameters for RSR curve
    larcfm::Vect2 rsrSE = cre - crs;
    vnu = rsrSE.angle();
    sl = rsrSE.norm();
    if (sl > 1e-5)
    {
        q1 = rsrSE.Hat();
        z1 = crs + rotateZ(q1, M_PI / 2).Scal(R);
        z2 = cre + rotateZ(q1, M_PI / 2).Scal(R);
        double rsrL = GetSegmentLength(crs,cre,z1,z2,1,1); 
        PackTCPdata("RSR", crs, cre, z1, z2, 1, 1, rsrL);
    }else{
        if(fabs(alt2 - alt1) < 1e-3){
            z1 = crs + rotateZ(startPos.vect2()-crs, M_PI);
            PackTCPdata("RRR", crs, cre, z1, z1, 1, 1, 2*M_PI*R);
        }
    }

    /// Parameters for RSL curve
    larcfm::Vect2 rslSE   = cle - crs;
    sl = rslSE.norm();
    if(sl > 2*R){
        vnu = rslSE.angle();          
        vnu2 = acos(2*R/sl);
        q1 = rotateZ(e1,vnu + vnu2 - M_PI/2);
        z1 = crs + rotateZ(e1,vnu+vnu2).Scal(R);
        z2 = cle + rotateZ(e1,vnu+vnu2 - M_PI).Scal(R);
        double rslL = GetSegmentLength(crs,cle,z1,z2,1,-1); 
        PackTCPdata("RSL",crs,cle,z1,z2,1,-1,rslL);
    } 

    /// Parameters for LSR curve
    larcfm::Vect2 lsrSE = cre - cls; 
    sl = lsrSE.norm();
    if(sl > 2*R){
        vnu = lsrSE.angle();
        vnu2 = vnu - M_PI/2 + asin(2*R/sl);
        q1 = rotateZ(e1,vnu2 + M_PI/2);
        z1 = cls + rotateZ(e1,vnu2).Scal(R);
        z2 = cre + rotateZ(e1,vnu2 + M_PI).Scal(R);
        double lsrL = GetSegmentLength(cls,cre,z1,z2,-1,1); 
        PackTCPdata("LSR",cls,cre,z1,z2,-1,1,lsrL); 
    }

    /// Parameters for LSL curve
    larcfm::Vect2 lslSE   = cle - cls; 
    vnu  = lslSE.angle();
    sl   = lslSE.norm();
    if (sl > 1e-5) {
        q1 = lslSE.Hat();
        z1 = cls + rotateZ(q1, -M_PI / 2).Scal(R);
        z2 = cle + rotateZ(q1, -M_PI / 2).Scal(R);
        double lslL = GetSegmentLength(cls,cle,z1,z2,-1,-1); 
        PackTCPdata("LSL", cls, cle, z1, z2, -1, -1, lslL);
    }else{
        if (fabs(alt2 - alt1) < 1e-3){
            z1 = cls + rotateZ(startPos.vect2() - cls, M_PI);
            PackTCPdata("LLL", cls, cle, z1, z1, -1, -1, 2 * M_PI * R);
        }
    }


    typedef std::pair<std::string,std::vector<double>*> mapE;
    std::vector<mapE> pathlens;
    for(auto &item: segmentLength){
        mapE elem(item.first,&item.second);
        pathlens.push_back(elem);
    }

    auto comp = [] (mapE elem1,mapE elem2) { 
        std::vector<double> *val1 = elem1.second; 
        std::vector<double> *val2 = elem2.second; 
        double x = (*val1)[0];
        double y = (*val2)[0];
        return x < y;
    };

    /// Sort the paths based on segment lengths
    std::sort(pathlens.begin(),pathlens.end(),comp);


    for(auto &elem: pathlens){
        std::string pathType = elem.first;
        
        
        /// Compute tcps for altitude changes
        tcpData_t finalTCPdata = ComputeAltTcp(tcp[pathType],startVel.gs(),endVel.gs());

        
        if(finalTCPdata.size() == 4 && (startVel.gs()-endVel.gs()) > 0.1){
             finalTCPdata = ComputeSpeedTcp(tcp[pathType],startVel.gs(),endVel.gs());
        }

        /// Check for fence conflicts
          
        //bool gfConflict = CheckFenceConflict(finalTCPdata);
        //if (gfConflict) continue;
        

        /// Check traffic conflicts on the path
        bool tfConflict = CheckTrafficConflict(finalTCPdata);

        if (tfConflict) continue;

        end->TCPdata = finalTCPdata;
        end->time = finalTCPdata[finalTCPdata.size() - 1].first.time();

        // Distance accumulated
        end->g = (*elem.second)[0] + start->g;   
        // straight line distance to final destination
        end->h = sqrt(pow(end->pos.distanceH(goalFix),2) + pow(end->pos.distanceV(goalFix),2));
        return true;
    }

    return false;
}

bool DubinsPlanner::CheckAltFeasibility(double startZ,double endZ,double dist,double gs1,double gs2){
    
    double v0 = gs1;
    double v1 = gs2;
    double vc = params.climbgs; 
    double ax = params.hAccel;
    double dax = params.hDaccel;
    double hdot = params.maxVS;
    /// Get initial deceleration/acceleration to vc from v0
    double dt_01 = (vc-v0)/dax;
    double dx_01 = v0*dt_01 + 1/2*(dax)*dt_01*dt_01;

    /// Get time to reach v1 from vc with accelration ax: (ax is deceleration if vc > v1)
    double dt_45 = (v1 - vc)/(ax);
    double dx_45 = vc*dt_45 + 1/2*(ax)*pow(dt_45,2);

    double dx = dist - dx_01 - dx_45;

    if(dx < 0){
        return false;
    }

    double dt = dx/vc;
    double dz = hdot*dt; 
    if (dz < fabs(endZ - startZ)){
        return false;
    }
    return true;
}

tcpData_t DubinsPlanner::ComputeSpeedTcp(tcpData_t &TCPdata,double startgs,double stopgs){
    larcfm::Vect3 posA = TCPdata[1].first.position().vect3();
    larcfm::Vect3 posB = TCPdata[2].first.position().vect3();
    larcfm::Vect3 posC = TCPdata[3].first.position().vect3();
    larcfm::Vect2 center = TCPdata[2].second.turnCenter().vect2();

    double radius = std::fabs(TCPdata[2].second.getRadiusSigned());
    double trkAB = (posB - posA).vect2().compassAngle(); 
    double len = posA.distanceH(posB);
    double v0 = startgs;
    double v1 = stopgs;
    double ax = params.hAccel;
    double dax = params.hDaccel;
    double t = 0;
    double a = 0;
    double s = 0;
    if(v1 < v0){
        a = dax;
    }else{
        a = ax;
    }
    /// Get time to reach new speed based on acceleration profile
    t = fabs((v1-v0)/a);
    /// Get distance to complete acceleration to new new speed
    s = fabs(v0*t + 0.5*a*t*t);
    /// Get segment distance before staring acceleration
    double dist = len - s;
    /// Compute intermediate point for BGS (assume EGS is colocated with BOT)
    double tintermediate = TCPdata[1].first.time() + dist/v0;
    double tdiff = (TCPdata[2].first.time() - (tintermediate + t));
    TCPdata[2].first = TCPdata[2].first.makeTime(TCPdata[2].first.time() - tdiff);
    double turnAngle2 = larcfm::Util::turnDelta((posB.vect2()-center).trk(),(posC.vect2()-center).trk(),TCPdata[2].second.turnDir());
    if (fabs(turnAngle2) > 1e-3) {
        double turnRate2 = v1 / radius;
        double turnTime2 = turnAngle2 / turnRate2;
        TCPdata[3].first = TCPdata[3].first.makeTime(TCPdata[2].first.time() + turnTime2);
    }
    if(dist > 0){
        larcfm::Vect3 pos = posA.linearByDist2D(trkAB, dist);
        larcfm::NavPoint interim_pt = larcfm::NavPoint(larcfm::Position(pos), tintermediate);
        larcfm::TcpData bgstcp = larcfm::TcpData().setBGS(a);
        TCPdata[2].second.setEGS();

        tcpData_t newtcp;
        newtcp.push_back(TCPdata[0]);
        newtcp.push_back(TCPdata[1]);
        newtcp.push_back(std::make_pair(interim_pt, bgstcp));
        newtcp.push_back(TCPdata[2]);
        newtcp.push_back(TCPdata[3]);
        return newtcp;
    }else{
        return TCPdata;
    }
}

tcpData_t DubinsPlanner::ComputeAltTcp(tcpData_t &TCPdata,double startgs,double stopgs){
    
    larcfm::Vect3 posA = TCPdata[1].first.position().vect3();
    larcfm::Vect3 posB = TCPdata[2].first.position().vect3();

    double trkAB = (posB - posA).vect2().compassAngle(); 
    double v0 = startgs;
    double v1 = stopgs;
    double vc = params.climbgs; 
    double ax = params.hAccel;
    double az = params.vAccel;
    double dax = params.hDaccel;
    double daz = params.vDaccel;
    double hdot = params.maxVS;
    double hdot0 = 0.0;
    double h0 = posA.z;
    double x0 = 0.0;
    double t0 = TCPdata[1].first.time();
    double dx = posA.distanceH(posB);
    double dh = posB.distanceV(posA); 
    double h4 = h0 + dh, h5 = h4;

    /**
     * The formulation here assumes the vc < v0 and h5 > h0.  
     * If the above assumption is not true, 
     * then hdot,az,daz,ax,dax signs must be chosen appropriately.
     */ 
    if (h0 > h5){
        hdot = params.minVS;
        az = params.vDaccel;
        daz = params.vAccel;
    }

    if (vc > v0){
        dax = params.hAccel;
        ax = params.hDaccel;
    }

    /// Get initial deceleration/acceleration to vc from v0
    double dt_01 = (vc-v0)/dax;
    double dx_01 = v0*dt_01 + 1/2*(dax)*dt_01*dt_01;
    double x1 = x0 + dx_01;
    double h1 = h0;
    double t1 = t0 + dt_01;

    /// Expression for x,h after initial vertical acceleration/deceleration from  0 to hdot
    double dt_12 = fabs(hdot/az);
    double t2 = t1 + dt_12;
    double h2 = h0 + hdot0*(t2-t1) + 0.5*az*pow(t2-t1,2);
    double x2 = x1 + vc*(t2-t1);

    /// Expression for x,h after steady climb 
    double dt_34 = fabs(hdot/daz);

    double h3 = h4 - (hdot*(dt_34) + 0.5*daz*pow(dt_34,2));
    double t3 = t2 + fabs(h3-h2)/fabs(hdot);
    double x3 = x2 + vc*(t3-t2);

    double t4 = t3 + dt_34; 
    double x4 = x3 + vc*(t4-t3);

    /// Time to reach v1 from vc with accelration ax: (ax is deceleration if vc > v1)
    double dt_45 = fabs((v1 - vc)/(ax));

    double dx_45 = vc*dt_45 + 1/2*(ax)*pow(dt_45,2);

    double t5 = t4+dt_45;
    double x5 = x4 + dx_45;
    
    bool valid1 = fabs(x2) <= fabs(x3);
    bool valid2 = fabs(x5) <= fabs(dx);
    bool valid3 = h0 <= h5 ? (fabs(h3) >= fabs(h0)) && (fabs(h3) <= fabs(h4)): 
                             (fabs(h3) <= fabs(h0)) && (fabs(h3) >= fabs(h4));
    bool valid = valid1 && valid2 && valid3;

    tcpData_t newTcp;
    if(valid){
        /// Use the BOT from original TCP
        newTcp.push_back(TCPdata[0]);
        larcfm::Vect3 basePoint = TCPdata[1].first.position().vect3();

        /// Set EOT to also be BGS
        TCPdata[1].second.setBGS(dax);
        newTcp.push_back(TCPdata[1]);

        /// Set EGS & BVS at x1 distance from prev EOT
        larcfm::Vect3 egs = basePoint.linearByDist2D(trkAB,x1).mkZ(h1);
        larcfm::NavPoint navpt1(larcfm::Position(egs),t1);
        larcfm::TcpData egstcp = larcfm::TcpData().setEGS();
        egstcp.setBVS(az);
        newTcp.push_back(std::make_pair(navpt1,egstcp));

        /// Set EVS at x2 distance from prev EOT
        larcfm::Vect3 evs = basePoint.linearByDist2D(trkAB,x2).mkZ(h2);
        larcfm::NavPoint navpt2(larcfm::Position(evs),t2);
        larcfm::TcpData evstcp = larcfm::TcpData().setEVS();
        newTcp.push_back(std::make_pair(navpt2,evstcp));

        /// Set BVS at x3 from prev EOT
        larcfm::Vect3 bvs = basePoint.linearByDist2D(trkAB,x3).mkZ(h3);
        larcfm::NavPoint navpt3(larcfm::Position(bvs),t3);
        larcfm::TcpData bvstcp = larcfm::TcpData().setBVS(daz);
        newTcp.push_back(std::make_pair(navpt3,bvstcp));

        /// Set EVS/BGS at x4 from prev EOT
        larcfm::Vect3 bgs = basePoint.linearByDist2D(trkAB,x4).mkZ(h4);
        larcfm::NavPoint navpt4(larcfm::Position(bgs),t4);
        larcfm::TcpData bgstcp = larcfm::TcpData().setEVS();
        bgstcp.setBGS(ax);
        newTcp.push_back(std::make_pair(navpt4,bgstcp));

        /// Set EGS at x5 from prev EOT
        larcfm::Vect3 egs2 = basePoint.linearByDist2D(trkAB,x5).mkZ(h5);
        larcfm::NavPoint navpt5(larcfm::Position(egs2),t5);
        larcfm::TcpData egs2tcp = larcfm::TcpData().setEGS();
        newTcp.push_back(std::make_pair(navpt5,egs2tcp));

        /// Add remaining TCPs from original data
        /// distance to BOT from x5
        double x56 = egs2.distanceH(TCPdata[2].first.position().vect3());
        double t6 = t5 + x56/v1;
        larcfm::NavPoint navpt6(TCPdata[2].first.position(),t6);
        newTcp.push_back(std::make_pair(navpt6,TCPdata[2].second));

        double t7 = t6 + (TCPdata[3].first.time() - TCPdata[2].first.time());
        larcfm::NavPoint navpt7(TCPdata[3].first.position(),t7);
        newTcp.push_back(std::make_pair(navpt7,TCPdata[3].second));
    }
    if (newTcp.size() > 0) 
        return newTcp;
    else
        return TCPdata;

}




bool DubinsPlanner::ComputePath(double startTime){
   potentialFixes.clear();
   /// Compute potential fix points
   GetPotentialFixes();
   node* root = &potentialFixes[0];
   node* goal = &potentialFixes[1];
   root->time  = startTime;

   /// Build graph
   BuildTree(root);

   /// search path using Astar algorithm
   bool status = AstarSearch(root,goal);

   return status;
}

bool DubinsPlanner::CheckTrafficConflict(tcpData_t trajectory){
    /// Currently only handles trajectories with no overlapping TCP types
    /// Assumes traffic plans are linear
    int trajSize = trajectory.size();
    bool conflict = false;
    for (auto &tfplan: trafficPlans){
        for (int i = 1; i < tfplan.size(); ++i)
        {
            larcfm::Vect3 tfpos = tfplan.getPos(i - 1).vect3();
            larcfm::Velocity tfvel = larcfm::Velocity::makeTrkGsVs(tfplan.trkOut(i - 1) * 180 / M_PI, "degree",
                                                                   tfplan.gsOut(i - 1), "m/s",
                                                                   tfplan.vsOut(i - 1), "m/s");

            double tfStartTime = tfplan.time(i-1);
            double tfStopTime = tfplan.time(i);

            larcfm::Vect2 tfTimeInterval = larcfm::Vect2(tfStartTime,tfStopTime);

            for (int j = 1; j < trajSize; ++j)
            {
                auto trajpt1 = trajectory[j - 1];
                auto trajpt2 = trajectory[j];
                larcfm::Vect3 posA = trajpt1.first.position().vect3();
                larcfm::Vect3 posB = trajpt2.first.position().vect3();
                double timeA = trajpt1.first.time();
                double timeB = trajpt2.first.time();

                if((timeB - timeA) <= 1e-3){
                    continue;
                }

                if(tfStartTime > timeB){
                    conflict = false;
                }

                if (trajpt1.second.isBOT())
                {
                    // This implies trajpt2 is EOT
                    larcfm::Velocity startVel1; // Not currently used.
                    larcfm::Vect2 timeInterval1(timeA, timeB);
                    larcfm::Vect3 center = trajpt1.second.turnCenter().vect3();
                    double R = trajpt1.second.getRadiusSigned();
                    double turnDelta = larcfm::Util::turnDelta((posA-center).vect2().trk(),(posB-center).vect2().trk(),(R>0?+1:-1));
                    double turnRate = fabs(turnDelta)/(timeB-timeA);
                    conflict = CheckConflictLineCircle(center, posA, posB, turnRate, timeInterval1, R, tfpos, tfvel, tfStartTime, tfTimeInterval);
                    if (conflict)
                        return true;
                }else{
                    larcfm::Vect2 timeInterval1(timeA, timeB);
                    double trk = (posB-posA).vect2().trk() * 180/M_PI;
                    double gs  = posA.distanceH(posB)/(timeB-timeA);
                    double vs  = fabs(posA.distanceV(posB))/(timeB - timeA);
                    larcfm::Velocity startVel = larcfm::Velocity::makeTrkGsVs(trk, "degree", gs, "m/s", vs, "m/s");
                    conflict = CheckConflictLineLine(posA, startVel, timeA, timeInterval1, tfpos, tfvel, tfStartTime, tfTimeInterval);
                    if (conflict)
                        return true;
                }
            }
        }
    }
    return conflict;
}

bool DubinsPlanner::CheckTrafficConflict(double startTime,larcfm::Vect3 center1, larcfm::Vect3 startPos, larcfm::Vect3 tcp1,
                                               larcfm::Vect3 center2, larcfm::Vect3 tcp2, larcfm::Vect3 endPos, double r,double dt1,double dt2,double dt3,double gs,double vs){

    bool conflict;
    for(auto &plan: trafficPlans){
        for(int i=1;i<plan.size();++i){
            larcfm::Vect3 tfpos = plan.getPos(i-1).vect3();
            larcfm::Velocity tfvel = larcfm::Velocity::makeTrkGsVs(plan.trkOut(i-1)*180/M_PI,"degree",
                                                                   plan.gsOut(i-1),"m/s",
                                                                   plan.vsOut(i-1),"m/s");

            double tfStartTime = plan.time(i-1);
            double tfStopTime = plan.time(i);
            if(tfStartTime > startTime+dt1+dt2+dt3){
                return false;
            }

            if(tfStartTime < startTime && tfStopTime > startTime){

            }

            if(tfStopTime > startTime + dt1 + dt2 + dt3){
                tfStopTime = startTime + dt1 + dt2 + dt3;
            }


            larcfm::Vect2 tfTimeInterval = larcfm::Vect2(tfStartTime,tfStopTime);

            /// Check for conflict with the first turn segment
            if (dt1 > 1e-3) {
                larcfm::Velocity startVel1; // Not currently used.
                larcfm::Vect2 timeInterval1(startTime, startTime + dt1);
                conflict = CheckConflictLineCircle(center1, startPos, tcp1, 0, timeInterval1, r, tfpos, tfvel, tfStartTime, tfTimeInterval);
                if (conflict)
                    return true;
            }

            /// Check for conflict with level flight segment 
            if (dt2 > 1e-3){
                larcfm::Vect2 timeInterval2(startTime + dt1, startTime + dt1 + dt2);
                larcfm::Velocity startVel2 = larcfm::Velocity::makeTrkGsVs((tcp2 - tcp1).vect2().trk() * 180 / M_PI, "degree", gs, "m/s", vs, "m/s");
                conflict = CheckConflictLineLine(tcp1, startVel2, startTime + dt1, timeInterval2, tfpos, tfvel, tfStartTime, tfTimeInterval);
                if (conflict)
                    return true;
            }

            /// Check for conflict with last turn segment
            if (dt3 > 1e-3){
                larcfm::Velocity startVel3;// Not currently used.
                larcfm::Vect2 timeInterval3(startTime + dt1 + dt2, startTime + dt1 + dt2 + dt3);
                conflict = CheckConflictLineCircle(center2, tcp2, endPos, 0, timeInterval3, r, tfpos, tfvel, tfStartTime, tfTimeInterval);
                if (conflict)
                    return true;
            }
        }
    }
    return false;
}

bool DubinsPlanner::CheckFenceConflict(tcpData_t trajectory){
    int trajSize = trajectory.size();
    bool conflict = false;
    std::list<larcfm::Poly3D> obsList = obstacleList;
    obsList.push_back(boundingBox);
    for (auto &obs : obsList)
    {
        auto vertices = obs.getVerticesRef();
        int totalVertices = vertices.size();
        double floor = obs.getBottom();
        double roof = obs.getTop();
        for (int i = 1; i < trajSize; ++i)
        {
            auto trajpt1 = trajectory[i - 1];
            auto trajpt2 = trajectory[i];
            larcfm::Vect3 posA = trajpt1.first.position().vect3();
            larcfm::Vect3 posB = trajpt2.first.position().vect3();

            /// Check for fence conflict between point A and point B
            if (trajpt1.second.isBOT())
            {
                /// For a BOT, find intersection of complete turn circle
                /// with fence vertices. Note: This is overly conservative
                larcfm::Vect3 center = trajpt1.second.turnCenter().vect3();
                double R = fabs(trajpt1.second.getRadiusSigned());
                if (center.z >= floor && center.z <= roof)
                {
                    for (int j = 0; j < totalVertices; ++j)
                    {
                        larcfm::Vect2 vA = vertices[j] - center.vect2();
                        larcfm::Vect2 vB = vertices[(j + 1) % totalVertices] - center.vect2();
                        auto roots = GetLineCircleIntersection(vA, vB - vA, 0, R+1);
                        if (std::isnan(roots.first))
                        {
                            continue;
                        }
                        else
                        {
                            if ((roots.first >=0 && roots.first <= 1) ||
                                (roots.second >=0 && roots.second <= 1)){
                                    conflict = true;
                                    return conflict;
                                }
                        }
                    }
                }
            }
            else
            {
                /// For all other types of TCPs, find linear intersection
                for (int j = 0; j < totalVertices; ++j)
                {
                    larcfm::Vect2 vA = vertices[j];
                    larcfm::Vect2 vB = vertices[(j + 1) % totalVertices];
                    conflict = LinePlanIntersection(vA, vB, floor, roof, posA, posB);
                    if (conflict)
                    {
                        return conflict;
                    }
                }
            }
        }
    }

    return conflict;
}

bool DubinsPlanner::CheckProjectedFenceConflict(node* qnode,node* goal){
    for(auto &obs: obstacleList){
        int sizePoly = obs.size();
        for(int i=0;i<sizePoly;i++){
            int j = (i+1)%sizePoly;
            larcfm::Vect2 A = obs.get2D(i);
            larcfm::Vect2 B = obs.get2D(j);
            bool intCheck = LinePlanIntersection(A,B, obs.getBottom(),obs.getTop(),
                                                 qnode->pos,goal->pos);

            if(intCheck){
                return true;
            }
        }
    }

    int sizePoly = shrunkbbox.size();
    for(int i=0;i<sizePoly;++i){
            int j = (i+1)%sizePoly;
            larcfm::Vect2 A = shrunkbbox[i];
            larcfm::Vect2 B = shrunkbbox[j];
            bool intCheck = LinePlanIntersection(A,B, boundingBox.getBottom(),boundingBox.getTop(),
                                                 qnode->pos,goal->pos);
            if(intCheck){
                return true;
            }
    }

    return false;
}

bool DubinsPlanner::LinePlanIntersection(larcfm::Vect2& A,larcfm::Vect2& B,double floor,double ceiling,larcfm::Vect3& CurrPos,larcfm::Vect3& NextWP){

    /**
     * Equation of plan is given by: (p-p0).n = 0 ( '.' here is the dot product between vectors)
     *   where p0 is a point on the plane, n is the unit normal vector of the plane.
     * Equation of line: p = l0 + d*l
     *   where l0 is a point on the line, d is a scalar, l is a vector in the line's direction.
     * Subsitute line equation into plan to find intersection. Solve for d:
     *   d = (p0 - l0).n/(l.n)
     * Use d in equation of line to get the point of intersection with the plane.
     */
    double x1 = A.x;
    double y1 = A.y;
    double z1 = floor;

    double x2 = B.x;
    double y2 = B.y;
    double z2 = ceiling;

    
    larcfm::Vect3 l0 = CurrPos;
    larcfm::Vect3 p0 = larcfm::Vect3(A, z1);

    larcfm::Vect3 n = larcfm::Vect3((B-A).PerpL(),0);
    larcfm::Vect3 l = NextWP - CurrPos;
    
    double num = (p0 - l0).dot(n);
    double den = l.dot(n); 
    if(fabs(den) > 1e-3){
        double d =  num / den;

        larcfm::Vect3 PntI = l0.Add(l.Scal(d));

        larcfm::Vect3 OA = larcfm::Vect3(B - A, 0);
        larcfm::Vect3 OB = larcfm::Vect3(0, 0, z2 - z1);
        larcfm::Vect3 OP = PntI - p0;
        larcfm::Vect3 CN = NextWP - CurrPos;
        larcfm::Vect3 CP = PntI - CurrPos;

        // check for projection of PntI on AB
        double proj1 = OP.dot(OA.Hat()) / OA.norm();

        // check for projection of PntI to line within the z limits of the plane
        double proj2 = OP.dot(OB.Hat()) / OB.norm();

        // Check for projection of PntI to line within CurrPos and NextPos 
        double proj3 = CP.dot(CN.Hat()) / CN.norm();

        if (proj1 >= 0 && proj1 <= 1)
        {
            if (proj2 >= 0 && proj2 <= 1)
            {
                if (proj3 >= 0 && proj3 <= 1)
                    return true;
            }
        }
    }
    else
    {
        if(fabs(num) < 1e-3){
            // Line joining current point to next point is parallel to plan 
            larcfm::Vect3 OA = larcfm::Vect3(B - A, 0);
            larcfm::Vect3 CN =  NextWP - larcfm::Vect3(A,0);
            double proj = CN.dot2D(OA.Hat())/OA.norm();
            if(proj>= 0 && proj <= 1){
                return true;
            }
        }else{
            return false;
        }
    }

    return false;
}

void DubinsPlanner::GetPlan(larcfm::EuclideanProjection& proj,larcfm::Plan& newRoute){

    /// The first node in the plan is produced externally
    double ETA;
    node* prevNode = nullptr;
    double initSpeed = rootVel.gs();
    double radius = initSpeed/(params.turnRate);
    int count = 1;
    for (auto node : path) {
        for (int i = 0; i < node.TCPdata.size(); ++i) {
            larcfm::Position wp(proj.inverse(node.TCPdata[i].first.position().vect3()));
            /// Convert TCP center from NED frame to Geodetetic reference frame
            if(node.TCPdata[i].second.isBOT()){
                larcfm::Position center(proj.inverse(node.TCPdata[i].second.turnCenter().vect3()));
                node.TCPdata[i].second.setTurnCenter(center);
            }
            
            double time = node.TCPdata[i].first.time();
            /**
             * Since the first TCP in this node is the same as 
             * the last TCP from the previous node, merge TCP data
             **/
            if(count>1 && i==0){
                larcfm::TcpData tcp = newRoute.getTcpDataRef(count-1);
                larcfm::TcpData tcpNew = tcp.mergeTCPData(node.TCPdata[i].second);
                newRoute.setTcpData(count-1,tcpNew);
            }else{
                newRoute.add(larcfm::NavPoint(wp,time),node.TCPdata[i].second);
                count++;
            }

            /// Add MOT TCP data if making a turn > 180 degree
            if(node.TCPdata[i].second.isEOT()){
                int iBOT = newRoute.prevTrkTCP(count-1);
                int dir = newRoute.turnDir(iBOT);
                double trk1 = newRoute.trkIn(iBOT);
                double gs1 = newRoute.gsIn(iBOT);
                double vs1 = newRoute.vsIn(iBOT);
                double trk2 = newRoute.trkOut(count-1);
                larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(trk1*180/M_PI,"degree",gs1,"m/s",vs1,"m/s"); 
                double trk3 = std::fmod(larcfm::Util::turnDelta(trk1,trk2,dir),2*M_PI);
                double distBOTEOT = node.TCPdata[i-1].first.distanceH(node.TCPdata[i].first);
                if(trk3 > M_PI && distBOTEOT > 0.1){
                    trk3 = trk3/2;
                    larcfm::Vect3 posA = proj.project(newRoute.getPos(iBOT));
                    double turnTime = trk3 / (params.turnRate);
                    double motT = newRoute.time(iBOT) + turnTime;
                    std::pair<larcfm::Vect3, larcfm::Velocity> out = larcfm::Kinematics::turn(posA, vel, turnTime, std::fabs(newRoute.turnRadiusAtTime(motT)), dir>0?true:false);
                    larcfm::Position mot(proj.inverse(out.first));
                    auto MOT = std::make_pair(larcfm::NavPoint(mot, motT), larcfm::TcpData().setMOT(true));
                    newRoute.add(MOT);
                    count++;
                }
            }

            
        }
    }

    //std::cout<<newRoute.toString()<<std::endl;
}


std::pair<double,double> DubinsPlanner::GetLineLineIntersection(larcfm::Vect3 start1,larcfm::Vect3 direction1,double t1,larcfm::Vect3 start2,larcfm::Vect3 direction2,double t2,double R){
    /**
     * Check if two vehicles travelling in a straight line come within D distance apart of each other.
     * Xa = [xa,ya,za] - starting position of vehicle A 
     * Va = [vxa,vya,vza] - starting velocity of vehicle A
     * Xb = [xb,yb,zb] - starting position of vehicle B
     * Vb = [vxb,vyb,vzb] - starting velocity of vehicle B
     * Xa(t) = Xa(t01) + Va x (t - t01)
     * Xb(t) = Xb(t01) + Vb x (t - t02)
     * Xr(t) = Xb(t) - Xa(t)
     * Check \Forall t \in [t0,tf], ||Xr(t)|| > D 
     * Ensured by checking for the roots of quadratic equation
     * ||Xr(t)||^2 - D^2 = 0
     **/
    double xa = start1.x, ya = start1.y, za = start1.z;
    double xb = start2.x, yb = start2.y, zb = start2.z;

    double vxa = direction1.x, vya = direction1.y, vza = direction1.z;
    double vxb = direction2.x, vyb = direction2.y, vzb = direction2.z;

    double D = R;

    double A = vxa*vxa - 2*vxa*vxb + vxb*vxb + vya*vya - 2*vya*vyb + vyb*vyb;
    double B = -2*t1*vxa*vxa + 2*t1*vxa*vxb - 2*t1*vya*vya + 2*t1*vya*vyb + 2*t2*vxa*vxb - 2*t2*vxb*vxb + 2*t2*vya*vyb - 
                2*t2*vyb*vyb + 2*vxa*xa - 2*vxa*xb - 2*vxb*xa + 2*vxb*xb + 2*vya*ya - 2*vya*yb - 2*vyb*ya + 2*vyb*yb;
    double C = -D*D + t1*t1*vxa*vxa + t1*t1*vya*vya - 2*t1*t2*vxa*vxb - 2*t1*t2*vya*vyb - 2*t1*vxa*xa + 2*t1*vxa*xb - 2*t1*vya*ya + 
                2*t1*vya*yb + t2*t2*vxb*vxb + t2*t2*vyb*vyb + 2*t2*vxb*xa - 2*t2*vxb*xb + 2*t2*vyb*ya - 2*t2*vyb*yb + 
                xa*xa - 2*xa*xb + xb*xb + ya*ya - 2*ya*yb + yb*yb;
    
    double disc = B*B - 4*A*C; 
    if(disc < 0){
        return std::make_pair(NaN,NaN);
    }else{
        double tx1 = (-B + sqrt(disc))/(2*A);
        double tx2 = (-B - sqrt(disc))/(2*A);
        return std::make_pair(tx1,tx2);
    }
}

bool DubinsPlanner::CheckConflictLineLine(larcfm::Vect3 start1, larcfm::Velocity startVel1,double startTime1,larcfm::Vect2 timeInterval1,
                                       larcfm::Vect3 start2, larcfm::Velocity startVel2,double startTime2,larcfm::Vect2 timeInterval2){

    

    double D = params.wellClearDistH*1.1;
    std::pair<double,double> roots = GetLineLineIntersection(start1,startVel1,startTime1,start2,startVel2,startTime2,D);

    if(std::isnan(roots.first)){
        // No conflicts if there are only imaginary roots
        return false;
    }else{
        double tx1 = roots.first;
        double tx2 = roots.second;

        // Check if points of conflict lies within our time interval
        if (tx1 >= timeInterval1.x && tx1 <= timeInterval1.y &&
            tx1 >= timeInterval2.x && tx1 <= timeInterval2.y){
            double z1 = start1.z + (tx1 - startTime1)*startVel1.z;
            double z2 = start2.z + (tx1 - startTime2)*startVel2.z;
            if (fabs(z1 - z2) < params.wellClearDistV){
                return true;
            }
        }
        
        if(tx2 >= timeInterval1.x && tx2 <= timeInterval1.y &&
            tx2 >= timeInterval2.x && tx2 <= timeInterval2.y){
            double z1 = start1.z + (tx2 - startTime1)*startVel1.z;
            double z2 = start2.z + (tx2 - startTime2)*startVel2.z;
            if (fabs(z1 - z2) < params.wellClearDistV){
                return true;
            }
        }
    }

    return false;

}

std::pair<double, double> DubinsPlanner::GetLineCircleIntersection(larcfm::Vect2 start, larcfm::Vect2 direction, double t0, double R) {
    /**
     * Find intersection between a line and circle of radius r centered at (0,0)
     * Assume a line is parameterized as follows. 
     * where start \in R2, direction \in R2, t,t0 \in R
     * line = start + (t-t0)*direction.
     **/
    double a, b, c, disc;
    a = (direction.x) * (direction.x) + (direction.y) * (direction.y);
    b = 2 * (start.x * direction.x + start.y * direction.y) - 2 * t0 * direction.x * direction.x - 2 * t0 * direction.y * direction.y;
    c = (start.x) * (start.x) + (start.y) * (start.y) - R * R + 2 * t0 * t0 * direction.x * direction.x 
        + 2 * t0 * t0 * direction.y * direction.y - 2 * t0 * direction.x * start.x - 2 * t0 * direction.y * start.y;

    disc = b * b - 4 * a * c;
    if (disc >= 0)
    {
        double t1 = (-b + sqrt(disc)) / (2 * a);
        double t2 = (-b - sqrt(disc)) / (2 * a);
        return std::make_pair(t1, t2);
    }
    else
    {
        return std::make_pair(NaN, NaN);
    }
}

bool DubinsPlanner::CheckConflictLineCircle(larcfm::Vect3 center1, larcfm::Vect3 start1, larcfm::Vect3 stop1,double turnRate,larcfm::Vect2 timeInterval1, double radius1,
                                          larcfm::Vect3 start2, larcfm::Vect3 startVel2,double t0,larcfm::Vect2 timeInterval2){

         
     double r = params.wellClearDistH + fabs(radius1);
     larcfm::Vect3 c1tf = start2 - center1;
     std::pair<double,double> roots = GetLineCircleIntersection(c1tf.vect2(),startVel2.vect2(),t0,r); 

     if(std::isnan(roots.first)){
         return false;
     }

     auto projTurn = [&] (double t) {
       double startTrk = (start1-center1).vect2().trk();  
       double turnDelta = turnRate*t;
       double newTrk  = startTrk + turnDelta* (radius1>0?+1:-1);
       double R = fabs(radius1);
       larcfm::Vect3 newPoint = center1 + larcfm::Vect3::makeXYZ(R*sin(newTrk),"m",R*cos(newTrk),"m",center1.z,"m");
       return newPoint;
     };


     // Check for conflicts by discretizing the turn into a finite number of points
     
     double dT = (timeInterval1.y  - timeInterval1.x);
     int N = (int)fmax(dT,15);
     double dt = dT/N;
     for(int i=0;i<=N;++i){
         double projectedTime = (timeInterval1.x  + dt*i);
         larcfm::Vect3 posOnCircle = projTurn(projectedTime - timeInterval1.x);
         larcfm::Vect3 projTrafficPos = start2 + startVel2.Scal(projectedTime - t0);
         if ( projTrafficPos.distanceH(posOnCircle) < params.wellClearDistH) return true;
     }
     

     /*
     // Check to see if intersecting times are in conflict
     // NOTE: This check is very conservative
     if (!std::isnan(roots.first))
     {
         double t1 = roots.first;
         double t2 = roots.second; 

         if ((t1 >= timeInterval1.x) && (t1 <= timeInterval1.y) &&
             (t1 >= timeInterval2.x) && (t1 <= timeInterval2.y)) {
             return true;
         }

         if ((t2 >= timeInterval1.x) && (t2 <= timeInterval1.y) &&
             (t2 >= timeInterval2.x) && (t2 <= timeInterval2.y)) {
             return true;
         }

         double minT = std::min(t1, t2);
         double maxT = std::max(t1, t2);

         if (timeInterval1.x >= minT && timeInterval1.x <= maxT) {
             return true;
         }

         if (timeInterval1.y >= minT && timeInterval1.y <= maxT) {
             return true;
         }
      }*/
      return false;
}
