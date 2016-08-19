/* Projection from Spherical Earth to Euclidean Plane
 * 
 * Authors:  George Hagen              NASA Langley Research Center  
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *           Anthony Narkawicz         NASA Langley Research Center
 *
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * A static holding class for universal projection information.  All projection objects should be retrieved from this class.
 * 
 * Remember that the given projection point becomes the origin of the Euclidean space, so to preserve absolute altitude (above 
 * sea level) information in the Euclidean space, it may be necessary to make the projection point's altitude zero.
 */
public class Projection {

      public enum ProjectionType {UNKNOWN_PROJECTION, SIMPLE, SIMPLE_NO_POLAR, ENU, AZIEQUI, FLATEARTH, STEREO, FAST, ORTHO};


	  private static EuclideanProjection projection = new ENUProjection(0,0,0);
	  private static ProjectionType ptype = ProjectionType.ENU;
	  
	   /**
	    * Returns a new projection for the current type with the given reference point.
	    * 
	    * Note that in the projected Euclidean frame, if two points are made using the 
	    * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	    * altitudes may have changed (the reference point is subtracted).  If you need 
	    * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	    * the reference point should have a zero altitude.
	    */
	  public static EuclideanProjection createProjection(double lat, double lon, double alt) {
		  return projection.makeNew(lat, lon, alt);
	  }

	   /**
	    * Returns a new projection for the current type with the given reference point.
	    * 
	    * Note that in the projected Euclidean frame, if two points are made using the 
	    * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	    * altitudes may have changed (the reference point is subtracted).  If you need 
	    * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	    * the reference point should have a zero altitude.
	    * 
	    * For example, if you call p2 = Projection.createProjection(p1).project(p1), p2 will 
	    * have an altitude of zero.  If you instead call p2 = Projection.createProjection(p1.zeroAlt(0)).project(p1),
	    * p2 will have the same altitude as p1.
	    */
	  public static EuclideanProjection createProjection(LatLonAlt lla) {
		  //f.pln(" @@@@@@@@@@@@@@@@@@@@@@@ EuclideanProjection.getProjection: lla = "+lla);
		  //f.pln(" @@@@@@@@@@@@@@@@@@@@@@@ ptype = "+ptype);
		  //f.pln(" @@@@@@@@@@@@@@@@@@@@@@@ projection = "+projection);
		  return projection.makeNew(lla);
	  }

	   /**
	    * Returns a new projection for the current type with the given reference point.
	    * This will return an altitude-preserving projection against the given Position if it is lat/lon.
	    * If it is Euclidean, the projection will be against the LatLonAlt.ZERO point.
	    * 
	    * Note that in the projected Euclidean frame, if two points are made using the 
	    * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	    * altitudes may have changed (the reference point is subtracted).  If you need 
	    * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	    * the reference point should have a zero altitude.
	    * 
	    * For example, if you call p2 = Projection.createProjection(p1).project(p1), p2 will 
	    * have an altitude of zero.  If you instead call p2 = Projection.createProjection(p1.zeroAlt(0)).project(p1),
	    * p2 will have the same altitude as p1.
	    */
	  public static EuclideanProjection createProjection(Position pos) {
		  LatLonAlt lla = pos.lla().zeroAlt();
		  if (!pos.isLatLon()) lla = LatLonAlt.ZERO;
		  return projection.makeNew(lla);
	  }


	  /**
	   * Geodetic projections into the Euclidean frame, for various reasons, tend to lose accuracy over long distances
	   * or when close to the poles.  This can be countered by examining trajectories as shorter segments at a time.  
	   * This is already done in Detector and Stratway, but not in any other tools.  For CDII, it is best
	   * to break up the ownship's plan in this way.  For CDSI and IntentBands, it is better to break up the traffic
	   * in this way.
	   * 
	   * This return an estimate on the suggested maximum segment size, depending on the current projection.
	   * 
	   * @param lat - latitude  [rad]
	   * @param accuracy - desired accuracy (allowable error) [m]
	   * @return the maximum length of a trajectory segment at the given latitude that preserves the desired accuracy.
	   */
	  public static double projectionConflictRange(double lat, double accuracy) {
		  return projection.conflictRange(lat, accuracy);
	  }

	  /**
	   * This is a range about which the projection will completely break down and start producing nonsensical answers.
	   * Attempting to use the projection at ranges greater than this is an error state (at ranges less than this but greater
	   * than the conflictRange, it may still be unacceptably inaccurate, however).
	   * 
	   * @return maximum range for the projection (in meters).	
	   */
	  public static double projectionMaxRange() {
		  return projection.maxRange();
	  }

	   /**
	    * Set the projection to a new type.  This is a global change.
	    */
	  public static void setProjectionType(ProjectionType t) {
		  ptype = t;
		  switch(t) {
		  	case SIMPLE: projection = new SimpleProjection(0,0,0); break;
		  	case SIMPLE_NO_POLAR: projection = new SimpleNoPolarProjection(0,0,0); break;
		  	case ENU: projection = new ENUProjection(0,0,0); break;
		  	case AZIEQUI: projection = new AziEquiProjection(0,0,0); break;
		  	case FLATEARTH: projection = new FlatEarthProjection(0,0,0); break;
		  	case ORTHO: projection = new OrthographicProjection(0,0,0); break;
		  	case STEREO: //projection = new StereographicProjection(0,0,0); break; // NOT WORKING
		  	case FAST: //projection  = new FastApproximateProjection(0,0,0); break;
		  	case UNKNOWN_PROJECTION: break; // do nothing
		  }
	  }

	   /**
	    * Given a string representation of a ProjectionType, return the ProjectionType
	    */
	  public static ProjectionType getProjectionTypeFromString(String s) {
		  try {
		    return Projection.ProjectionType.valueOf(s.toUpperCase());
		  } catch (IllegalArgumentException e) { 
			  return ProjectionType.UNKNOWN_PROJECTION;
		  }
	  }

	   /**
	    * Return the current ProjectionType
	    */
	  public static ProjectionType getProjectionType() {
		  return ptype;
	  }

}
