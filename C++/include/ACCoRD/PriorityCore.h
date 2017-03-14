/* Core Priority Rules Computation 
 * Authors: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PRIORITYCORE_H_
#define PRIORITYCORE_H_

#include "CDSSCore.h"
#include "Vect2.h"
#include "Vect3.h"

namespace larcfm {

/** 
 * 
 * This is a collection of functions to determine if one aircraft has priority 
 * over another aircraft.  It is an encapsulation of the "right of way" rules.<p>
 * 
 * All values are in internal units.<p>
 * 
 * Disclaimer: The current rules have been verified to have certain safety and
 * liveness properties, but they have not been validated, that is, they may not 
 * behave "naturally."  As we learn more about what is expected, these rules
 * may evolve in the future.
 * 
 */
class PriorityCore  {

private:
	PriorityCore() {
		// disallow creation of objects of this type.
	}
	
public:

    /** 
     * The time "back" from the time of closest approach   
     */
	static double TC; 
  
    /**
     * The definition of cruise.  If the absolute value a vehicle's vertical
     * speed is within this value then, by definition, the aircraft is in cruise.<p>
     *
     * This values should be "final"; however, since its precise values
     * is still to be experimentally determined, it is left modifiable.
     */
    static double CRUISE_DEF;
    /**
     * This is a representation of the known uncertainty in the vertical speed.<p>
     *
     * This values should be "final"; however, since its precise values
     * is still to be experimentally determined, it is left modifiable.
     */
    static double VERTICAL_DEADBAND;
    /**
     * This is a representation of the known uncertainty in the horizontal speed. <p>
     *
     * This values should be "final"; however, since its precise values
     * is still to be experimentally determined, it is left modifiable.
     */
    static double LATERAL_DEADBAND;


    static double LATERAL_RELATIVE_DEADBAND;  


private:
	/** 
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return true if the aircraft is descending
	 */
	static bool isDescending(const Vect3& v);
	/**
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return return true if the aircraft is not descending 
	 */
	static bool isNotDescending(const Vect3& v);
	/** 
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return 
	 */
	static bool isCruising(const Vect3& v);
	/** 
	 * 
	 * @param v  a 3D vector representing an aircraft velocity vector
	 * @return trune if the aircraft is ascending 
	 */
	static bool isAscending(const Vect3& v);
	/** 
	 * 
	 * @param v  a 3D vector representing an aircraft velocity vector
	 * @return true if the aircraft is not descending 
 	 */
	static bool isNotAscending(const Vect3& v);
    /**
     * 
     * @param v a 3D vector representing an aircraft velocity vector
     * @return  
     */
	static bool isNotCruising(const Vect3& v);
    /**
     * 
     * @param v a 3D vector representing an aircraft velocity vector
     * @return true if the aircraft is descending or cruising 
     */
	static bool PossiblyDescendingORCruising(const Vect3& v);
    /**
     * 
     * @param v  a 3D vector representing an aircraft velocity vector
     * @return returns true if the aircraft is cruising or ascending 
     */
	static bool PossiblyCruisingORAscending(const Vect3& v);
    /**
     * 
     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
     * @param ai_V  a 3D vector representing traffic  aircraft velocity vector
     * @return
     */
    static bool HorizontalRulesApply(const Vect3& ao_V, const Vect3& ai_V);
    /**
     * 
     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
     * @return
     */
    static bool VerticalRulesApply(const Vect3&  ao_V, const Vect3& ai_V);
    /**
     * 
     * @param so a 3D vector representing the position vector of the ownship
     * @param si a 3D vector representing the position vector of the traffic 
     * @param vo a  3D vector representing the velocity vector of the ownship
     * @param vi a 3D vector representing the velocity vector of the traffic 
     * @return the velocity component
     */
    static double VelComponent(const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi);
   /**
    * 
    * @param time t (should be tca) 
    * @param so a 3D vector representing the position vector of the ownship
    * @param si a 3D vector representing the position vector of the traffic 
    * @param vo a  3D vector representing the velocity vector of the ownship
    * @param vi a 3D vector representing the velocity vector of the traffic 
    * @return velocity  component at time t 
    */
    static double VelComponentAt(const double t, const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi);
   /**
    * 
    * @param so a 3D vector representing the position vector of the ownship
    * @param si a 3D vector representing the position vector of the traffic 
    * @param vo a  3D vector representing the velocity vector of the ownship
    * @param vi a 3D vector representing the velocity vector of the traffic 
    * @return 
    */
    static double EntryComponent(const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi);
   /**
    * 
    * @param ao_S a 3D vector representing the position vector of ownship
    * @param ao_V a 3D vector representing the velocity vector of the ownship
    * @param ai_S a 3D vector representing the position vector of traffic 
    * @param ai_V a 3D vector representing the velocity vector of traffic 
    * @return
    */
    static bool HorizontalDeadband(const Vect3& ao_S, const Vect3& ao_V, const Vect3& ai_S, const Vect3& ai_V);
   /**
    * 
    * @param ao_V 
    * @param ai_V
    * @return
    */
    static bool VerticalDeadband(const Vect3&  ao_V, const Vect3& ai_V);

public:
    /** 
     * Determines if the ownship has priority (in the lateral dimension only) over 
     * the traffic aircraft.
     * 
     * @param so a 3D vector representing the position vector of ownship
     * @param vo a 3D veector representing the velocity vector of the ownship
     * @param si a 3D vector representing the position vector of the traffic aircraft 
     * @param vi a 3D vector representing the velocity vector of the traffic aircraft
     * @return true, if the ownship has priority over the traffic aircraft
     */
	static bool lateral_only(const Vect3&  so, const Vect3& vo, const Vect3& si, const Vect3& vi);
	
	/**
	 * Determines if the ownship has priority over the given traffic aircraft.
	 * 
	 * @param so a 3D vector representing the position vector of ownship
	 * @param vo a 3D vector representing the velocity vector of the ownship
	 * @param mo true if the ownship is managed
	 * @param si a 3D vector representing the position vector of the traffic aircraft 
	 * @param vi a 3D vector representing the velocity vector of the traffic aircraft
	 * @param mi true if the traffic aircraft is managed
	 * @return true, if the ownship has priority over the traffic aircraft
	 */
	static bool check(const Vect3& so, const Vect3& vo, bool mo, const Vect3& si, const Vect3& vi, bool mi);
	
	
};

}

#endif /* PRIORITYCORE_H_ */
