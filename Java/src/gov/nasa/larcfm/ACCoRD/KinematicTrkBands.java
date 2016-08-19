/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Kinematics;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.ProjectedKinematics;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class KinematicTrkBands extends KinematicRealBands {

  private double turn_rate_;     
  private double bank_angle_; // Only used when turn_rate is set to 0  

  // min/max is left/right relative to ownship's track
  public KinematicTrkBands() {
    super(DaidalusParameters.DefaultValues.getLeftTrack(),
        DaidalusParameters.DefaultValues.getRightTrack(),
        true,2*Math.PI,
        DaidalusParameters.DefaultValues.getTrackStep(), 
        DaidalusParameters.DefaultValues.isEnabledRecoveryTrackBands());
    turn_rate_ = DaidalusParameters.DefaultValues.getTurnRate();
    bank_angle_ = DaidalusParameters.DefaultValues.getBankAngle();
  }

  public KinematicTrkBands(KinematicTrkBands b) {
    super(b);
    turn_rate_ = b.turn_rate_;
    bank_angle_ = b.bank_angle_;
  }

  public double get_turn_rate() {
    return turn_rate_;
  }

  public void set_turn_rate(double val) {
    if (val != turn_rate_) {
      turn_rate_ = val;
      reset();
    }
  }

  public double get_bank_angle() {
    return bank_angle_;
  }

  public void set_bank_angle(double val) {
    if (val != bank_angle_) {
      bank_angle_ = val;
      reset();
    }
  }

  public double own_val(TrafficState ownship) {
    return ownship.track();
  }

  public double time_step(TrafficState ownship) {
    double gso = ownship.groundSpeed();
    double omega = turn_rate_ == 0 ? Kinematics.turnRate(gso,bank_angle_) : turn_rate_;
    return get_step()/omega;
  }

  public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {  
    double gso = ownship.groundSpeed();
    double bank = turn_rate_ == 0 ? bank_angle_ : Math.abs(Kinematics.bankAngle(gso,turn_rate_));
    double R = Kinematics.turnRadius(gso,bank);
    Pair<Position,Velocity> posvel = ProjectedKinematics.turn(ownship.getPosition(),ownship.getVelocity(),time,R,dir);
    return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
  }

}
