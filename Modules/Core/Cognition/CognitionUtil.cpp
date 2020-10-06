#include "Cognition.hpp"

double Cognition::ComputeHeading(const larcfm::Position &positionA,const larcfm::Position &positionB){
    return larcfm::Units::to(larcfm::Units::deg,positionA.track(positionB));
}

double Cognition::ComputeXtrackDistance(const larcfm::Position &prev_wp,
                                        const larcfm::Position &next_wp,
                                        const larcfm::Position &pos,
                                        double offset[]){

    larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(prev_wp);
    larcfm::Vect2 A(0,0);
    larcfm::Vect2 B = proj.project(next_wp).vect2();
    larcfm::Vect2 C = proj.project(pos).vect2();
    larcfm::Vect2 AB = B-A;
    larcfm::Vect2 AC = C-A;

    double distAB   = AB.norm();
    double perpProj = fabs(AC.dot(AB.PerpL().Hat()));
    double straightProj = AC.dot(AB.Hat())/distAB;  
   
    if(offset != NULL){
        offset[0] = perpProj;
        offset[1] = straightProj;
    }

    return perpProj;
}

bool Cognition::CheckProjectedTrafficConflict(){
    bool conflictH,conflictV = true;
    if (!std::isinf(timeToTrafficViolation1) && !std::isnan(timeToTrafficViolation1)) {
        double projAlt = position.alt() + velocity.verticalSpeed("m/s") * timeToTrafficViolation1;
        int wpid = nextWpId[activePlan->getID()];
        double targetAlt = activePlan->getPos(wpid).alt();
        if (fabs(projAlt - targetAlt) > parameters.ZTHR) {
            conflictV = false;
        }

        larcfm::Position projPos = position.linearEst(velocity, timeToTrafficViolation1);
        double offsets[2];
        ComputeXtrackDistance(activePlan->getPos(wpid - 1), activePlan->getPos(wpid), projPos, offsets);
        if (offsets[0] <= parameters.DTHR/2 && (offsets[1] >= 0 && offsets[1] <= 1))
        {
            conflictH = true;
        }else{
            conflictH = false;
        }
    }
    return conflictH && conflictV;
}

void Cognition::ManeuverToIntercept(const larcfm::Position &prev_wp,
                                    const larcfm::Position &next_wp,
                                    const larcfm::Position &curr_position,
                                    double x_trk_dev_gain,
                                    const double resolution_speed,
                                    const double allowed_dev,
                                    larcfm::Velocity &output_velocity){
    double Vs,Vf,V,sgn;
    double Trk;

    double offsets[2];
    ComputeXtrackDistance(prev_wp,next_wp,curr_position,offsets);
    double cross_track_deviation = offsets[0];

    if(x_trk_dev_gain < 0){
        x_trk_dev_gain = -x_trk_dev_gain;
    }

    Vs = x_trk_dev_gain*cross_track_deviation;
    V  = resolution_speed;

    if(Vs >= 0){
        sgn = 1;
    }
    else{
        sgn = -1;
    }

    if(pow(std::abs(Vs),2) >= pow(V,2)){
        Vs = V*sgn;
    }

    Vf = sqrt(pow(V,2) - pow(Vs,2));

    Trk = prev_wp.track(next_wp);
    double Vn = Vf*cos(Trk) - Vs*sin(Trk);
    double Ve = Vf*sin(Trk) + Vs*cos(Trk);
    double Vu = 0;

    double track = atan2(Ve,Vn)*180/M_PI;
    if(track < 0){
        track = 360 + track;
    }

    output_velocity = larcfm::Velocity::makeTrkGsVs(track,"degree", resolution_speed,"m/s", 0,"m/s");
}

larcfm::Position Cognition::GetNearestPositionOnPlan(const larcfm::Position &prev_wp,
                                                     const larcfm::Position &next_wp,
                                                     const larcfm::Position &current_pos){
    double offsets[2];
    ComputeXtrackDistance(prev_wp,next_wp,current_pos,offsets);

    double distAB = prev_wp.distanceH(next_wp); 
    if(distAB < 1e-3){
        return prev_wp;
    }

    double heading2nextWP  = prev_wp.track(next_wp);;
    return prev_wp.linearDist2D(heading2nextWP,offsets[1]*distAB);
}

double Cognition::GetInterceptHeadingToPlan(const larcfm::Position &prev_wp,
                                            const larcfm::Position &next_wp,
                                            const larcfm::Position &current_pos){
    larcfm::Position positionOnPlan;
    positionOnPlan = GetNearestPositionOnPlan(prev_wp,next_wp,current_pos);
    return current_pos.track(positionOnPlan)*180/M_PI;
}

bool Cognition::CheckTurnConflict(double low,double high,double new_heading,double old_heading){

    low  = std::fmod(360+low,360);
    high = std::fmod(360+high,360);
    new_heading = std::fmod(360+new_heading,360);
    old_heading = std::fmod(360+old_heading,360);

    double delta = larcfm::Util::turnDelta(old_heading * M_PI/180,new_heading * M_PI/180);
    int turnDir  = larcfm::Util::turnDir(old_heading * M_PI/180,new_heading * M_PI/180);
    bool rightTurn = (turnDir==1);

    if (low > high){
        high = high + 360;
    }

    bool cond1 = new_heading >= low && new_heading <= high;
    bool cond2 = old_heading >= low && old_heading <= high;

    if(cond1 || cond2){
        return true;
    }

    if(rightTurn){
        if (new_heading < old_heading){
            new_heading += 360;
        }
    }else{
        if (new_heading > old_heading){
            old_heading += 360;
        }
    }
    
    if( low >= old_heading && high <= new_heading ){
        return true;
    }else{
        return false;
    }
}

bool Cognition::ComputeTargetFeasibility(larcfm::Position target){

    double newtrk = position.track(target) * 180 / M_PI;
    double oldtrk = velocity.track("degree"); 

    double elapsedTime = utcTime - trafficConflictStartTime;

    if(elapsedTime < parameters.lookaheadTime/2){
        return false;
    }

    bool conflict = false;
    // Check if it is safe to turn given the current bands available
    // Check feasibility based on trk bands
    for(int i=0;i<trkBandNum;++i){
        double low = trkBandMin[i];
        double high = trkBandMax[i];
        if(trkBandType[i] != BANDREGION_NONE && trkBandType[i] != BANDREGION_RECOVERY){
            conflict |= CheckTurnConflict(low,high,oldtrk,newtrk);
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }

    // Check feasibility based on speed bands
    for(int i=0;i<gsBandNum;++i){
        double low = gsBandMin[i];
        double high = gsBandMax[i];
        if(gsBandType[i] != BANDREGION_NONE && gsBandType[i] != BANDREGION_RECOVERY){
            double refSpeed = GetPlan("Plan0")->gsIn(nextFeasibleWpId);
            if(refSpeed >= low && refSpeed <= high){
                conflict |= true; 
            }
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }

    // Check feasibility based on alt bands
    for(int i=0;i<altBandNum;++i){
        double low = altBandMin[i];
        double high = altBandMax[i];
        if(altBandType[i] != BANDREGION_NONE && altBandType[i] != BANDREGION_RECOVERY){
            double targetAlt = target.alt();
            if(targetAlt >= low && targetAlt <= high){
                conflict |= true; 
            }
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }
    return !conflict; // Negate conflict to denote feasibility
}