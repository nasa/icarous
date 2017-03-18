/*
 * Copyright (c) 2015-2017 United States Government as represented by
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
	public KinematicTrkBands(KinematicBandsParameters parameters) {
		super(-parameters.getLeftTrack(),
				parameters.getRightTrack(),
				true,2*Math.PI,
				parameters.getTrackStep(), 
				parameters.isEnabledRecoveryTrackBands());
		turn_rate_ = parameters.getTurnRate();
		bank_angle_ = parameters.getBankAngle();
	}

	public KinematicTrkBands(KinematicTrkBands b) {
		super(b);
		turn_rate_ = b.turn_rate_;
		bank_angle_ = b.bank_angle_;
	}

	public boolean instantaneous_bands() {
		return turn_rate_ == 0 && bank_angle_ == 0;
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
		return ownship.getVelocityXYZ().compassAngle();
	}

	public double time_step(TrafficState ownship) {
		double gso = ownship.getVelocityXYZ().gs();
		double omega = turn_rate_ == 0 ? Kinematics.turnRate(gso,bank_angle_) : turn_rate_;
		return get_step()/omega;
	}

	public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {  
		Pair<Position,Velocity> posvel;
		if (instantaneous_bands()) {
			double trk = ownship.getVelocityXYZ().compassAngle()+(dir?1:-1)*j_step_*get_step(); 
			posvel = Pair.make(ownship.getPositionXYZ(),ownship.getVelocityXYZ().mkTrk(trk));
		} else {
			double gso = ownship.getVelocityXYZ().gs();
			double bank = turn_rate_ == 0 ? bank_angle_ : Math.abs(Kinematics.bankAngle(gso,turn_rate_));
			double R = Kinematics.turnRadius(gso,bank);
			posvel = ProjectedKinematics.turn(ownship.getPositionXYZ(),ownship.getVelocityXYZ(),time,R,dir);
		}
		return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
	}

}
