#include "Cognition.hpp"

double Cognition::ComputeHeading(const larcfm::Position &positionA,const larcfm::Position &positionB){
    return larcfm::Units::to(larcfm::Units::deg,positionA.track(positionB));
}

double Cognition::ComputeXtrackDistance(const larcfm::Position &prev_wp,
                                        const larcfm::Position &next_wp,
                                        const larcfm::Position &pos,
                                        double offset[]){
    double psi1         = prev_wp.track(next_wp) * 180/M_PI;
    double psi2         = prev_wp.track(pos) * 180/M_PI;
    double sgn          = 0;

    if( (psi1 - psi2) >= 0){
        sgn = 1;              // Vehicle left of the path
    }
    else if( (psi1 - psi2) <= 180){
        sgn = -1;             // Vehicle right of the path
    }
    else if( (psi1 - psi2) < 0 ){
        sgn = -1;             // Vehicle right of path
    }
    else if ( (psi1 - psi2) >= -180  ){
        sgn = 1;              // Vehicle left of path
    }

    double bearing = std::abs(psi1 - psi2);
    double dist = prev_wp.distanceH(pos);
    double cross_track_deviation = sgn*dist*sin(bearing * M_PI/180);
    double cross_track_offset    = dist*cos(bearing * M_PI/180);

    if(offset != NULL){
        offset[0] = cross_track_deviation;
        offset[1] = cross_track_offset;
    }

    return cross_track_deviation;
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

void Cognition::GetPositionOnPlan(const larcfm::Position &prev_wp,
                                  const larcfm::Position &next_wp,
                                  const larcfm::Position &current_pos,
                                  larcfm::Position &position_on_plan){
    double offsets[2];
    ComputeXtrackDistance(prev_wp,next_wp,current_pos,offsets);

    double heading_next_wp  = prev_wp.track(next_wp);;
    double dn               = offsets[1]*cos(heading_next_wp);
    double de               = offsets[1]*sin(heading_next_wp);

    position_on_plan = prev_wp.linearEst(dn, de);

    if(position_on_plan.alt() <= 0){
        position_on_plan = position_on_plan.mkAlt(next_wp.alt());
    }
}

double Cognition::GetInterceptHeadingToPlan(const larcfm::Position &prev_wp,
                                            const larcfm::Position &next_wp,
                                            const larcfm::Position &current_pos){
    larcfm::Position position_on_plan;
    GetPositionOnPlan(prev_wp,next_wp,current_pos,position_on_plan);
    return current_pos.track(position_on_plan)*180/M_PI;
}

bool Cognition::CheckTurnConflict(const double low,const double high,double new_heading,double old_heading){
    if(new_heading < 0){
        new_heading = 360 + old_heading;
    }

    if(old_heading < 0){
        old_heading = 360 + old_heading;
    }

    // Get direction of turn
    double psi   = new_heading - old_heading;
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
        diff = old_heading;
        A = old_heading - diff;
        B = new_heading - diff;
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
        diff = 360 - old_heading;
        A    = old_heading + diff;
        B    = new_heading + diff;
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
