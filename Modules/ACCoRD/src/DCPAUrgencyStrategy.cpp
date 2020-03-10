/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * DCPAUrgencyStrategy.cpp
 *
 * Most urgent strategy based on distance at closest point of approach. When this distance is less than the minimum
 * recovery separation given by D and H, time to closest point of approach is used.
 *
 */

#include "ACCoRDConfig.h"
#include "CD3D.h"
#include "DCPAUrgencyStrategy.h"

namespace larcfm {

/**
 * @return most urgent traffic aircraft for given ownship, traffic and lookahead time T.
 * Return -1 if no aircraft is most urgent.
 */
int DCPAUrgencyStrategy::mostUrgentAircraft(const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const {
  (void)T; //bypass unused parameter warning (needed for interface)
  int repac = -1;
  if (!ownship.isValid() || traffic.empty()) {
    return repac;
  }
  double mindcpa = 0;
  double mintcpa = 0;
  double D = ACCoRDConfig::NMAC_D;
  double H = ACCoRDConfig::NMAC_H;
  Vect3 so = ownship.get_s();
  Velocity vo = ownship.get_v();
  for (TrafficState::nat ac = 0; ac < traffic.size(); ++ac) {
    Vect3 si = traffic[ac].get_s();
    Velocity vi = traffic[ac].get_v();
    Vect3 s = so.Sub(si);
    Vect3 v = vo.Sub(vi);
    double tcpa = CD3D::tccpa(s,vo,vi,D,H);
    double dcpa = v.ScalAdd(tcpa,s).cyl_norm(D,H);
    // If aircraft have almost same tcpa, select the one with smallest dcpa
    // Otherwise,  select aircraft with smallest tcpa
    bool tcpa_strategy = Util::almost_equals(tcpa,mintcpa,PRECISION5) ? dcpa < mindcpa : tcpa < mintcpa;
    // If aircraft have almost same dcpa, select the one with smallest tcpa
    // Otherwise,  select aircraft with smallest dcpa
    bool dcpa_strategy = Util::almost_equals(dcpa,mindcpa,PRECISION5) ? tcpa < mintcpa : dcpa < mindcpa;
    // If aircraft are both in a min recovery trajectory, follows tcpa strategy. Otherwise follows dcpa strategy
    if (repac < 0 || // There are no candidates
        (dcpa <= 1 ? mindcpa > 1 || tcpa_strategy : dcpa_strategy)) {
      repac = ac;
      mindcpa = dcpa;
      mintcpa = tcpa;
    }
  }
  return repac;
}

UrgencyStrategy* DCPAUrgencyStrategy::copy() const {
  return new DCPAUrgencyStrategy();
}

}
