/*
 * Copyright (c) 2015-2017 United States Government as represented by
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

	public KinematicVsBands(KinematicBandsParameters parameters) {
		super(parameters.getMinVerticalSpeed(),
				parameters.getMaxVerticalSpeed(),
				parameters.getVerticalSpeedStep(),
				parameters.isEnabledRecoveryVerticalSpeedBands());
		vertical_accel_ = parameters.getVerticalAcceleration();
	}

	public KinematicVsBands(KinematicVsBands b) {
		super(b);
		vertical_accel_ = b.vertical_accel_;
	}

	public boolean instantaneous_bands() {
		return vertical_accel_ == 0;
	}

	public double get_vertical_accel() {
		return vertical_accel_;
	}

	public void set_vertical_accel(double val) {
		if (val != vertical_accel_) {
			vertical_accel_ = val;
			reset();
		}
	}

	public double own_val(TrafficState ownship) {
		return ownship.getVelocityXYZ().vs();
	}

	public double time_step(TrafficState ownship) {
		return get_step()/vertical_accel_;
	}

	public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {    
		Pair<Position,Velocity> posvel;
		if (instantaneous_bands()) {
			double vs = ownship.getVelocityXYZ().vs()+(dir?1:-1)*j_step_*get_step(); 
			posvel = Pair.make(ownship.getPositionXYZ(),ownship.getVelocityXYZ().mkVs(vs));
		} else {
			posvel = ProjectedKinematics.vsAccel(ownship.getPositionXYZ(),
					ownship.getVelocityXYZ(),time,(dir?1:-1)*vertical_accel_);
		}
		return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
	}

}
