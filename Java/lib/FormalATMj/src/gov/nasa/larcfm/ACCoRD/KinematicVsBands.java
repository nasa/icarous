/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.ProjectedKinematics;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class KinematicVsBands extends KinematicRealBands {

  private double vertical_accel_; // Climb/descend acceleration

  public KinematicVsBands() {
    super(DefaultDaidalusParameters.getMinVerticalSpeed(),
        DefaultDaidalusParameters.getMaxVerticalSpeed(),
        DefaultDaidalusParameters.getVerticalSpeedStep(),
        DefaultDaidalusParameters.isEnabledRecoveryVerticalSpeedBands());
    vertical_accel_ = DefaultDaidalusParameters.getVerticalAcceleration();
  }

  public KinematicVsBands(KinematicVsBands b) {
    super(b);
    vertical_accel_ = b.vertical_accel_;
  }

  public void setVerticalAcceleration(double val) {
    if (val >= 0 && val != vertical_accel_) {
      vertical_accel_ = val;
      reset();
    }
  }

  public double getVerticalAcceleration() {
    return vertical_accel_;
  }

  public double own_val(TrafficState ownship) {
    return ownship.verticalSpeed();
  }

  public double time_step(TrafficState ownship) {
    return get_step()/vertical_accel_;
  }

  public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {    
    Pair<Position,Velocity> posvel = ProjectedKinematics.vsAccel(ownship.getPosition(),
        ownship.getVelocity(),time,(dir?1:-1)*vertical_accel_);
    return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
  }

}
