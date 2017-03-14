/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Kinematics;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.ProjectedKinematics;
import gov.nasa.larcfm.Util.Tuple5;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class KinematicAltBands extends KinematicRealBands {

	private double vertical_rate_;  // Climb/descend rate for altitude band
	private double vertical_accel_; // Climb/descend acceleration
	
	public KinematicAltBands(KinematicBandsParameters parameters) {
		super(parameters.getMinAltitude(),
				parameters.getMaxAltitude(),
				parameters.getAltitudeStep(),
				parameters.isEnabledRecoveryAltitudeBands());
		vertical_rate_ = parameters.getVerticalRate();
		vertical_accel_ = parameters.getVerticalAcceleration();
	}

	public KinematicAltBands(KinematicAltBands b) {
		super(b);
		vertical_rate_ = b.vertical_rate_;
		vertical_accel_ = b.vertical_accel_;
	}

	public boolean instantaneous_bands() {
		return vertical_rate_ == 0 || vertical_accel_ == 0;
	}

	public double get_vertical_rate() {
		return vertical_rate_;
	}

	public void set_vertical_rate(double val) {
		if (val != vertical_rate_) {
			vertical_rate_ = val;
			reset();
		}
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
		return ownship.getPositionXYZ().alt();
	}

	public double time_step(TrafficState ownship) {
		return 1;
	}

	public Pair<Vect3, Velocity> trajectory(TrafficState ownship, double time, boolean dir) {
		double target_alt = min_val(ownship)+j_step_*get_step();
		Pair<Position,Velocity> posvel;
		if (instantaneous_bands()) {
			posvel = Pair.make(ownship.getPositionXYZ().mkZ(target_alt),ownship.getVelocityXYZ().mkVs(0));
		} else {
			double tsqj = ProjectedKinematics.vsLevelOutTime(ownship.getPositionXYZ(),ownship.getVelocityXYZ(),vertical_rate_,
					target_alt,vertical_accel_)+time_step(ownship);
			if (time <= tsqj) {
				posvel = ProjectedKinematics.vsLevelOut(ownship.getPositionXYZ(), ownship.getVelocityXYZ(), time, vertical_rate_, target_alt, vertical_accel_);
			} else {
				Position npo = ownship.getPositionXYZ().linear(ownship.getVelocityXYZ(),time);
				posvel = Pair.make(npo.mkZ(target_alt),ownship.getVelocityXYZ().mkVs(0));
			}
		}
		return Pair.make(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
	}

	private boolean conflict_free_traj_step(Detection3D conflict_det, Optional<Detection3D> recovery_det, double B, double T, double B2, double T2,
			TrafficState ownship, List<TrafficState> traffic) {
		boolean trajdir = true;
		if (instantaneous_bands()) {
			return no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,0,ownship,traffic);
		} else {
			double tstep = time_step(ownship);
			double target_alt = min_val(ownship)+j_step_*get_step();
			Tuple5<Double,Double,Double,Double,Double> tsqj = Kinematics.vsLevelOutTimes(ownship.getPositionXYZ().alt(),ownship.getVelocityXYZ().vs(),
					vertical_rate_,target_alt,vertical_accel_,-vertical_accel_,true);
			double tsqj1 = tsqj.first+0;
			double tsqj2 = tsqj.second+0;
			double tsqj3 = tsqj.third+tstep;
			for (int i=0; i<=Math.floor(tsqj1/tstep);++i) {
				double tsi = i*tstep;
				if ((B<=tsi && tsi<=T && any_los_aircraft(conflict_det,trajdir,tsi,ownship,traffic)) ||
						(recovery_det.isPresent() && B2 <= tsi && tsi <= T2 && 
						any_los_aircraft(recovery_det.get(),trajdir,tsi,ownship,traffic))) { 
					return false;
				}
			}
			if ((tsqj2>=B && 
					any_conflict_aircraft(conflict_det,B,Util.min(T,tsqj2),trajdir,Util.max(tsqj1,0),ownship,traffic)) || 
					(recovery_det.isPresent() && tsqj2>=B2 && 
					any_conflict_aircraft(recovery_det.get(),B2,Util.min(T2,tsqj2),trajdir,Util.max(tsqj1,0),ownship,traffic))) {
				return false;
			}
			for (int i=(int)Math.ceil(tsqj2/tstep); i<=Math.floor(tsqj3/tstep);++i) {
				double tsi = i*tstep;
				if ((B<=tsi && tsi<=T && any_los_aircraft(conflict_det,trajdir,tsi,ownship,traffic)) ||
						(recovery_det.isPresent() && B2 <= tsi && tsi <= T2 && 
						any_los_aircraft(recovery_det.get(),trajdir,tsi,ownship,traffic))) { 
					return false;
				}
			}
			return no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,Util.max(tsqj3,0),ownship,traffic);
		}
	}

	private void alt_bands_generic(List<Integerval> l,
			Detection3D conflict_det, Optional<Detection3D> recovery_det, double B, double T, double B2, double T2,
			TrafficState ownship, List<TrafficState> traffic) {
		int max_step = (int)Math.floor((max_val(ownship)-min_val(ownship))/get_step())+1;
		int d = -1; // Set to the first index with no conflict
		for (int k = 0; k <= max_step; ++k) {
			j_step_ = k;
			if (d >=0 && conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
				continue;
			} else if (d >=0) {
				l.add(new Integerval(d,k-1));
				d = -1;
			} else if (conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
				d = k;
			}
		}
		if (d >= 0) {
			l.add(new Integerval(d,max_step));
		}
	}

	public void none_bands(IntervalSet noneset, Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, int epsh, int epsv,
			double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		List<Integerval> altint = new ArrayList<Integerval>();
		alt_bands_generic(altint,conflict_det,recovery_det,B,T,0,B,ownship,traffic);
		toIntervalSet(noneset,altint,get_step(),min_val(ownship),min_val(ownship),max_val(ownship));
	}

	public boolean any_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, int epsh, int epsv,
			double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,true,false) >= 0 ||
				first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,false,false) >= 0;
	}

	public boolean all_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, int epsh, int epsv,
			double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,true,true) < 0 &&
				first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,false,true) < 0;
	}

	int first_nat(int mini, int maxi, boolean dir, Detection3D conflict_det, Optional<Detection3D> recovery_det,
			double B, double T, double B2, double T2, TrafficState ownship, List<TrafficState> traffic, boolean green) {
		while (mini <= maxi) {
			j_step_ = mini;
			if (dir && green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
				return j_step_; 
			} else if (dir) {
				++mini;
			} else {
				j_step_ = maxi;
				if (green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
					return j_step_;
				} else if (maxi == 0) {
					return -1;
				} else {
					--maxi;
				}
			}
		}
		return -1;
	}

	public int first_band_alt_generic(Detection3D conflict_det, Optional<Detection3D> recovery_det,
			double B, double T, double B2, double T2,
			TrafficState ownship, List<TrafficState> traffic, boolean dir, boolean green) {
		int upper = (int)(dir ? Math.floor((max_val(ownship)-min_val(ownship))/get_step())+1 : 
			Math.floor((ownship.getPositionXYZ().alt()-min_val(ownship))/get_step()));
		int lower = dir ? (int)(Math.ceil(ownship.getPositionXYZ().alt()-min_val(ownship))/get_step()) : 0;
		if (ownship.getPositionXYZ().alt() < min_val(ownship) || ownship.getPositionXYZ().alt() > max_val(ownship)) {
			return -1;
		} else {
			return first_nat(lower,upper,dir,conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,green);
		}
	}
	
  // dir=false is down, dir=true is up
  public double resolution(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, int epsh, int epsv,
      double B, double T, TrafficState ownship, List<TrafficState> traffic, boolean dir) {
    int ires = first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,dir,true);
    if (ires < 0) {
      return (dir ? 1 : -1)*Double.POSITIVE_INFINITY;
    } else {
      return min_val(ownship)+ires*get_step();
    }
  }

}
