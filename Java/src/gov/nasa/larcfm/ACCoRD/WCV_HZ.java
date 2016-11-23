/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

/* Horizontal Well Clear Volume concept based on Modified TAU,
 * and, in the vertical dimension, VMOD
 */

public class WCV_HZ extends WCV_TAUMOD {
  
  /** Constructor that a default instance of the WCV tables. */
  public WCV_HZ() {
    table = new WCVTable();
    wcv_vertical = new WCV_VMOD();
  }

  /** Constructor that specifies a particular instance of the WCV tables. */
  public WCV_HZ(WCVTable tab) {
  	table = tab.copy();
  	wcv_vertical = new WCV_VMOD();
  }  
  
  public WCV_HZ make() {
    return new WCV_HZ();
  }

  /**
   * Returns a deep copy of this WCV_HZ object, including any results that have been calculated.  
   */
  public WCV_HZ copy() {
    WCV_HZ ret = new WCV_HZ(table);
    ret.id = id;
    return ret;
  }
  
  public boolean contains(Detection3D cd) {
    if (cd instanceof WCV_HZ) {
      return containsTable((WCV_tvar)cd);
    }
    return false;
  }

}
