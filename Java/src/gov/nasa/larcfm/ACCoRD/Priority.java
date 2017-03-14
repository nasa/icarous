/* Priority Rules Computation  
 * Contact: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import static gov.nasa.larcfm.ACCoRD.PriorityCore.CRUISE_DEF;
import static gov.nasa.larcfm.ACCoRD.PriorityCore.VERTICAL_DEADBAND;
import static gov.nasa.larcfm.ACCoRD.PriorityCore.LATERAL_DEADBAND;
import static gov.nasa.larcfm.ACCoRD.PriorityCore.LATERAL_RELATIVE_DEADBAND;
import static gov.nasa.larcfm.ACCoRD.PriorityCore.TC;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Velocity;

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
public final class Priority  {

	private Priority() {
		// Do not allow these to be created.
	}

	/** I don't know what this parameter is for */
	public static double TIME_MANEUVER_RESTRICTION;
	/** I don't know what this parameter is for */
	public static double TIME_PROXIMITY;
	/** I don't know what this parameter is for */
	public static double TIME_LOOKAHEAD;

	static {
	    TC = Units.from("s",30.0);
		CRUISE_DEF = Units.from("fpm", 150.0); 
		VERTICAL_DEADBAND = Units.from("fpm", 20.0);
		LATERAL_RELATIVE_DEADBAND = Units.from("kn",1.0);
		LATERAL_DEADBAND = Units.from("kn", 1.0);
		TIME_MANEUVER_RESTRICTION = Units.from("s", 0.0);
		TIME_PROXIMITY = Units.from("s", 0.0);
		TIME_LOOKAHEAD = Units.from("s", 0.0);
	}
	
	
	
	/** Returns TC in seconds */
	public static double get_tc(){
	  return  Units.to("s", PriorityCore.TC);
	}
	/** Returns the vertical speed value that defines "cruise" in [fpm] */
	public static double get_cruise_def(){
		return Units.to("fpm", CRUISE_DEF);
	}
	
	/** Returns the value of the relative uncertainty in vertical speed in [fpm] */
	public static double get_vertical_deadband(){
		return Units.to("fpm", VERTICAL_DEADBAND);
	}
	
	   /** Returns the value of the relative uncertainty in horizontal speed in [knot] */
    public static double get_lateral_relative_deadband(){
        return Units.to("kn", LATERAL_RELATIVE_DEADBAND);
    }
    
       
	/** Returns the value of the relative uncertainty in horizontal speed in [knot] */
	public static double get_lateral_deadband(){
		return Units.to("kn", LATERAL_DEADBAND);
	}
	   

        
	/** returns the value of the time maneuver restrictions. I don't know what this parameter is for */
	public static double get_time_maneuver_restriction(){
		return TIME_MANEUVER_RESTRICTION;
	}
	
	/** returns the value of the time proximity. I don't know what this parameter is for  */ 
	public static double get_time_proximity(){
		return TIME_PROXIMITY;
	}
	/** returns the value of the time lookahead. I don't know what this parameter is for  */ 
	public static double get_time_lookahead(){
		return TIME_LOOKAHEAD;
	}
	
	/** Set the value of the vertical speed that defines "cruise" in [fpm]<p> 
	 *  
	 * This value should never change; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable. 
	 */
	public static void set_cruise_def(double CruiseDef){
		CRUISE_DEF = Units.from("fpm", CruiseDef);
	}
	
	/** Set the value of the relative uncertainty in vertical speed in [fpm] <p>
	 *  
	 * This value should never change; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable. 
	 */
	public static void set_vertical_deadband(double VerticalDeadband){
		VERTICAL_DEADBAND = Units.from("fpm", VerticalDeadband);
	}
	
	/** Set the value of the relative uncertainty in the horizontal speed in [knot] <p>
	 *  
	 * This value should never change; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable. 
	 */
	
	   public static void set_tc(double tc){
         TC = Units.from("s", tc);
    }
	
	/** Set the value of the relative uncertainty in the horizontal speed in [knot]  
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	public static void set_lateral_relative_deadband(double LateralRelativeDeadband){
	     LATERAL_RELATIVE_DEADBAND = Units.from("kn", LateralRelativeDeadband);
	}
	/** Set the value of the relative uncertainty in the horizontal speed in [knot] <p>
    *
    * This value should never change; however, since its precise values
    * is still to be experimentally determined, it is left modifiable.
    */
	public static void set_lateral_deadband(double LateralDeadband){
		LATERAL_DEADBAND = Units.from("kn", LateralDeadband);
	}
	
	/** Set the value of the time to maneuver. I don't know what this parameter is for  */
	public static void set_time_maneuver_restriction(double TimeManeuverRestriction){
	    TIME_MANEUVER_RESTRICTION = TimeManeuverRestriction;
	}
	
	/** Set the value of the time proximity. I don't know what this parameter is for  */
	public static void set_time_proximity(double TimeProximity){
		TIME_PROXIMITY = TimeProximity;
	}
	
	/** Set the value of the time lookahead. I don't know what this parameter is for  */
	public static void set_time_lookahead(double TimeLookahead){
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
	public static boolean lateral_only (
			double sox, double soy, double soz, 
			double vox, double voy, double voz, 
			double six, double siy, double siz,
			double vix, double viy, double viz ) { 
	 
	   return PriorityCore.lateral_only(
			   new Vect3(Units.from("nm",sox),  Units.from("nm",soy),  Units.from("ft",soz)), 
			   new Vect3(Units.from("kn",vox), Units.from("kn",voy), Units.from("fpm",voz)), 
			   new Vect3(Units.from("nm",six),  Units.from("nm",siy),  Units.from("ft",siz)), 
			   new Vect3(Units.from("kn",vix), Units.from("kn",viy), Units.from("fpm",viz)));
	}
	
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
	public static boolean check(
		double sox, double soy, double soz, 
		double vox, double voy, double voz,
		boolean mo,
		double six, double siy, double siz,
		double vix, double viy, double viz,
		boolean mi) {
		
		return PriorityCore.check(
		   new Vect3(Units.from("nm",sox),  Units.from("nm",soy),  Units.from("ft",soz)), 
		   new Vect3(Units.from("kn",vox), Units.from("kn",voy), Units.from("fpm",voz)),
		   mo,
		   new Vect3(Units.from("nm",six),  Units.from("nm",siy),  Units.from("ft",siz)), 
		   new Vect3(Units.from("kn",vix), Units.from("kn",viy), Units.from("fpm",viz)),
		   mi);
	}
	
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
	public static boolean checkLL(
			double lato, double lono, double alto, 
			double trko, double gso, double vso,
			boolean mo,
			double lati, double loni, double alti,
			double trki, double gsi, double vsi,
			boolean mi) {


		Position po = new Position(LatLonAlt.make(lato,lono,alto));
		Velocity vo = Velocity.makeTrkGsVs(trko,"deg", gso,"kn", vso,"fpm");
		Position pi = new Position(LatLonAlt.make(lati,loni,alti));
		Velocity vi = Velocity.makeTrkGsVs(trki,"deg", gsi,"kn", vsi,"fpm");

		return check(po, vo, mo, pi, vi, mi);
	}

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
	public static boolean check(
			Position po, Velocity vo, boolean mo,
			Position pi, Velocity vi, boolean mi) {

		if (po.isLatLon()) {
			EuclideanProjection sp = Projection.createProjection(po.lla()); // this does NOT preserve altitudes (they will be relative)

			return PriorityCore.check(
					new Vect3(0.0,0.0,0.0), vo, mo,
					sp.project(pi.lla()), sp.projectVelocity(pi.lla(), vi), mi);
		} else {
			return PriorityCore.check(po.point(), vo, mo, pi.point(), vi, mi);
		}
	}

}
