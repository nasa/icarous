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

public class KinematicGsBands extends KinematicRealBands {

  private double horizontal_accel_; // Horizontal acceleration

  public KinematicGsBands() {
    super(DefaultDaidalusParameters.getMinGroundSpeed(),
        DefaultDaidalusParameters.getMaxGroundSpeed(),
        DefaultDaidalusParameters.getGroundSpeedStep(),
        DefaultDaidalusParameters.isEnabledRecoveryGroundSpeedBands());
    horizontal_accel_ = DefaultDaidalusParameters.getHorizontalAcceleration();
  }

  public KinematicGsBands(KinematicGsBands b) {
    super(b);
    horizontal_accel_ = b.horizontal_accel_;
  }

  public void setHorizontalAcceleration(double val) {
    if (val >= 0 && val != horizontal_accel_) {
      horizontal_accel_ = val;
      reset();
    }
  }

  public double getHorizontalAcceleration() {
    return horizontal_accel_;
  }

  public double own_val(TrafficState ownship) {
    return ownship.groundSpeed();
  }

  public double time_step(TrafficState ownship) {
    return get_step()/horizontal_accel_;
  }

  public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {    
    Pair<Position,Velocity> posvel = ProjectedKinematics.gsAccel(ownship.getPosition(),ownship.getVelocity(),time,
        (dir?1:-1)*horizontal_accel_);
    return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
  }

}
