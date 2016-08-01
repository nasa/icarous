/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.List;

import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

/* 
 * Most urgent strategy based on distance at closest point of approach. When this distance is less than the minimum 
 * recovery separation given by D and H, time to closest point of approach is used. 
 */
public class DCPAUrgencyStrategy implements UrgencyStrategy {

  /**
   * @return most urgent traffic aircraft given for ownship, traffic and lookahead time T
   */

  public TrafficState mostUrgentAircraft(Detection3D detector, TrafficState ownship, List<TrafficState> traffic, double T) {
    TrafficState repac = TrafficState.INVALID;
    if (!ownship.isValid() || traffic.isEmpty()) {
      return repac;
    }
    double mindcpa = 0;
    double mintcpa = 0;
    double D = ACCoRDConfig.NMAC_D;
    double H = ACCoRDConfig.NMAC_H;
    Vect3 so = ownship.get_s();
    Velocity vo = ownship.get_v();
    for (int ac = 0; ac < traffic.size(); ++ac) {
      Vect3 si = traffic.get(ac).get_s();
      Velocity vi = traffic.get(ac).get_v();
      Vect3 s = so.Sub(si);
      Velocity v = vo.Sub(vi);
      ConflictData det = detector.conflictDetection(so,vo,si,vi,0,T); 
      if (det.conflict()) {  
        double tcpa = CD3D.tccpa(s,vo,vi,D,H);
        double dcpa = v.ScalAdd(tcpa,s).cyl_norm(D,H); 
        // If aircraft have almost same tcpa, select the one with smallest dcpa
        // Otherwise,  select aircraft with smallest tcpa 
        boolean tcpa_strategy = Util.almost_equals(tcpa,mintcpa,Util.PRECISION5) ? dcpa < mindcpa : tcpa < mintcpa;
        // If aircraft have almost same dcpa, select the one with smallest tcpa
        // Otherwise,  select aircraft with smallest dcpa 
        boolean dcpa_strategy = Util.almost_equals(dcpa,mindcpa,Util.PRECISION5) ? tcpa < mintcpa : dcpa < mindcpa;
        // If aircraft are both in a min recovery trajectory, follows tcpa strategy. Otherwise follows dcpa strategy
        if (!repac.isValid() || // There are no candidates
            (dcpa <= 1 ? mindcpa > 1 || tcpa_strategy : dcpa_strategy)) {
          repac = traffic.get(ac);
          mindcpa = dcpa;
          mintcpa = tcpa; 
        }
      }
    }
    return repac;
  }

  public UrgencyStrategy copy() {
    return new DCPAUrgencyStrategy();
  }

}
