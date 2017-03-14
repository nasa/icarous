/* ====================================================================
 * Authors:      George Hagen, Rick Butler and Jeff Maddalon,
 * Organization: NASA/Langley Research Center
 * Website:      http://shemesh.larc.nasa.gov/fm/
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * ====================================================================
 */

package gov.nasa.larcfm.ACCoRD;
import gov.nasa.larcfm.Util.Units;
  /**
   * This class captures the default parameters for ACCORD.
   *
   */
public final class ACCoRDConfig {

    public static final boolean debugPrintTime = false; // ************* makes it easier to read runtest results 
	public static boolean NewRepulsiveCrit = false; 
	// NOTE [CAM]. When flag is set to true, the new horizontal and vertical repulsive criteria are used
	
    public static boolean allowSpeedOnlyResolutions = false;
	  	
    static public double minHorizExitSpeedLoS_default = Units.from("kn",100.0);
    static public double minVertExitSpeedLoS_default = Units.from("fpm",1499.99999999);
    static public double maxGs_default = Units.from("kn",700.0);
    static public double minGs_default = Units.from("kn",150.0);       // must be greater than 0
    static public double maxVs_default = Units.from("fpm",5000.0);
    
	static public double gsSearchLosDiscard = Units.from("nmi",1.5);
	static public double vsDiscretization_default = Units.from("fpm",10.0);
	
	final public static double NMAC_D = Units.from("ft",500); // Defined in RTCA SC-147
	final public static double NMAC_H = Units.from("ft",100); // Defined in RTCA SC-147

	public static void debugln(String s) {
	  if (debugPrintTime) {
	    System.out.println(s);
	  }
	}
	
	public static void setNewRepulsiveCrit(boolean flag) {
	  NewRepulsiveCrit = flag;
	} 

	public static boolean getNewRepulsiveCrit() {
	  return NewRepulsiveCrit;
	} 
	
    // internal units right now
    public static void setGsSearchLosDiscard(double val) {
       gsSearchLosDiscard = val;
    }

    public static void setAllowSpeedOnlyResolutions(boolean flag) {
      allowSpeedOnlyResolutions = flag;
    }

}
