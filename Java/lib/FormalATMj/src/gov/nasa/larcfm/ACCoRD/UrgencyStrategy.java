/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.List;

/* 
 * Generic interface for most urgent aircraft strategy.
 */
public interface UrgencyStrategy {
  
  public TrafficState mostUrgentAircraft(Detection3D detector, TrafficState ownship, List<TrafficState> traffic, 
      double T);
  
  public UrgencyStrategy copy();
  
}
