/* 
 * Generic Bands (an interface)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;

/**
 * This provides an interface to classes for various pure state conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
public interface GenericStateBands extends GenericBands, ErrorReporter {   

  /** 
   * Specify the ownship's position and velocity.  Once the ownship
   * is set, any existing 'band' information is cleared. This also
   * sets the coordinate frame for this object.
   *
   * @param id ownship identifier
   * @param s ownship position
   * @param v ownship velocity
   */
  public void setOwnship(String id, Position s, Velocity v);
  

   /** 
    * Add a traffic aircraft to this set of the conflict prevention
    * bands. No traffic aircraft can be added
    * until the ownship is set (see setOwnship()).
    * The coordinate frame of the traffic position must be the same as that of the object.
    *
    * @param id identifier of the traffic
    * @param si position of the traffic
    * @param vi velocity of the traffic
    */
   public void addTraffic(String id, Position si, Velocity vi);

}
