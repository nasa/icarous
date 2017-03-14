/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicBandsCore.h"
#include "KinematicBandsParameters.h"
#include "Constants.h"
#include "Util.h"
#include "string_util.h"
#include "format.h"
#include "CriteriaCore.h"
#include <string>
#include <vector>

namespace larcfm {

TCASTable KinematicBandsCore::RA = TCASTable();

KinematicBandsCore::KinematicBandsCore(const KinematicBandsParameters& params) {
  ownship = TrafficState::INVALID;
  traffic = std::vector<TrafficState>();
  parameters = KinematicBandsParameters(params);
  most_urgent_ac = TrafficState::INVALID;
  conflict_acs_ = std::vector< std::vector<TrafficState> >();
  tiov_ = std::vector<Interval>();
  current_alert_ = 0;
  reset();
}

KinematicBandsCore::KinematicBandsCore(const KinematicBandsCore& core) {
  setKinematicBandsCore(core);
}

KinematicBandsCore& KinematicBandsCore::operator=(const KinematicBandsCore& core) {
  setKinematicBandsCore(core);
  return *this;
}

KinematicBandsCore::~KinematicBandsCore() {
  clear();
}

/**
 * Set kinematic bands core
 */
void KinematicBandsCore::setKinematicBandsCore(const KinematicBandsCore& core) {
  ownship = core.ownship;
  traffic = std::vector<TrafficState>();
  traffic.insert(traffic.end(),core.traffic.begin(),core.traffic.end());
  parameters = KinematicBandsParameters(core.parameters);
  most_urgent_ac = core.most_urgent_ac;
  conflict_acs_ = std::vector< std::vector<TrafficState> >();
  tiov_ = std::vector<Interval>();
  current_alert_ = 0;
  reset();
}

/**
 *  Clear ownship and traffic data from this object.
 */
void KinematicBandsCore::clear() {
  ownship = TrafficState::INVALID;
  traffic.clear();
  reset();
}

/**
 *  Reset cached values
 */
void KinematicBandsCore::reset() {
  outdated_ = true;
  epsh_ = 0;
  epsv_ = 0;
  tiov_.clear();
  current_alert_ = 0;
}

/**
 *  Update cached values
 */
void KinematicBandsCore::update() {
  if (outdated_) {
    current_alert_ = 0;
    for (int alert_level=1; alert_level <= parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
      if (alert_level-1 >= (int) conflict_acs_.size()) {
        conflict_acs_.push_back(std::vector<TrafficState>());
      } else {
        conflict_acs_[alert_level-1].clear();
      }
      conflict_aircraft(alert_level);
      if (!conflict_acs_[alert_level-1].empty()) {
        current_alert_ = alert_level;
      }
    }
    epsh_ = epsilonH(ownship,most_urgent_ac);
    epsv_ = epsilonV(ownship,most_urgent_ac);
    outdated_ = false;
  }
}

/**
 * Returns current alert level
 */
int KinematicBandsCore::currentAlertLevel() {
  update();
  return current_alert_;
}

/**
 *  Returns horizontal epsilon for implicit coordination with respect to criteria ac
 */
int KinematicBandsCore::epsilonH() {
  update();
  return epsh_;
}

/**
 *  Returns vertical epsilon for implicit coordination with respect to criteria ac
 */
int KinematicBandsCore::epsilonV() {
  update();
  return epsv_;
}

/**
 * Returns actual minimum horizontal separation for recovery bands in internal units.
 */
double KinematicBandsCore::minHorizontalRecovery() const {
  double min_horizontal_recovery = parameters.getMinHorizontalRecovery();
  if (min_horizontal_recovery > 0)
    return min_horizontal_recovery;
  int sl = !hasOwnship() ? 3 : Util::max(3,TCASTable::getSensitivityLevel(ownship.altitude()));
  return RA.getHMD(sl);
}

/**
 * Returns actual minimum vertical separation for recovery bands in internal units.
 */
double KinematicBandsCore::minVerticalRecovery() const {
  double min_vertical_recovery = parameters.getMinVerticalRecovery();
  if (min_vertical_recovery > 0)
    return min_vertical_recovery;
  int sl = !hasOwnship() ? 3 : Util::max(3,TCASTable::getSensitivityLevel(ownship.altitude()));
  return RA.getZTHR(sl);
}

bool KinematicBandsCore::hasOwnship() const {
  return ownship.isValid();
}

TrafficState KinematicBandsCore::intruder(const std::string& id) const {
  return TrafficState::findAircraft(traffic,id);
}

bool KinematicBandsCore::hasTraffic() const {
  return traffic.size() > 0;
}

/**
 * Put in conflict_acs_ the list of aircraft predicted to be in conflict for the given alert level.
 * Requires: 1 <= alert_level <= parameters.alertor.mostSevereAlertLevel()
 */
void KinematicBandsCore::conflict_aircraft(int alert_level) {
  double tin  = PINFINITY;
  double tout = NINFINITY;
  bool conflict_band = BandsRegion::isConflictBand(parameters.alertor.getLevel(alert_level).getRegion());
  Detection3D* detector = parameters.alertor.getLevel(alert_level).getDetectorRef();
  double alerting_time = Util::min(parameters.getLookaheadTime(),
      parameters.alertor.getLevel(alert_level).getAlertingTime());
  for (TrafficState::nat i = 0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    ConflictData det = detector->conflictDetection(ownship.get_s(),ownship.get_v(),ac.get_s(),ac.get_v(),
        0,parameters.getLookaheadTime());
    bool lowc = detector->violation(ownship.get_s(),ownship.get_v(),ac.get_s(),ac.get_v());
    if (lowc || det.conflict()) {
      if (conflict_band && (lowc || det.getTimeIn() < alerting_time)) {
        conflict_acs_[alert_level-1].push_back(ac);
      }
      tin = Util::min(tin,det.getTimeIn());
      tout = Util::max(tout,det.getTimeOut());
    }
  }
  tiov_.push_back(Interval(tin,tout));
}

/**
 * Return list of conflict aircraft for a given alert level.
 * Requires: 1 <= alert_level <= parameters.alertor.mostSevereAlertLevel()
 */
std::vector<TrafficState> const & KinematicBandsCore::conflictAircraft(int alert_level) {
  update();
  if (alert_level >= 1 && alert_level <= parameters.alertor.mostSevereAlertLevel()) {
    return conflict_acs_[alert_level-1];
  }
  return TrafficState::INVALIDL;
}

/**
 * Return time interval of violation for given alert level
 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
 */
Interval const & KinematicBandsCore::timeIntervalOfViolation(int alert_level) {
  update();
  if (alert_level >= 1 && alert_level <= parameters.alertor.mostSevereAlertLevel()) {
    return tiov_[alert_level-1];
  }
  return Interval::EMPTY;
}

int KinematicBandsCore::epsilonH(const TrafficState& ownship, const TrafficState& ac) {
  if (ownship.isValid() && ac.isValid()) {
    Vect2 s = ownship.get_s().Sub(ac.get_s()).vect2();
    Vect2 v = ownship.get_v().Sub(ac.get_v()).vect2();
    return CriteriaCore::horizontalCoordination(s,v);
  } else {
    return 0;
  }
}

int KinematicBandsCore::epsilonV(const TrafficState& ownship, const TrafficState& ac) {
  if (ownship.isValid() && ac.isValid()) {
    Vect3 s = ownship.get_s().Sub(ac.get_s());
    return CriteriaCore::verticalCoordinationLoS(s,ownship.get_v(),ac.get_v(),
        ownship.getId(), ac.getId());
  } else {
    return 0;
  }
}

TrafficState const & KinematicBandsCore::criteria_ac() const {
  return parameters.isEnabledConflictCriteria() ? most_urgent_ac : TrafficState::INVALID;
}

TrafficState const & KinematicBandsCore::recovery_ac() const {
  return parameters.isEnabledRecoveryCriteria() ? most_urgent_ac : TrafficState::INVALID;
}

std::string KinematicBandsCore::toString() const {
  int precision = Constants::get_output_precision();
  std::string s="";
  s+="## KinematicBandsCore Parameters\n";
  s+=parameters.toString();
  s+="## KinematicBandsCore Internals\n";
  s+="outdated_ = "+Fmb(outdated_)+"\n";
  s+="most_urgent_ac_ = "+most_urgent_ac.getId()+"\n";
  s+="epsh_ = "+Fmi(epsh_)+"\n";
  s+="epsv_ = "+Fmi(epsv_)+"\n";
  for (TrafficState::nat i=0; i < conflict_acs_.size(); ++i) {
    s+="conflict_acs_["+Fmi(i)+"]: "+
        TrafficState::listToString(conflict_acs_[i])+"\n";
  }
  for (Interval::nat i=0; i < tiov_.size(); ++i) {
    s+="tiov_["+Fmi(i)+"]: "+
        tiov_[i].toString(precision)+"\n";
  }
  s+="## Ownship and Traffic\n";
  s+="NAME sx sy sz vx vy vz time\n";
  s+="[none] [m] [m] [m] [m/s] [m/s] [m/s] [s]\n";
  s+=ownship.getId()+", "+ownship.get_s().formatXYZ(precision,"",", ","")+
      ", "+ownship.get_v().formatXYZ(precision,"",", ","")+", "+
      FmPrecision(ownship.getTime(),precision)+"\n";
  for (TrafficState::nat i = 0; i < traffic.size(); i++) {
    s+=traffic[i].getId()+", "+traffic[i].get_s().formatXYZ(precision,"",", ","")+
        ", "+traffic[i].get_v().formatXYZ(precision,"",", ","")+
        ", "+FmPrecision(traffic[i].getTime(),precision)+"\n";
  }
  s+="##\n";
  return s;

}

}
