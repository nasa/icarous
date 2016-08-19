/* 
 * Generic Bands (an interface)
 *
 * Contact: George Hagen, Jeff Maddalon
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;


/**
 * This provides an interface to classes for various conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
public interface GenericIntentBands extends GenericBands {

	  /** 
	   * Specify the ownship's position and velocity.  Once the ownship
	   * is set, any existing 'band' information is cleared. This also
	   * sets the coordinate frame for this object.
	   *
	   * @param s ownship position
	   * @param v ownship velocity
	   * @param time current time [s]
	   * @param tunit TODO
	   */
	  public void setOwnship(Position s, Velocity v, double time, String tunit);
	  
	  /** 
	   * Specify the ownship's position and velocity.  Once the ownship
	   * is set, any existing 'band' information is cleared.  This also
	   * sets the bands object to the lat/lon coordinate frame.
	   *
	   * @param lat north latitude [deg] of ownship
	   * @param lon east longitude [deg] of ownship
	   * @param hunit TODO
	   * @param alt altitude [feet] of ownship
	   * @param vunit TODO
	   * @param trk track angle [degrees clockwise from true north] of ownship
	   * @param trkunit TODO
	   * @param gs ground speed [knot] of ownship
	   * @param gsunit TODO
	   * @param vs vertical speed [feet/min] of ownship
	   * @param vsunit TODO
	   * @param time current time [s]
	   * @param tunit TODO
	   */
	   public void setOwnshipLL(double lat, double lon, String hpunit, double alt, String vpunit, double trk, String trkunit, double gs, String gsunit, double vs, String vsunit, double time, String tunit);

	   /** 
	    * Specify the ownship's position and velocity.  Once the ownship
	    * is set, any existing 'band' information is cleared.  This also
	   * sets the bands object to the Euclidean coordinate frame.
	    *
	    * @param sx x position [nmi] of ownship
	   * @param sy y position [nmi] of ownship
	   * @param hpunit TODO
	   * @param sz z position [feet] of ownship
	   * @param vpunit TODO
	   * @param vx x speed component [knot] of ownship
	   * @param vy y speed component [knot] of ownship
	   * @param hvunit TODO
	   * @param vz z speed component [feet/min] of ownship
	   * @param vvunit TODO
	   * @param time current time [s]
	   * @param tunit TODO
	    */
	   public void setOwnshipXYZ(double sx, double sy, String hpunit, double sz, String vpunit, double vx, double vy, String hvunit, double vz, String vvunit, double time, String tunit);
	
//    /** 
//     * Specify the ownship's position and velocity.  Once the ownship
//     * is set, any existing 'band' information is cleared.  Whether
//     * the position are specified in latitude/longitude or Euclidean
//     * and the velocities are specified in track, ground speed, or
//     * Euclidean depends on the setLatLon() and setTrackVelocity() methods.
//     *
//     * @param lat_sx north latitude [deg] or x position [nmi] of ownship
//     * @param lon_sy east longitude [deg] or y position [nmi] of ownship
//     * @param alt_sz altitude [feet] or z position [feet] of ownship
//     * @param trk_vx track angle [degrees clockwise from true north] or x component [knot] of ownship
//     * @param gs_vy ground speed [knot] or y component [knot] of ownship
//     * @param vs_vz vertical speed [feet/min] or z component [feet/min] of ownship
//     * @param time (absolute) time [sec] of ownship
//     * 
//     * @deprecated
//     */
//    public void setOwnship(double lat_sx, double lon_sy, double alt_sz, 
//			   double trk_vx, double gs_vy, double vs_vz, double time);

    /** 
     * Add a traffic aircraft to this set of the conflict prevention
     * bands. Whether the position are specified in Latitude/longitude
     * or cartesian and the velocities are specified in track, ground
     * speed, or cartesian depends on the setLatLon() and
     * setTrackVelocity() methods.  No traffic aircraft can be added
     * until the ownship is set (see setOwnship()).
     *
     * @param fp intent flight plan for traffic.  This may be either in nmi or latlong.
     * @return false if no ownship has been set or there is a latlon mismatch
     */
    public boolean addTraffic(Plan fp);

}
