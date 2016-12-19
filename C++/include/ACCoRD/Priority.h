/* Priority Rules Computation  
 * Authors: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PRIORITY_H_
#define PRIORITY_H_

//#include "UnitSymbols.h"
#include "PriorityCore.h"
#include "Position.h"
#include "Velocity.h"

namespace larcfm {

/** 
 * 
 * This class is used to determine if one aircraft has priority over another aircraft.  It 
 * is an encapsulation of the "right of way" rules.<p>
 * 
 * Disclaimer: The current rules have been verified to have certain safety and
 * liveness properties, but they have not been validated, that is, they may not 
 * behave "naturally."  As we learn more about what is expected, these rules
 * may evolve in the future.
 * 
 */
class Priority  {

private:
	Priority() {
		// Do not allow these to be created.
	}

public:
	/** I don't know what this parameter is for */
	static double TIME_MANEUVER_RESTRICTION;
	/** I don't know what this parameter is for */
	static double TIME_PROXIMITY;
	/** I don't know what this parameter is for */
	static double TIME_LOOKAHEAD;


	/** Returns TC in seconds */
	static double get_tc(){
		return Units::to("s", PriorityCore::TC);
	}
	
	/** Returns the vertical speed value that defines "cruise" in [fpm] */
	static double get_cruise_def(){
		return Units::to("fpm", PriorityCore::CRUISE_DEF);
	}
	
	/** Returns the value of the relative uncertainty in vertical speed in [fpm] */
	static double get_vertical_deadband(){
		return Units::to("fpm", PriorityCore::VERTICAL_DEADBAND);
	}
	
	/** Returns the value of the relative uncertainty in horizontal speed in [knot] */
	static double get_lateral_deadband(){
		return Units::to("kn", PriorityCore::LATERAL_DEADBAND);
	}
	
	/** Returns the value of the relative uncertainty in horizontal speed in [knot] */
	static double get_lateral_relative_deadband(){
		return Units::to("kn", PriorityCore::LATERAL_RELATIVE_DEADBAND);
	}
	

	/** returns the value of the time maneuver restrictions. I don't know what this parameter is for */
	static double get_time_maneuver_restriction(){
		return TIME_MANEUVER_RESTRICTION;
	}
	
	/** returns the value of the time proximity. I don't know what this parameter is for  */ 
	static double get_time_proximity(){
		return TIME_PROXIMITY;
	}
	/** returns the value of the time lookahead. I don't know what this parameter is for  */ 
	static double get_time_lookahead(){
		return TIME_LOOKAHEAD;
	}
	

	/** Set the value of the relative uncertainty in the horizontal speed in [knot] <p>
	 *  
	 * This value should never change; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable. 
	 */
        static void set_tc(double tc){
	  PriorityCore::TC = Units::from("s", tc);
	}

	/** Set the value of the vertical speed that defines "cruise" in [fpm] <p>
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	static void set_cruise_def(double CruiseDef){
		PriorityCore::CRUISE_DEF = Units::from("fpm", CruiseDef);
	}
	
	/** Set the value of the relative uncertainty in vertical speed in [fpm] <p>
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	static void set_vertical_deadband(double VerticalDeadband){
		PriorityCore::VERTICAL_DEADBAND = Units::from("fpm", VerticalDeadband);
	}
	
	/** Set the value of the relative uncertainty in the horizontal speed in [knot] <p>
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	static void set_lateral_deadband(double LateralDeadband){
		PriorityCore::LATERAL_DEADBAND = Units::from("kn", LateralDeadband);
	}


	
	/** Set the value of the relative uncertainty in the horizontal speed in [knot]  
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	static void set_lateral_relative_deadband(double LateralRelativeDeadband){
		PriorityCore::LATERAL_RELATIVE_DEADBAND = Units::from("kn", LateralRelativeDeadband);
	}
	
	/** Set the value of the time to maneuver. I don't know what this parameter is for  */
	static void set_time_maneuver_restriction(double TimeManeuverRestriction){
	    TIME_MANEUVER_RESTRICTION = TimeManeuverRestriction;
	}
	
	/** Set the value of the time proximity. I don't know what this parameter is for  */
	static void set_time_proximity(double TimeProximity){
		TIME_PROXIMITY = TimeProximity;
	}
	
	/** Set the value of the time lookahead. I don't know what this parameter is for  */
	static void set_time_lookahead(double TimeLookahead){
		 TIME_LOOKAHEAD = TimeLookahead;
	}

    /** 
     * Determines if the ownship has priority (in the lateral dimension only) over 
     * the traffic aircraft.
     * 
     * @param sox x component of the position of the ownship in [nmi]
     * @param soy y component of the position of the ownship in [nmi]
     * @param soz z component of the position of the ownship in [ft]
     * @param vox x component of the velocity of the ownship in [knot]
     * @param voy y component of the velocity of the ownship in [knot]
     * @param voz z component of the velocity of the ownship in [fpm]
     * @param six x component of the position of the traffic aircraft in [nmi]
     * @param siy y component of the position of the traffic aircraft in [nmi]
     * @param siz z component of the position of the traffic aircraft in [ft]
     * @param vix x component of the velocity of the traffic aircraft in [knot]
     * @param viy y component of the velocity of the traffic aircraft in [knot]
     * @param viz z component of the velocity of the traffic aircraft in [fpm]
     * @return true, if the ownship has priority over the traffic aircraft
     */
	static bool lateral_only (
			double sox, double soy, double soz, 
			double vox, double voy, double voz, 
			double six, double siy, double siz,
			double vix, double viy, double viz );
	
	/**
	 * Determines if the ownship has priority over the given traffic aircraft.
	 * 
     * @param sox x component of the position of the ownship in [nmi]
     * @param soy y component of the position of the ownship in [nmi]
     * @param soz z component of the position of the ownship in [ft]
     * @param vox x component of the velocity of the ownship in [knot]
     * @param voy y component of the velocity of the ownship in [knot]
     * @param voz z component of the velocity of the ownship in [fpm]
	 * @param mo true if the ownship is managed
     * @param six x component of the position of the traffic aircraft in [nmi]
     * @param siy y component of the position of the traffic aircraft in [nmi]
     * @param siz z component of the position of the traffic aircraft in [ft]
     * @param vix x component of the velocity of the traffic aircraft in [knot]
     * @param viy y component of the velocity of the traffic aircraft in [knot]
     * @param viz z component of the velocity of the traffic aircraft in [fpm]
	 * @param mi true if the traffic aircraft is managed
	 * @return true, if the ownship has priority over the traffic aircraft
	 */
	static bool check (
		double sox, double soy, double soz, 
		double vox, double voy, double voz,
		bool mo,
		double six, double siy, double siz,
		double vix, double viy, double viz,
		bool mi);

    /**
     * Determines if the ownship has priority over the given traffic aircraft.
     *
     * @param lato latitude of the ownship in [degrees north]
     * @param lono longitude of the ownship in [degrees east]
     * @param alto altitude of the ownship in [ft]
     * @param trko track angle of the velocity of the ownship in [deg, clockwise from true north]
     * @param gso ground speed of the ownship in [knot]
     * @param vso vertical speed of the ownship in [fpm]
     * @param mo true if the ownship is managed
     * @param lati latitude of the traffic aircraft in [degrees north]
     * @param loni longitude of the traffic aircraft in [degrees east]
     * @param alti altitude of the traffic aircraft in [ft]
     * @param trki track angle of the velocity of the traffic aircraft in [deg, clockwise from true north]
     * @param gsi ground speed of the traffic aircraft in [knot]
     * @param vsi vertical speed of the traffic aircraft in [fpm]
     * @param mi true if the traffic aircraft is managed
     * @return true, if the ownship has priority over the traffic aircraft
     */
    static bool checkLL(
            double lato, double lono, double alto,
            double trko, double gso, double vso,
            bool mo,
            double lati, double loni, double alti,
            double trki, double gsi, double vsi,
            bool mi);

    /**
     * Determines if the ownship has priority over the given traffic aircraft.
     *
     * @param po position of the ownship
     * @param vo velocity of the ownship
     * @param mo true if the ownship is managed
     * @param pi position of the traffic aircraft
     * @param vi velocity of the traffic aircraft
     * @param mi true if the traffic aircraft is managed
     * @return true, if the ownship has priority over the traffic aircraft
     */
    static bool check(
            const Position& po, const Velocity& vo, bool mo,
            const Position& pi, const Velocity& vi, bool mi);
};

}

#endif /* PRIORITY_H_ */
