/* Core Priority Rules Computation 
 * Contact: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;

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
public final class PriorityCore  {

	private PriorityCore() {
		// disallow creation of objects of this type.
	}
	
	/** TC is a time back (e.g. 30 seconds) back from the time of closest approach.    
	 */
    public static double TC;
	
	/** 
	 * The definition of cruise.  If the absolute value a vehicle's vertical 
	 * speed is within this value then, by definition, the aircraft is in cruise.<p>
	 *  
	 * This value should be "final"; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable.
	 */
	public static double CRUISE_DEF;
	/** 
	 * This is a representation of the known uncertainty in the vertical speed.<p>
	 *  
	 * This value should be "final"; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable.
	 */
	public static double VERTICAL_DEADBAND;
	/** 
	 * This is a representation of the known uncertainty in the horizontal speed. <p>
	 *  
	 * This value should be "final"; however, since its precise values
	 * is still to be experimentally determined, it is left modifiable.
	 */
	
	public static double LATERAL_DEADBAND;

	
	/** This is a "relative" representation of known uncertainty in the vertical speed <p>
	 *    
	 * This value should be "final"; however, since its precise values
     * is still to be experimentally determined, it is left modifiable.
     */
	 public static double LATERAL_RELATIVE_DEADBAND;
	/** 
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return true if the aircraft is descending
	 */
	private static boolean isDescending(Vect3 v){
		return v.z <=  -CRUISE_DEF - VERTICAL_DEADBAND;
	}
	
	/**
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return return true if the aircraft is not descending 
	 */
	private static boolean isNotDescending(Vect3 v) {
		return v.z >= -CRUISE_DEF + VERTICAL_DEADBAND;
	}
	
	/** 
	 * 
	 * @param v a 3D vector representing an aircraft velocity vector
	 * @return 
	 */
	private static boolean isCruising(Vect3 v) {
		return Math.abs(v.z) <= CRUISE_DEF - VERTICAL_DEADBAND;
	}
	
	/** 
	 * 
	 * @param v  a 3D vector representing an aircraft velocity vector
	 * @return trune if the aircraft is ascending 
	 */
    private static boolean isAscending(Vect3 v) {
    	return v.z >= CRUISE_DEF + VERTICAL_DEADBAND;
    }
	/** 
	 * 
	 * @param v  a 3D vector representing an aircraft velocity vector
	 * @return true if the aircraft is not descending 
 	 */
    private static boolean isNotAscending(Vect3 v) {
      	return v.z <= CRUISE_DEF - VERTICAL_DEADBAND;
    }
    
    /**
     * 
     * @param v a 3D vector representing an aircraft velocity vector
     * @return  
     */
    private static boolean isNotCruising(Vect3 v) {
    	return (isDescending(v) || isAscending(v));
    	}
    
    /**
     * 
     * @param v a 3D vector representing an aircraft velocity vector
     * @return true if the aircraft is descending or cruising 
     */
    private static boolean PossiblyDescendingORCruising(Vect3 v) {
    	return (Math.abs(v.z + CRUISE_DEF))< VERTICAL_DEADBAND;
    }
    
    /**
     * 
     * @param v  a 3D vector representing an aircraft velocity vector
     * @return returns true if the aircraft is cruising or ascending 
     */
    private static boolean PossiblyCruisingORAscending(Vect3 v) {
    	return (Math.abs(v.z - CRUISE_DEF)) < VERTICAL_DEADBAND;
    }
    
//    /**
//     * 
//     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
//     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
//     * @return
//     */
//    private static boolean ModeCloseOneWay(Vect3 ao_V, Vect3  ai_V){
//        return  ( (isDescending(ao_V) && !isNotDescending(ai_V)) ||
//        		  (isCruising(ao_V) && !isNotCruising(ai_V))  ||
//        		  (isAscending(ao_V) && !isNotAscending(ai_V))
//        		);	
//    }
//    
//    /**
//     * 
//     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
//     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
//     * @return
//     */
//    private static boolean ModeClose(Vect3 ao_V, Vect3 ai_V){
//    	return (ModeCloseOneWay(ao_V,ai_V) || ModeCloseOneWay(ai_V,ao_V));
//    }

    /**
     * 
     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
     * @param ai_V  a 3D vector representing traffic  aircraft velocity vector
     * @return
     */
    private static boolean HorizontalRulesApply(Vect3 ao_V, Vect3 ai_V) {
    	return ( (isCruising(ao_V) && isCruising(ai_V)) ||
    			 (isDescending(ao_V) && isDescending(ai_V)) ||
    			 (isAscending(ao_V) && isAscending(ai_V))
    			);	
    }
    
    /**
     * 
     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
     * @return
     */
    private static boolean VerticalRulesApply(Vect3  ao_V, Vect3 ai_V) {
    	return ( (isDescending(ao_V) && isNotDescending(ai_V)) ||
    			(isDescending(ai_V) && isNotDescending(ao_V)) ||
    			(isCruising(ao_V) && isNotCruising(ai_V)) ||
    			(isCruising(ai_V) && isNotCruising(ao_V)) ||
    			(isAscending(ao_V) && isNotAscending(ai_V)) ||
    			(isAscending(ai_V) && isNotAscending(ao_V)) 
    			);
    }
    
    /**
     * 
     * @param so a 3D vector representing the position vector of the ownship
     * @param si a 3D vector representing the position vector of the traffic 
     * @param vo a  3D vector representing the velocity vector of the ownship
     * @param vi a 3D vector representing the velocity vector of the traffic 
     * @return the velocity component
     */
   private static double VelComponent(Vect3 so, Vect3 si, Vect3 vo, Vect3 vi) {
    	Vect2 w;
    	Vect2 z;
    	w = (so.Sub(si)).vect2();    	
    	z = (vo.Add(vi)).vect2();
    	if ( ! w.isZero()) {
    		return w.Scal(1.0/w.norm()).dot(z);
    	} else {
    		return 0.0;
    	}
   }
    
   
   /**
    * 
    * @param time t (should be tca) 
    * @param so a 3D vector representing the position vector of the ownship
    * @param si a 3D vector representing the position vector of the traffic 
    * @param vo a  3D vector representing the velocity vector of the ownship
    * @param vi a 3D vector representing the velocity vector of the traffic 
    * @return velocity  component at time t 
    */
   private static double VelComponentAt(double t, Vect3 so, Vect3 si, Vect3 vo, Vect3 vi) {
    Vect3 w;
    Vect3 z;
    w = vo.Scal(t);
    z = vi.Scal(t);
    return VelComponent(so.Add(w), si.Add(z),vo,vi);
   }
      
   /**
    * 
    * @param so a 3D vector representing the position vector of the ownship
    * @param si a 3D vector representing the position vector of the traffic 
    * @param vo a  3D vector representing the velocity vector of the ownship
    * @param vi a 3D vector representing the velocity vector of the traffic 
    * @return 
    */
  
   private static double EntryComponent(Vect3 so, Vect3 si, Vect3 vo, Vect3 vi) {
     Vect3 w;
     Vect3 z;
     double x,timeat;
     w = vo.Sub(vi);
     z = so.Sub(si);
     if ((w.vect2()).norm() < LATERAL_RELATIVE_DEADBAND) {
       return 0;
     }
     else{
       x = CD2D.tcpa(z.vect2(), vo.vect2(), vi.vect2());
       timeat = Math.max(0, x-TC);
       return VelComponentAt(timeat,so,si,vo,vi); 
     }
   }
    
          
   
   /**
    * 
    * @param ao_S a 3D vector representing the position vector of ownship
    * @param ao_V a 3D vector representing the velocity vector of the ownship
    * @param ai_S a 3D vector representing the position vector of traffic 
    * @param ai_V a 3D vector representing the velocity vector of traffic 
    * @return
    */
   private static boolean HorizontalDeadband(Vect3 ao_S, Vect3 ao_V, Vect3 ai_S, Vect3 ai_V) {
       
        return (Math.abs(EntryComponent(ao_S, ai_S, ao_V, ai_V) ) < LATERAL_DEADBAND);
    }
    
   /**
    * 
    * @param ao_V 
    * @param ai_V
    * @return
    */
  private static boolean VerticalDeadband(Vect3  ao_V, Vect3 ai_V){
	  return ( (PossiblyDescendingORCruising(ao_V) && ! isAscending(ai_V)) ||
			   (PossiblyCruisingORAscending(ao_V)  && ! isDescending(ai_V)) ||
			   (PossiblyDescendingORCruising(ai_V) && ! isAscending(ao_V)) ||
			   (PossiblyCruisingORAscending(ai_V)  && ! isDescending(ao_V))
			  );
  }
  
//  /**
//   * 
//   * @param ao_S a 3D vector representing the position vector of ownship
//   * @param ao_V a 3D veector representing the velocity vector of the ownship
//   * @param ai_S a 3D vector representing the position vector of traffic 
//   * @param ai_V a 3D vector representing the velocity vector of traffic 
//   * @return
//   */
//  private static boolean TotalDeadband(Vect3 ao_S, Vect3 ao_V, Vect3 ai_S, Vect3 ai_V){
//	   return ( VerticalDeadband(ao_V,ai_V) || VerticalDeadband(ai_V,ao_V) ||
//			   (HorizontalRulesApply(ao_V, ai_V) && HorizontalDeadband(ao_S,ao_V,ai_S,ai_V))
//			   );
//  }
  
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
	public static boolean lateral_only(Vect3  so, Vect3 vo, Vect3 si, Vect3 vi) {
	   return ( (! HorizontalDeadband(so,vo,si,vi)) && (EntryComponent(so,si,vo,vi) > 0));
	}
	
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
	public static boolean check(Vect3 so, Vect3 vo, boolean mo, Vect3 si, Vect3 vi, boolean mi) {
	      if ( mo ) { 
	    	  return true;
	      } else if ( mi ) {
	    	  return false;
	      } else if ( VerticalDeadband(vo,vi) ) {
	    	  return false;
	      } else if ( VerticalRulesApply(vo, vi) ) {
	    	  return (vo.z < vi.z);
	      } else if ( HorizontalRulesApply(vo, vi) ) {
	    	  return lateral_only(so, vo,si,vi);
	      } else {
	    	  return true;
	      }
	     
	}
	
	
}
