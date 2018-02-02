/*
 * ================================================================================
 * Daidalus : This class implements the interface to DAIDALUS (Detect and Avoid
 * Alerting Logic for Unmanned Systems).
 *
 * Contact:  Cesar Munoz               NASA Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * ==================================================================================
 */

#include "Daidalus.h"
#include "TCASTable.h"
#include "TCAS3D.h"
#include "WCV_TAUMOD.h"
#include "CD3D.h"
#include "ACCoRDConfig.h"
#include "CDCylinder.h"
#include "ConflictData.h"
#include "LossData.h"
#include "format.h"
#include "NoneUrgencyStrategy.h"
#include "Constants.h"

namespace larcfm {

/**
 * Create a new Daidalus object such that
 * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
 * - Maneuver guidance logic assumes instantaneous maneuvers
 * - Bands saturate at DMOD/ZTHR
 */
Daidalus::Daidalus() : error("Daidalus") {
  parameters = KinematicBandsParameters();
  urgency_strat_ = new NoneUrgencyStrategy();
  wind_vector_ = Velocity::ZEROV();
  current_time_ = 0;
  ownship_ = TrafficState::INVALID;
  traffic_ = std::vector<TrafficState>();
  set_WC_SC_228_MOPS();
}

/**
 * Create a new Daidalus object and copy all configuration parameters and traffic information
 * from another Daidalus object.
 */
Daidalus::Daidalus(const Daidalus& daa) : error("Daidalus") {
  parameters = KinematicBandsParameters(daa.parameters);
  urgency_strat_ = daa.urgency_strat_->copy();
  wind_vector_ = daa.wind_vector_;
  current_time_ = daa.current_time_;
  ownship_ = daa.ownship_;
  traffic_ = std::vector<TrafficState>();
  traffic_.insert(traffic_.end(),daa.traffic_.begin(),daa.traffic_.end());
}

Daidalus::~Daidalus() {
  traffic_.clear();
  delete urgency_strat_;
}

Daidalus& Daidalus::operator=(const Daidalus& daa) {
  parameters = KinematicBandsParameters(daa.parameters);
  delete urgency_strat_;
  urgency_strat_ = daa.urgency_strat_->copy();
  wind_vector_ = daa.wind_vector_;
  current_time_ = daa.current_time_;
  ownship_ = daa.ownship_;
  traffic_ = std::vector<TrafficState>();
  traffic_.insert(traffic_.end(),daa.traffic_.begin(),daa.traffic_.end());
  return *this;
}

/*
 * Set Daidalus object such that
 * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
 * - Maneuver guidance logic assumes instantaneous maneuvers
 * - Bands saturate at DMOD/ZTHR
 */
void Daidalus::set_WC_SC_228_MOPS() {
  parameters.alertor = AlertLevels::WC_SC_228_Thresholds();
  parameters.setInstantaneousBands();
  parameters.setCollisionAvoidanceBands(false);
  parameters.setMinHorizontalRecovery(0.66,"nmi");
  parameters.setMinVerticalRecovery(450,"ft");
}

/*
 * Set Daidalus object such that
 * - Alerting thresholds are buffered
 * - Maneuver guidance logic assumes kinematic maneuvers
 * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
 *   when type is false.
 * - Bands don't saturate until NMAC
 */
void Daidalus::set_Buffered_WC_SC_228_MOPS(bool type) {
  parameters.alertor = AlertLevels::Buffered_WC_SC_228_Thresholds();
  parameters.setKinematicBands(type);
  parameters.setCollisionAvoidanceBands(true);
  parameters.setCollisionAvoidanceBandsFactor(0.2);
  parameters.setMinHorizontalRecovery(1.0,"nmi");
  parameters.setMinVerticalRecovery(450,"ft");
}

/**
 * Clear aircraft list, reset current time and wind vector.
 */
void Daidalus::reset() {
  ownship_ = TrafficState::INVALID;
  traffic_.clear();
  wind_vector_ = Velocity::ZEROV();
  current_time_ = 0;
}

/**
 * @return number of aircraft, including ownship.
 */
int Daidalus::numberOfAircraft() const {
  if (!ownship_.isValid()) {
    return 0;
  } else {
    return traffic_.size()+1;
  }
}

/**
 * @return last traffic index. Every traffic aircraft has an index between 1 and lastTrafficIndex.
 * The index 0 is reserved for the ownship. When lastTrafficIndex is 0, the ownship is set but no
 * traffic aircraft has been set. When lastTrafficIndex is negative, ownship has not been set.
 */
int Daidalus::lastTrafficIndex() const {
  if (!ownship_.isValid()) {
    return -1;
  } else {
    return traffic_.size();
  }
}

/**
 * Get wind vector
 */
Velocity const & Daidalus::getWindField() const {
  return wind_vector_;
}

/**
 * Set wind vector (common to all aircraft)
 */
void Daidalus::setWindField(const Velocity& wind) {
  if (lastTrafficIndex() >= 0) {
    Velocity delta_wind = wind_vector_.Sub(wind);
    ownship_ = TrafficState::makeOwnship(ownship_.getId(),ownship_.getPosition(),
        ownship_.getVelocity().Add(delta_wind),ownship_.getTime());
    for (TrafficState::nat i=0; i < traffic_.size(); ++i) {
      TrafficState ac = traffic_[i];
      traffic_[i] = ownship_.makeIntruder(ac.getId(),ac.getPosition(),
          ac.getVelocity().Add(delta_wind));
    }
  }
  wind_vector_ = wind;
}

/**
 * Clear all aircraft and set ownship state and current time. Velocity vector is ground velocity.
 */
void Daidalus::setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  traffic_.clear();
  ownship_ = TrafficState::makeOwnship(id,pos,vel.Sub(wind_vector_),time);
  current_time_ = time;
}

/**
 * Clear all aircraft and set ownship state and current time. Velocity vector is ground velocity.
 */
void Daidalus::setOwnshipState(const TrafficState& ownship, double time) {
  setOwnshipState(ownship.getId(),ownship.getPosition(),ownship.getVelocity(),
      time);
}

/**
 * Add traffic state at given time. Velocity vector is ground velocity.
 * If time is different from current time, traffic state is projected, past or future,
 * into current time assuming wind information. If it's the first aircraft, this aircraft is
 * set as the ownship. Return aircraft index.
 */
int Daidalus::addTrafficState(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  if (lastTrafficIndex() < 0) {
    setOwnshipState(id,pos,vel,time);
    return 0;
  } else {
    double dt = current_time_-time;
    Position pt = dt == 0 ? pos : pos.linear(vel,dt);
    TrafficState ac = ownship_.makeIntruder(id,pt,vel.Sub(wind_vector_));
    if (ac.isValid()) {
      traffic_.push_back(ac);
      return traffic_.size();
    } else {
      return -1;
    }
  }
}

/**
 * Add traffic state at current time. Velocity vector is ground velocity.
 * If it's the first aircraft, this aircraft is set as the ownship. Return aircraft index.
 */
int Daidalus::addTrafficState(const std::string& id, const Position& pos, const Velocity& vel) {
  return addTrafficState(id,pos,vel,getCurrentTime());
}

/**
 * Add traffic state at given time.
 * If time is different from current time, traffic state is projected, past or future,
 * into current time assuming wind information. If it's the first aircraft, this aircraft is
 * set as the ownship. Return aircraft index.
 */
int Daidalus::addTrafficState(const TrafficState& ac, double time) {
  return addTrafficState(ac.getId(),ac.getPosition(),ac.getVelocity(),time);
}

/**
 * Add traffic state at current time.
 * If it's the first aircraft, this aircraft is set as the ownship.
 * Return aircraft index.
 */
int Daidalus::addTrafficState(const TrafficState& ac) {
  return addTrafficState(ac.getId(),ac.getPosition(),ac.getVelocity());
}

/**
 * Exchange ownship aircraft with aircraft at index ac_idx.
 */
void Daidalus::resetOwnship(int ac_idx) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    int ac = ac_idx-1;
    TrafficState new_own = TrafficState::makeOwnship(traffic_[ac]);
    TrafficState old_own = new_own.makeIntruder(ownship_);
    ownship_ = new_own;
    for (TrafficState::nat i = 0; i < traffic_.size(); ++i) {
      if (i == (TrafficState::nat)ac) {
        traffic_[i] = old_own;
      } else {
        traffic_[i] = ownship_.makeIntruder(traffic_[i]);
      }
    }
  } else {
    error.addError("resetOwnship: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
}

/**
 * Exchange ownship aircraft with aircraft named id.
 */
void Daidalus::resetOwnship(const std::string& id) {
  resetOwnship(aircraftIndex(id));
}

/**
 * Get index of aircraft with given name. Return -1 if no such index exists
 */
int Daidalus::aircraftIndex(const std::string& name) const {
  if (lastTrafficIndex() >= 0) {
    if (ownship_.getId() == name) {
      return 0;
    }
    for (TrafficState::nat i = 0; i < traffic_.size(); ++i) {
      if (traffic_[i].getId() == name)
        return i+1;
    }
  }
  return -1;
}

/**
 * @return current time.
 */
double Daidalus::getCurrentTime() const {
  return current_time_;
}

/**
 * Linearly projects all aircraft states to time t [s] and set current time to t.
 */
void Daidalus::setCurrentTime(double time) {
  if (time != current_time_ && lastTrafficIndex() >= 0) {
    double dt = time-current_time_;
    Velocity vo = ownship_.getVelocity().Add(wind_vector_); // Original ground velocity
    Position po = ownship_.getPosition().linear(vo,dt);
    ownship_ = TrafficState::makeOwnship(ownship_.getId(),po,ownship_.getVelocity(),time);
    for (TrafficState::nat i=0; i < traffic_.size(); ++i) {
      TrafficState ac = traffic_[i];
      Velocity vi = ac.getVelocity().Add(wind_vector_); // Original ground velocity
      Position pi = ac.getPosition().linear(vi,dt);
      traffic_[i] = ownship_.makeIntruder(ac.getId(),pi,ac.getVelocity());
    }
    current_time_ = time;
  }
}

/**
 * Computes alerting type of ownship and aircraft at index ac_idx for current
 * aircraft states.  The number 0 means no alert. A negative number means
 * that aircraft index is not valid.
 * When the alertor object has been configured to consider ownship maneuvers, i.e.,
 * using spread values, the alerting logic could also use information about the ownship
 * turning, accelerating, and climbing status as follows:
 * - turning < 0: ownship is turning left, turning > 0: ownship is turning right, turning = 0:
 * do not make any turning assumption about the ownship.
 * - accelerating < 0: ownship is decelerating, accelerating > 0: ownship is accelerating,
 * accelerating = 0: do not make any accelerating assumption about the ownship.
 * - climbing < 0: ownship is descending, climbing > 0: ownship is climbing, climbing = 0:
 * do not make any climbing assumption about the ownship.
 */
int Daidalus::alerting(int ac_idx, int turning, int accelerating, int climbing) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    TrafficState ac = traffic_[ac_idx-1];
    kinematicMultiBands(kb_);
    return kb_.alerting(ac, turning, accelerating, climbing);
  } else {
    error.addError("alerting: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return -1;
  }
}

/**
 * Computes alerting type of ownship and aircraft at index ac_idx for current
 * aircraft states.  The number 0 means no alert. A negative number means
 * that aircraft index is not valid.
 */
int Daidalus::alerting(int ac_idx) {
  return alerting(ac_idx,0,0,0);
}

/**
 * Detects conflict with aircraft at index ac_idx for given alert level.
 * Conflict data provides time to violation and time to end of violation
 * within lookahead time.
 */
ConflictData Daidalus::detection(int ac_idx, int alert_level) const {
  Detection3D* detector = parameters.alertor.detectorRef(alert_level);
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex() && detector != NULL) {
    TrafficState ac = traffic_[ac_idx-1];
    return detector->conflictDetection(ownship_.get_s(),ownship_.get_v(),ac.get_s(),ac.get_v(),
        0,parameters.getLookaheadTime());
  } else {
    error.addError("detection: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return ConflictData();
  }
}

/**
 * Detects conflict with aircraft at index ac_idx for conflict alert level.
 * Conflict data provides time to violation and time to end of violation
 * within lookahead time.
 */
ConflictData Daidalus::detection(int ac_idx) const {
  return detection(ac_idx,0);
}

/**
 * @return time to violation, in seconds, between ownship and aircraft at index ac_idx, for the
 * lookahead time. The returned time is relative to current time. PINFINITY means no
 * conflict within lookahead time. NaN means that aircraft index is out of range.
 */
double Daidalus::timeToViolation(int ac_idx) const {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    ConflictData det = detection(ac_idx);
    if (det.conflict()) {
      return det.getTimeIn();
    }
    return PINFINITY;
  }
  else {
    error.addError("timeToViolation: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return NaN;
  }
}

/**
 * Compute in bands the kinematic multi bands at current time. Computation of bands is lazy,
 * they are only computed when needed.
 */
void Daidalus::kinematicMultiBands(KinematicMultiBands& bands) const {
  bands.clear();
  if (lastTrafficIndex() < 0) {
    error.addError("getKinematicBands: ownship has not been set");
  } else {
    bands.setKinematicBandsParameters(parameters);
    bands.setOwnship(ownship_);
    bands.setTraffic(traffic_);
    bands.setMostUrgentAircraft(mostUrgentAircraft());
  }
}

/**
 * Returns state of ownship.
 */
TrafficState const & Daidalus::getOwnshipState() const {
  return ownship_;
}

/**
 * Returns state of aircraft at index ac_idx
 */
TrafficState const & Daidalus::getAircraftState(int ac_idx) const {
  if (0 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    if (ac_idx == 0) {
      return ownship_;
    } else {
      return traffic_[ac_idx-1];
    }
  } else {
    error.addError("getAircraftState: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return TrafficState::INVALID;
  }
}

/**
 * @return reference to strategy for computing most urgent aircraft.
 */
UrgencyStrategy* Daidalus::getUrgencyStrategyRef() const {
  return urgency_strat_;
}

/**
 * Set strategy for computing most urgent aircraft.
 */
void Daidalus::setUrgencyStrategy(const UrgencyStrategy* strat) {
  delete urgency_strat_;
  urgency_strat_ = strat->copy();
}

/**
 * Returns most urgent aircraft for given alert level according to urgency strategy.
 */
TrafficState Daidalus::mostUrgentAircraft(int alert_level) const {
  Detection3D* detector = parameters.alertor.detectorRef(alert_level);
  if (lastTrafficIndex() > 0 && detector != NULL) {
    return urgency_strat_->mostUrgentAircraft(detector,ownship_,traffic_,parameters.getLookaheadTime());
  } else {
    return TrafficState::INVALID;
  }
}

/**
 * Returns most urgent aircraft for conflict alert level according to urgency strategy.
 */
TrafficState Daidalus::mostUrgentAircraft() const {
  return mostUrgentAircraft(0);
}

void Daidalus::add_blob(std::vector< std::vector<Position> >& blobs, std::deque<Position>& vin, std::deque<Position>& vout) {
  if (vin.empty() && vout.empty()) {
    return;
  }
  // Add conflict contour
  std::vector<Position> blob = std::vector<Position>();
  blob.insert(blob.end(),vin.begin(),vin.end());
  blob.insert(blob.end(),vout.begin(),vout.end());
  blobs.push_back(blob);
  vin.clear();
  vout.clear();
}

/**
 * Computes horizontal contours contributed by aircraft at index ac_idx, for
 * given alert level. A contour is a non-empty list of points in counter-clockwise
 * direction representing a polygon.
 * @param blobs list of track contours returned by reference.
 * @param ac_idx is the index of the aircraft used to compute the contours.
 */
void Daidalus::horizontalContours(std::vector< std::vector<Position> >& blobs, int ac_idx, int alert_level) {
  Detection3D* detector = parameters.alertor.detectorRef(alert_level);
  blobs.clear();
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex() && detector != NULL) {
    std::deque<Position> vin = std::deque<Position>();
    Position po = ownship_.getPosition();
    Velocity vo = ownship_.getVelocity();
    TrafficState intruder = traffic_[ac_idx-1];
    Vect3 si = intruder.get_s();
    Velocity vi = intruder.get_v();
    double current_trk = vo.trk();
    std::deque<Position> vout = std::deque<Position>();
    /* First step: Computes conflict contour (contour in the current path of the aircraft).
     * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
     * contour will be computed. Otherwise, stops at the first non-conflict degree.
     */
    double right = 0; // Contour conflict limit to the right relative to current track  [0-2pi rad]
    double two_pi = 2*Pi;
    for (; right < two_pi; right += parameters.getTrackStep()) {
      Velocity vop = vo.mkTrk(current_trk+right);
      LossData los = detector->conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
          0,parameters.getLookaheadTime());
      if ( !los.conflict() ) {
        break;
      }
      if (los.getTimeIn() != 0 ) {
        // if not in los, add position at time in (counter clock-wise)
        vin.push_back(po.linear(vop,los.getTimeIn()));
      }
      // in any case, add position ad time out (counter clock-wise)
      vout.push_front(po.linear(vop,los.getTimeOut()));
    }
    /* Second step: Compute conflict contour to the left */
    double left = 0;  // Contour conflict limit to the left relative to current track [0-2pi rad]
    if (0 < right && right < two_pi) {
      /* There is a conflict contour, but not a violation */
      for (left = parameters.getTrackStep(); left < two_pi; left += parameters.getTrackStep()) {
        Velocity vop = vo.mkTrk(current_trk-left);
        LossData los = detector->conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
            0,parameters.getLookaheadTime());
        if ( !los.conflict() ) {
          break;
        }
        vin.push_front(po.linear(vop,los.getTimeIn()));
        vout.push_back(po.linear(vop,los.getTimeOut()));
      }
    }
    add_blob(blobs,vin,vout);
    // Third Step: Look for other blobs to the right within track threshold
    if (right < parameters.getHorizontalContourThreshold()) {
      for (; right < two_pi-left; right += parameters.getTrackStep()) {
        Velocity vop = vo.mkTrk(current_trk+right);
        LossData los = detector->conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
            0,parameters.getLookaheadTime());
        if (los.conflict()) {
          vin.push_back(po.linear(vop,los.getTimeIn()));
          vout.push_front(po.linear(vop,los.getTimeOut()));
        } else {
          add_blob(blobs,vin,vout);
          if (right >= parameters.getHorizontalContourThreshold()) {
            break;
          }
        }
      }
      add_blob(blobs,vin,vout);
    }
    // Fourth Step: Look for other blobs to the left within track threshold
    if (left < parameters.getHorizontalContourThreshold()) {
      for (; left < two_pi-right; left += parameters.getTrackStep()) {
        Velocity vop = vo.mkTrk(current_trk-left);
        LossData los = detector->conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
            0,parameters.getLookaheadTime());
        if (los.conflict()) {
          vin.push_front(po.linear(vop,los.getTimeIn()));
          vout.push_back(po.linear(vop,los.getTimeOut()));
        } else {
          add_blob(blobs,vin,vout);
          if (left >= parameters.getHorizontalContourThreshold()) {
            break;
          }
        }
      }
      add_blob(blobs,vin,vout);
    }
  } else {
    error.addError("trackContour: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
}

/**
 * Computes horizontal contours contributed by aircraft at index ac_idx, for
 * conflict alert level. A contour is a non-empty list of points in counter-clockwise
 * direction representing a polygon.
 * @param blobs list of track contours returned by reference.
 * @param ac_idx is the index of the aircraft used to compute the contours.
 */
void Daidalus::horizontalContours(std::vector< std::vector<Position> >& blobs, int ac_idx) {
  horizontalContours(blobs,ac_idx,0);
}

std::string Daidalus::aircraftListToPVS(int prec) const {
  return ownship_.listToPVSAircraftList(traffic_,prec);
}

std::string Daidalus::outputStringAircraftStates() const {
  std::string ualt = parameters.getUnits("alt_step");
  std::string ugs = parameters.getUnits("gs_step");
  std::string uvs = parameters.getUnits("vs_step");
  std::string uxy = "m";
  if (Units::isCompatible(ugs,"knot")) {
    uxy = "nmi";
  } else if (Units::isCompatible(ugs,"fpm")) {
    uxy = "ft";
  } else if (Units::isCompatible(ugs,"kph")) {
    uxy = "km";
  }
  return ownship_.formattedTraffic(traffic_,uxy,ualt,ugs,uvs);
}

std::string Daidalus::toString() const {
  std::string s = "Daidalus Object\n";
  s += parameters.toString();
  if (ownship_.isValid()) {
    s += "###\nAircraft States:\n"+outputStringAircraftStates();
  }
  return s;
}

std::string Daidalus::release() {
  return "DAIDALUS++ V-"+KinematicBandsParameters::VERSION+
      "-FormalATM-"+Constants::version+" (March-18-2017)";
}

}
