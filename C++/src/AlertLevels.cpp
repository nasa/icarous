/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "AlertLevels.h"
#include "ParameterData.h"
#include "Detection3DParameterReader.h"
#include "Detection3DParameterWriter.h"
#include "Detection3D.h"
#include "BandsRegion.h"
#include "WCV_TAUMOD.h"
#include "Constants.h"

#include <vector>
#include <map>

namespace larcfm {

AlertLevels::AlertLevels() {
  alertor_ = std::vector<AlertThresholds>();
  conflict_level_ = 0;
}

AlertLevels::AlertLevels(const AlertLevels& alertor) {
  alertor_ = std::vector<AlertThresholds>();
  copy(alertor);
}

AlertLevels::~AlertLevels() {
  alertor_.clear();
}

/**
 * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
 * Maneuver guidance logic produces multilevel bands:
 * MID: Corrective
 * NEAR: Warning
 * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)
 *
 */
AlertLevels AlertLevels::WC_SC_228_Thresholds() {
  WCVTable preventive = WCVTable();
  preventive.setDTHR(0.66,"nmi");
  preventive.setZTHR(700,"ft");
  preventive.setTTHR(35);
  preventive.setTCOA(0);

  WCVTable corrective = WCVTable();
  corrective.setDTHR(0.66,"nmi");
  corrective.setZTHR(450,"ft");
  corrective.setTTHR(35);
  corrective.setTCOA(0);

  WCVTable warning = WCVTable();
  warning.setDTHR(0.66,"nmi");
  warning.setZTHR(450,"ft");
  warning.setTTHR(35);
  warning.setTCOA(0);

  AlertLevels alertor = AlertLevels();
  alertor.setConflictAlertLevel(2);
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(preventive),55,75,BandsRegion::NONE));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(corrective),55,75,BandsRegion::MID));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(warning),25,55,BandsRegion::NEAR));

  return alertor;
}

/**
 * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
 * Maneuver guidance logic produces single level bands:
 * NEAR: Corrective
 * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)
 *
 */
AlertLevels AlertLevels::SingleBands_WC_SC_228_Thresholds() {
  WCVTable preventive = WCVTable();
  preventive.setDTHR(0.66,"nmi");
  preventive.setZTHR(700,"ft");
  preventive.setTTHR(35);
  preventive.setTCOA(0);

  WCVTable corrective = WCVTable();
  corrective.setDTHR(0.66,"nmi");
  corrective.setZTHR(450,"ft");
  corrective.setTTHR(35);
  corrective.setTCOA(0);

  WCVTable warning = WCVTable();
  warning.setDTHR(0.66,"nmi");
  warning.setZTHR(450,"ft");
  warning.setTTHR(35);
  warning.setTCOA(0);

  AlertLevels alertor = AlertLevels();
  alertor.setConflictAlertLevel(2);
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(preventive),55,75,BandsRegion::NONE));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(corrective),55,75,BandsRegion::MID));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(warning),25,55,BandsRegion::NONE));

  return alertor;
}

/**
 * @return alerting thresholds (buffered) as defined by SC-228 MOPS.
 * Maneuver guidance logic produces multilevel bands:
 * MID: Corrective
 * NEAR: Warning
 * Well-clear volume (buffered) is defined by alerting level 2 (conflict_alerting_level)
 *
 */
AlertLevels AlertLevels::Buffered_WC_SC_228_Thresholds() {
  WCVTable preventive = WCVTable();
  preventive.setDTHR(1.0,"nmi");
  preventive.setZTHR(750,"ft");
  preventive.setTTHR(35);
  preventive.setTCOA(20);

  WCVTable corrective = WCVTable();
  corrective.setDTHR(1.0,"nmi");
  corrective.setZTHR(450,"ft");
  corrective.setTTHR(35);
  corrective.setTCOA(20);

  WCVTable warning = WCVTable();
  warning.setDTHR(1.0,"nmi");
  warning.setZTHR(450,"ft");
  warning.setTTHR(35);
  warning.setTCOA(20);

  AlertLevels alertor = AlertLevels();
  alertor.setConflictAlertLevel(2);
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(preventive),60,75,BandsRegion::NONE));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(corrective),60,75,BandsRegion::MID));
  alertor.addLevel(AlertThresholds(new WCV_TAUMOD(warning),30,55,BandsRegion::NEAR));

  return alertor;
}

/**
 * @return alerting thresholds for single bands given by detector,
 * alerting time, and lookahead time.
 */
AlertLevels AlertLevels::SingleBands(const Detection3D* detector,
    double alerting_time, double lookahead_time) {
  AlertLevels alertor = AlertLevels();
  alertor.setConflictAlertLevel(1);
  alertor.addLevel(AlertThresholds(detector,alerting_time,lookahead_time,BandsRegion::NEAR));
  return alertor;
}

/**
 * Clears alert levels
 **/
void AlertLevels::clear() {
  alertor_.clear();
  conflict_level_ = 0;
}

/**
 * @return numerical type of most severe alert level.
 */
int AlertLevels::mostSevereAlertLevel() const {
  return alertor_.size();
}

/**
 * @return first guidance level whose region is different from NONE
 */
int AlertLevels::firstGuidanceLevel() const {
  for (nat i=0; i < alertor_.size(); ++i) {
    if (BandsRegion::isConflictBand(alertor_[i].getRegion())) {
      return i+1;
    }
  }
  return 0;
}

/**
 * @return last guidance level whose region is different from NONE
 */
int AlertLevels::lastGuidanceLevel() const {
  for (nat i=alertor_.size(); i > 0; --i) {
    if (BandsRegion::isConflictBand(alertor_[i-1].getRegion())) {
      return i;
    }
  }
  return 0;
}

/**
 * @return conflict alert level. When the numerical is value is 0, first guidance level
 */
int AlertLevels::conflictAlertLevel() const {
  if (1 <= conflict_level_ && conflict_level_ <= mostSevereAlertLevel()) {
    return conflict_level_;
  } else {
    return firstGuidanceLevel();
  }
}

/**
 * @return detector for given alert level
 */
Detection3D* AlertLevels::detectorRef(int alert_level) const {
  if (alert_level == 0) {
    alert_level = conflictAlertLevel();
  }
  if (alert_level > 0) {
    return alertor_[alert_level-1].getDetectorRef();
  } else {
    return NULL;
  }
}

/**
 * @return detector for conflict alert level
 */
Detection3D* AlertLevels::conflictDetectorRef() const {
  return detectorRef(0);
}

/**
 * Set conflict alert level
 */
void AlertLevels::setConflictAlertLevel(int alert_level) {
  if (0 <= alert_level) {
    conflict_level_ = alert_level;
  }
}

/**
 * Set the threshold values of a given alert level.
 */
void AlertLevels::setLevel(int level, const AlertThresholds& thresholds) {
  if (1 <= level && level <= (int) alertor_.size()) {
    alertor_[level-1] = AlertThresholds(thresholds);
  }
}

/**
 * Add an alert level and returns its numerical type, which is a positive number.
 */
int AlertLevels::addLevel(const AlertThresholds& thresholds) {
  alertor_.push_back(AlertThresholds(thresholds));
  return alertor_.size();
}

/**
 * @return threshold values of a given alert level
 */
AlertThresholds const & AlertLevels::getLevel(int level) const {
  if (1 <= level && level <= (int) alertor_.size()) {
    return alertor_[level-1];
  } else {
    return AlertThresholds::INVALID;
  }
}

/**
 * Set alertor to the same values as the given parameter
 */
void AlertLevels::copy(const AlertLevels& alertor) {
  alertor_.clear();
  conflict_level_ = alertor.conflict_level_;
  for (int i=1; i <= alertor.mostSevereAlertLevel(); ++i) {
    alertor_.push_back(AlertThresholds(alertor.getLevel(i)));
  }
}

ParameterData AlertLevels::getParameters() const {
  Detection3DParameterReader::registerDefaults();
  ParameterData p;
  updateParameterData(p);
  return p;
}

void AlertLevels::updateParameterData(ParameterData& p) const {
  Detection3DParameterReader::registerDefaults();
  p.set("conflict_level",Fmi(conflict_level_));
  // get list of detectors that are in the alerts
  std::vector<Detection3D*> dlist;
  std::vector<std::string> idlist;
  for (nat i = 0; i < alertor_.size(); i++) {
    Detection3D* det = alertor_[i].getDetectorRef();
    det->setIdentifier("det_"+Fmi(i+1));
    dlist.push_back(det);
    idlist.push_back(det->getIdentifier()+"_id");

  }
  // create the base parameterdata object storing the detector information
  // this also ensures they each have a unique identifier
  ParameterData pdmain = Detection3DParameterWriter::writeCoreDetection(dlist, NULL, NULL);
  // remove detection3d identifier keys from pdmain
  for (int i = 0; i < (int) idlist.size(); i++) {
    pdmain.remove(idlist[i]);
  }
  int precision = Constants::get_output_precision();
  // add parameters for each alerter, ensuring they have an ordered set of identifiers
  for (int i = 1; i <= mostSevereAlertLevel(); i++) {
    ParameterData pd;
    pd.set("detector", getLevel(i).getDetectorRef()->getIdentifier());
    pd.setInternal("alerting_time", getLevel(i).getAlertingTime(), "s", precision);
    pd.setInternal("early_alerting_time", getLevel(i).getEarlyAlertingTime(), "s", precision);
    pd.set("region", BandsRegion::to_string(getLevel(i).getRegion()));
    pd.setInternal("spread_trk", getLevel(i).getTrackSpread(), "deg", precision);
    pd.setInternal("spread_gs", getLevel(i).getGroundSpeedSpread(), "knot", precision);
    pd.setInternal("spread_vs", getLevel(i).getVerticalSpeedSpread(), "fpm", precision);
    pd.setInternal("spread_alt", getLevel(i).getAltitudeSpread(), "ft", precision);
    //make sure each instance has a unique, ordered name
    std::string prefix = "alert_"+Fmi(i)+"_";
    pdmain.copy(pd.copyWithPrefix(prefix), true);
  }
  p.copy(pdmain, true);}

void AlertLevels::setParameters(const ParameterData& p) {
  Detection3DParameterReader::registerDefaults();
  if (p.contains("conflict_level")) conflict_level_ = p.getInt("conflict_level");
  // read in all detector information
  std::vector<Detection3D*> dlist = Detection3DParameterReader::readCoreDetection(p).first;
  // put in map for easy lookup
  std::map<std::string,Detection3D*> dmap = std::map<std::string,Detection3D*>();
  for (int i = 0; i < (int) dlist.size(); i++) {
    dmap[dlist[i]->getIdentifier()] = dlist[i];
  }
  // extract parameters for each alertlevel:
  int counter = 1;
  std::string prefix = "alert_"+Fmi(counter)+"_";
  ParameterData pdsub = p.extractPrefix(prefix);
  if (pdsub.size() > 0) {
    alertor_.clear();
  }
  while (pdsub.size() > 0) {
    // build the alertlevel
    Detection3D* det = dmap[pdsub.getString("detector")];
    double alertingTime = pdsub.getValue("alerting_time");
    double earlyAlertingTime = pdsub.getValue("early_alerting_time");
    BandsRegion::Region br = BandsRegion::valueOf(pdsub.getString("region"));
    AlertThresholds al = AlertThresholds(det,alertingTime,earlyAlertingTime,br);
    delete det; // al has a new copy, clean up the old one
    al.setTrackSpread(pdsub.getValue("spread_trk"));
    al.setGroundSpeedSpread(pdsub.getValue("spread_gs"));
    al.setVerticalSpeedSpread(pdsub.getValue("spread_vs"));
    al.setAltitudeSpread(pdsub.getValue("spread_alt"));
    // modify or add the alertlevel (this cannot remove levels)
    if (counter <= (int) alertor_.size()) {
      setLevel(counter,al);
    } else {
      addLevel(al);
    }
    // next set
    counter++;
    prefix = "alert_"+Fmi(counter)+"_";
    pdsub = p.extractPrefix(prefix);
  }
}

std::string AlertLevels::toString() const {
  std::string s = "Conflict Level: "+Fmi(conflict_level_)+" ("+Fmi(conflictAlertLevel())+")\n";
  for (nat i=0; i < alertor_.size(); ++i) {
    s += "Level "+Fmi((i+1))+": "+alertor_[i].toString()+"\n";
  }
  return s;
}

std::string AlertLevels::toPVS(int prec) const {
  std::string s = "("+Fmi(conflictAlertLevel())+", (: ";
  bool first = true;
  for (nat i=0; i < alertor_.size(); ++i) {
    if (first) {
      first = false;
    } else {
      s += ",";
    }
    s += alertor_[i].toPVS(prec);
  }
  return s+" :))";
}

}
