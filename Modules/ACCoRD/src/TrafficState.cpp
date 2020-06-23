/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Position.h"
#include "Velocity.h"
#include "Projection.h"
#include "Constants.h"
#include "format.h"
#include "string_util.h"
#include "TrafficState.h"

#include <string>


namespace larcfm {

TrafficState::TrafficState() :
                                id_("_NoAc_"),
                                pos_(Position::INVALID()),
                                gvel_(Velocity::INVALIDV()),
                                avel_(Velocity::INVALIDV()),
                                eprj_(Projection::createProjection(Position::ZERO_LL())),
                                alerter_(0),
                                sum_(SUMData::EMPTY()),
                                posxyz_(Position::INVALID()),
                                sxyz_(Vect3::INVALID()),
                                velxyz_(Velocity::INVALIDV()) {}

const TrafficState& TrafficState::INVALID() {
  static TrafficState tmp;
  return tmp;
}

TrafficState::TrafficState(const std::string& id, const Position& pos, const Velocity& vel) :
                                    id_(id),
                                    pos_(pos),
                                    gvel_(vel),
                                    avel_(vel),
                                    eprj_(Projection::createProjection(Position::ZERO_LL())),
                                    alerter_(1),
                                    posxyz_(pos),
                                    sxyz_(pos.vect3()),
                                    velxyz_(vel) {}

TrafficState::TrafficState(const std::string& id, const Position& pos, const Velocity& vel, EuclideanProjection eprj, int alerter) :
                                    id_(id),
                                    pos_(pos),
                                    gvel_(vel),
                                    avel_(vel),
                                    eprj_(eprj),
                                    alerter_(alerter) {
  if (pos.isLatLon()) {
    sxyz_ = eprj.project(pos);
    Velocity v = eprj.projectVelocity(pos,vel);
    posxyz_ = Position(sxyz_);
    velxyz_ = Velocity::make(v);
  } else {
    posxyz_ = pos;
    sxyz_ = pos.vect3();
    velxyz_ = vel;
  }
}

// Set air velocity to new_avel
void TrafficState::setAirVelocity(const Velocity& new_avel) {
  Velocity wind = windVector();
  avel_ = new_avel;
  gvel_ = Velocity(new_avel.Add(wind));
  applyEuclideanProjection();
}

// Set position to new_pos and apply Euclidean projection. This methods doesn't change ownship, i.e.,
// the resulting aircraft is considered as another intruder.
void TrafficState::setPosition(const Position& new_pos) {
  pos_ = new_pos;
  applyEuclideanProjection();
}

/**
 * Apply Euclidean projection. Requires aircraft's position in lat/lon
 */
void TrafficState::applyEuclideanProjection() {
  if (pos_.isLatLon()) {
    sxyz_ = eprj_.project(pos_);
    Velocity v = eprj_.projectVelocity(pos_, avel_);
    posxyz_ = Position(sxyz_);
    velxyz_ = Velocity::make(v);
  } else {
    posxyz_ = pos_;
    sxyz_ = pos_.vect3();
    velxyz_ = avel_;
  }
}

/**
 * Set aircraft as ownship
 */
void TrafficState::setAsOwnship() {
  if (isLatLon()) {
    eprj_ = Projection::createProjection(pos_.lla().zeroAlt());
    applyEuclideanProjection();
  }
}

TrafficState TrafficState::makeOwnship(const std::string& id, const Position& pos, const Velocity& vel) {
  TrafficState ac = TrafficState(id,pos,vel);
  ac.setAsOwnship();
  return ac;
}

void TrafficState::setAsIntruderOf(const TrafficState& ownship) {
  if (isLatLon() && ownship.isLatLon()) {
    eprj_ = ownship.getEuclideanProjection();
    applyEuclideanProjection();
  }
}

TrafficState TrafficState::makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const {
  if (pos_.isLatLon() != pos.isLatLon()) {
    return INVALID();
  }
  return TrafficState(id, pos, vel, eprj_, 1);
}

void TrafficState::setAlerterIndex(int alerter) {
  alerter_ = std::max(0, alerter);
}

int TrafficState::getAlerterIndex() const {
  return alerter_;
}

void TrafficState::applyWindVector(const Velocity& wind_vector) {
  avel_ = Velocity(gvel_.Sub(wind_vector));
  if (isLatLon()) {
    applyEuclideanProjection();
  } else {
    velxyz_ = avel_;
  }
}

Velocity TrafficState::windVector() const {
  return Velocity(gvel_.Sub(avel_));
}

const EuclideanProjection& TrafficState::getEuclideanProjection() const {
  return eprj_;
}

const Vect3& TrafficState::get_s() const {
  return sxyz_;
}

const Velocity& TrafficState::get_v() const {
  return velxyz_;
}

Vect3 TrafficState::pos_to_s(const Position& p) const {
  if (p.isLatLon()) {
    if (!pos_.isLatLon()) {
      return Vect3::INVALID();
    }
    return eprj_.project(p);
  }
  return p.vect3();
}

Velocity TrafficState::vel_to_v(const Position& p, const Velocity& v) const {
  if (p.isLatLon()) {
    if (!pos_.isLatLon()) {
      return Velocity::INVALIDV();
    }
    return eprj_.projectVelocity(p,v);
  }
  return v;
}

Velocity TrafficState::inverseVelocity(const Velocity& v) const {
  return eprj_.inverseVelocity(get_s(),v,true);
}

TrafficState TrafficState::linearProjection(double offset) const {
  Position new_pos = pos_.linear(avel_,offset);
  TrafficState ac = *this;
  ac.setPosition(new_pos);
  return ac;
}

/**
 * Index of aircraft id in traffic list. If aircraft is not in the list, returns -1
 * @param traffic
 * @param id
 */
int TrafficState::findAircraftIndex(const std::vector<TrafficState>& traffic, const std::string& id) {
  for (nat i=0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    if (equals(id,ac.getId())) {
      return i;
    }
  }
  return -1;
}

std::string TrafficState::listToString(const std::vector<std::string>& traffic) {
  std::string s = "{";
  bool comma = false;
  for (nat i=0; i < traffic.size(); ++i) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += traffic[i];
  }
  return s+"}";
}

std::string TrafficState::formattedHeader(const std::string& utrk, const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const {
  std::string s1="NAME";
  std::string s2="[none]";
  if (pos_.isLatLon()) {
    s1 += " lat lon alt trk gs vs";
    s2 += " [deg] [deg] ["+ualt+"] ["+utrk+"] ["+ugs+"] ["+uvs+"]";
  } else {
    s1 += " sx sy sz trk gs vs";
    s2 += " ["+uxy+"] ["+uxy+"] ["+ualt+"] ["+utrk+"] ["+ugs+"] ["+uvs+"]";
  }
  s1 += " time alerter";
  s2 += " [s] [none]";
  s1 += " s_EW_std s_NS_std s_EN_std sz_std v_EW_std v_NS_std v_EN_std vz_std";
  s2 += " ["+uxy+"] ["+uxy+"] ["+uxy+"] ["+ualt+"] ["+ugs+"] ["+ugs+"] ["+ugs+"] ["+uvs+"]";
  return s1+"\n"+s2+"\n";
}

std::string TrafficState::formattedTrafficState(const std::string& utrk, const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs, double time) const {
  std::string s= getId();
  if (pos_.isLatLon()) {
    s += ", "+pos_.lla().toString("deg","deg",ualt,Constants::get_output_precision());
  } else {
    s += ", "+fsStrNP(pos_.vect3(), Constants::get_output_precision(),uxy,uxy,ualt);
  }
  s += ", "+avel_.toStringNP(utrk,ugs,uvs,Constants::get_output_precision())+", "+
      FmPrecision(time,Constants::get_output_precision());
  s += ", "+Fmi(alerter_);
  s += ", "+FmPrecision(sum_.get_s_EW_std(uxy));
  s += ", "+FmPrecision(sum_.get_s_NS_std(uxy));
  s += ", "+FmPrecision(sum_.get_s_EN_std(uxy));
  s += ", "+FmPrecision(sum_.get_sz_std(ualt));
  s += ", "+FmPrecision(sum_.get_v_EW_std(ugs));
  s += ", "+FmPrecision(sum_.get_v_NS_std(ugs));
  s += ", "+FmPrecision(sum_.get_v_EN_std(ugs));
  s += ", "+FmPrecision(sum_.get_vz_std(uvs));
  return s+"\n";
}

std::string TrafficState::formattedTrafficList(const std::vector<TrafficState>& traffic,
    const std::string& utrk, const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs, double time) {
  std::string s = "";
  for (nat i=0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    s += ac.formattedTrafficState(utrk,uxy, ualt, ugs, uvs, time);
  }
  return s;
}

std::string TrafficState::formattedTraffic(const std::vector<TrafficState>& traffic,
    const std::string& utrk, const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs, double time) const {
  std::string s = "";
  s += formattedHeader(utrk,uxy,ualt,ugs,uvs);
  s += formattedTrafficState(utrk,uxy,ualt,ugs,uvs,time);
  s += formattedTrafficList(traffic,utrk,uxy,ualt,ugs,uvs,time);
  return s;
}

std::string TrafficState::toPVS() const {
  return "(# id:= \"" + id_ + "\", s:= "+get_s().toPVS()+
      ", v:= "+get_v().toPVS()+", alerter:= "+Fmi(alerter_)+
      ", unc := (# s_EW_std:= "+FmPrecision(sum_.get_s_EW_std())+
      ", s_NS_std:= "+FmPrecision(sum_.get_s_NS_std())+
      ", s_EN_std:= "+FmPrecision(sum_.get_s_EN_std())+
      ", sz_std:= "+FmPrecision(sum_.get_sz_std())+
      ", v_EW_std:= "+FmPrecision(sum_.get_v_EW_std())+
      ", v_NS_std:= "+FmPrecision(sum_.get_v_NS_std())+
      ", v_EN_std:= "+FmPrecision(sum_.get_v_EN_std())+
      ", vz_std:= "+FmPrecision(sum_.get_vz_std())+
      " #) #)";
}

std::string TrafficState::listToPVSAircraftList(const std::vector<TrafficState>& traffic) const {
  std::string s="";
  s += "(: ";
  s += toPVS();
  for (nat i = 0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    s += ", ";
    s += ac.toPVS();
  }
  return s+" :)";
}

std::string TrafficState::listToPVSStringList(const std::vector<std::string>& traffic) {
  if (traffic.empty()) {
    return "null[string]";
  } else {
    std::string s = "(:";
    bool comma = false;
    for (nat i = 0; i < traffic.size(); ++i) {
      std::string ac = traffic[i];
      if (comma) {
        s += ", ";
      } else {
        s += " ";
        comma = true;
      }
      s += "\"" + ac + "\"";
    }
    return s+" :)";
  }
}

bool TrafficState::isValid() const {
  return !pos_.isInvalid() && !gvel_.isInvalid();
}

bool TrafficState::isLatLon() const {
  return pos_.isLatLon();
}

const std::string& TrafficState::getId() const {
  return id_;
}

const Position& TrafficState::getPosition() const {
  return pos_;
}

const Velocity& TrafficState::getGroundVelocity() const {
  return gvel_;
}

/**
 * @return Aircraft's air velocity
 */
const Velocity& TrafficState::getAirVelocity() const {
  return avel_;
}

/**
 * @return Aircraft's velocity (can be ground or air depending on whether a wind vector was applied or not)
 */
const Velocity& TrafficState::getVelocity() const {
  return avel_;
}

const Position& TrafficState::positionXYZ() const {
  return posxyz_;
}

const Velocity& TrafficState::velocityXYZ() const {
  return velxyz_;
}

double TrafficState::horizontalDirection() const {
  return avel_.compassAngle();
}

double TrafficState::horizontalDirection(const std::string& utrk) const {
  return avel_.compassAngle(utrk);
}

double TrafficState::horizontalSpeed() const {
  return avel_.gs();
}

double TrafficState::horizontalSpeed(const std::string& ugs) const {
  return avel_.groundSpeed(ugs);
}

double TrafficState::verticalSpeed() const {
  return avel_.vs();
}

double TrafficState::verticalSpeed(const std::string& uvs) const {
  return avel_.verticalSpeed(uvs);
}

double TrafficState::altitude() const{
  return pos_.alt();
}

double TrafficState::altitude(const std::string& ualt) const {
  return Units::to(ualt,pos_.alt());
}

const SUMData& TrafficState::sum() const {
  return sum_;
}

/**
 * s_EW_std: East/West position standard deviation in internal units
 * s_NS_std: North/South position standard deviation in internal units
 * s_EN_std: East/North position standard deviation in internal units
 */
void TrafficState::setHorizontalPositionUncertainty(double s_EW_std, double s_NS_std, double s_EN_std) {
  sum_.setHorizontalPositionUncertainty(s_EW_std, s_NS_std, s_EN_std);
}

/**
 * sz_std : Vertical position standard deviation in internal units
 */
void TrafficState::setVerticalPositionUncertainty(double sz_std) {
  sum_.setVerticalPositionUncertainty(sz_std);
}

/**
 * v_EW_std: East/West velocity standard deviation in internal units
 * v_NS_std: North/South velocity standard deviation in internal units
 * v_EN_std: East/North velocity standard deviation in internal units
 */
void TrafficState::setHorizontalVelocityUncertainty(double v_EW_std, double v_NS_std,  double v_EN_std) {
  sum_.setHorizontalVelocityUncertainty(v_EW_std, v_NS_std, v_EN_std);
}

/**
 * vz_std : Vertical velocity standard deviation in internal units
 */
void TrafficState::setVerticalSpeedUncertainty(double vz_std) {
  sum_.setVerticalSpeedUncertainty(vz_std);
}

/**
 * Set all uncertainties to 0
 */
void TrafficState::resetUncertainty() {
  sum_.resetUncertainty();
}

bool TrafficState::sameId(const TrafficState& ac) const {
  return isValid() && ac.isValid() && equals(id_, ac.id_);
}

std::string TrafficState::toString() const {
  return "("+id_+", "+pos_.toString()+", "+avel_.toString()+")";
}

}
