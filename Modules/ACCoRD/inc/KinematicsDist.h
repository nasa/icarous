/*
 * Kinematics.h
 * 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef KINEMATICSDIST_H_
#define KINEMATICSDIST_H_

#include "Vect3.h"
#include "Vect4.h"
#include "Velocity.h"
#include "Triple.h"


namespace larcfm {

/**
 * <p>A library of functions to aid the computation of the kinematics of an aircraft.  This
 * library is currently under development and is far from complete.  The majority of the functions
 * handle constant velocity turns and movement with constant ground speed acceleration.</p>
 * 
 * <p>Unless otherwise noted, all kinematics function parameters are in internal units -- angles are in radians,
 * linear speeds are in m/s, distances are in meters, time is in seconds.</p>
 * 
 */
class KinematicsDist {
public:




	/** Minimum distance between two aircraft when BOTH turn, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi   the target velocity of traffic (i.e. after turn maneuver complete)
	 * @param bankAngleOwn       the bank angle of the ownship
	 * @param turnRightOwn     the turn direction of ownship
	 * @param bankAngleTraf      the bank angle of the traffic
	 * @param turnRightTraf    the turn direction of traffic
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static Vect4 minDistBetweenTrk(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi, const Velocity& nvi,
			double bankAngleOwn, bool turnRightOwn, double bankAngleTraf, bool turnRightTraf, double stopTime);


	/** Minimum distance between two aircraft when BOTH aircraft gs accelerate, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi           target velocity of traffic (i.e. after acceleration maneuver complete)
	 * @param gsAccelOwn    ground speed acceleration of the ownship
	 * @param gsAccelTraf   ground speed acceleration of the traffic
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static Vect4 minDistBetweenGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,  const Velocity& nvi,
			double gsAccelOwn, double gsAccelTraf, double stopTime);


	/** Minimum distance between two aircraft when only ownship gs accelerates, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param gsAccelOwn    ground speed acceleration of the ownship
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static Vect4 minDistBetweenGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
			double gsAccelOwn, double stopTime);



	/** Minimum distance between two aircraft when BOTH aircraft vs accelerate, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi           target velocity of traffic (i.e. after acceleration maneuver complete)
	 * @param vsAccelOwn    vertical speed acceleration of the ownship
	 * @param vsAccelTraf   vertical speed acceleration of the traffic
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static Vect4 minDistBetweenVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,  const Velocity& nvi,
			double vsAccelOwn, double vsAccelTraf, double stopTime);


	/** Minimum distance between two aircraft when only ownship vs accelerates, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param vsAccelOwn    vertical speed acceleration of the ownship

	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static Vect4 minDistBetweenVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
			double vsAccelOwn, double stopTime);

	static double gsTimeConstantAccelFromDist(double gs1, double a, double dist);

};

}

#endif //KINEMATICSDIST_H_
