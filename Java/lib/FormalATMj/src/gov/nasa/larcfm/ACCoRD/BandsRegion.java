/* 
 * Classification of Regions
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

/**
 * This enumeration provides constants to indicate the type of a
 * region of conflict prevention information.
 */
public enum BandsRegion { 
  /* UNKNOWN : Returned when a band is requested for a value beyond the min-max range
   * NONE: No band (green)
   * NEAR: Near conflict band (red)
   * RECOVERY: Band for an infeasible or loss of separation case (dashed red)
   * FAR: Far conflict band (yellow)
   */

  UNKNOWN("<UNKNOWN>"), NONE("<NONE>"), NEAR("<NEAR>"), RECOVERY("<RECOVERY>"), MID("<MID>"), FAR("<FAR>"), ;

  private String name;

  BandsRegion(String name) {
    this.name = name;
  }

  public String toString() {
    return name;
  }

  public String toPVS() {
    switch (this) {
    case NONE: return "NONE";
    case NEAR: return "NEAR";
    case RECOVERY: return "RECOVERY";
    case MID: return "MID";
    case FAR: return "FAR";
    default: return "UNKNOWN";
    }
  }
  
  boolean isResolutionBand() {
    return this == NONE || this == RECOVERY;
  }

}
