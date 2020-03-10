/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Alerter.h"
#include "ParameterData.h"
#include "Detection3DParameterReader.h"
#include "Detection3DParameterWriter.h"
#include "Detection3D.h"
#include "BandsRegion.h"
#include "WCV_TAUMOD.h"
#include "WCV_tvar.h"
#include "string_util.h"

#include <vector>
#include <map>

namespace larcfm {

Alerter::Alerter() {
  id_ = "default";
}

bool Alerter::isValid() const {
  return !levels_.empty();
}

Alerter::Alerter(const std::string& id) {
  id_ = equals(id, "") ? "default" : id;
}

const Alerter& Alerter::INVALID() {
  static Alerter a;
  return a;
}

void Alerter::setId(const std::string& id) {
  if (!equals(id,"")) {
    id_ = id;
  }
}

const std::string& Alerter::getId() const {
  return id_;
}

/**
 * @return DO-365 HAZ preventive thresholds, i.e., DTHR=0.66nmi, ZTHR=700ft,
 * TTHR=35s, TCOA=0, alerting time = 55s, early alerting time = 75s,
 * bands region = NONE
 */
const AlertThresholds& Alerter::DO_365_Phase_I_HAZ_preventive() {
  static AlertThresholds preventive(&WCV_TAUMOD::DO_365_Phase_I_preventive(),55,75,BandsRegion::NONE);
  return preventive;
}

/**
 * @return DO-365 HAZ corrective thresholds, i.e., DTHR=0.66nmi, ZTHR=450ft,
 * TTHR=35s, TCOA=0, alerting time = 55s, early alerting time = 75s,
 * bands region = MID
 */
const AlertThresholds& Alerter::DO_365_Phase_I_HAZ_corrective() {
  static AlertThresholds corrective(&WCV_TAUMOD::DO_365_DWC_Phase_I(),55,75,BandsRegion::MID);
  return corrective;
}

/**
 * @return DO-365 HAZ warning thresholds, i.e., DTHR=0.66nmi, ZTHR=450ft,
 * TTHR=35s, TCOA=0, alerting time = 25s, early alerting time = 55s,
 * bands region = NEAR
 */
const AlertThresholds& Alerter::DO_365_Phase_I_HAZ_warning() {
  static AlertThresholds corrective(&WCV_TAUMOD::DO_365_DWC_Phase_I(),25,55,BandsRegion::NEAR);
  return corrective;
}

const Alerter& Alerter::DWC_Phase_I() {
  static Alerter alerter;
  if (!alerter.isValid()) {
    alerter.setId("DWC_Phase_I");
    alerter.addLevel(DO_365_Phase_I_HAZ_preventive());
    alerter.addLevel(DO_365_Phase_I_HAZ_corrective());
    alerter.addLevel(DO_365_Phase_I_HAZ_warning());
  }
  return alerter;
}

/**
 * @return buffered HAZ preventive thresholds, i.e., DTHR=1nmi, ZTHR=750ft,
 * TTHR=35s, TCOA=20s, alerting time = 60s, early alerting time = 75s,
 * bands region = NONE
 */
const AlertThresholds& Alerter::Buffered_Phase_I_HAZ_preventive() {
  static AlertThresholds preventive(&WCV_TAUMOD::Buffered_Phase_I_preventive(),60,75,BandsRegion::NONE);
  return preventive;
}

/**
 * @return buffered HAZ corrective thresholds, i.e., DTHR=1nmi, ZTHR=450ft,
 * TTHR=35s, TCOA=20s, alerting time = 60s, early alerting time = 75s,
 * bands region = MID
 */
const AlertThresholds& Alerter::Buffered_Phase_I_HAZ_corrective() {
  static AlertThresholds corrective(&WCV_TAUMOD::Buffered_DWC_Phase_I(),60,75,BandsRegion::MID);
  return corrective;
}

/**
 * @return buffered HAZ warning thresholds, i.e., DTHR=1nmi, ZTHR=450ft,
 * TTHR=35s, TCOA=20s, alerting time = 30s, early alerting time = 55s,
 * bands region = NEAR
 */
const AlertThresholds& Alerter::Buffered_Phase_I_HAZ_warning() {
  static AlertThresholds corrective(&WCV_TAUMOD::Buffered_DWC_Phase_I(),30,55,BandsRegion::NEAR);
  return corrective;
}

const Alerter& Alerter::Buffered_DWC_Phase_I() {
  static Alerter alerter;
  if (!alerter.isValid()) {
    alerter.addLevel(Buffered_Phase_I_HAZ_preventive());
    alerter.addLevel(Buffered_Phase_I_HAZ_corrective());
    alerter.addLevel(Buffered_Phase_I_HAZ_warning());
  }
  return alerter;
}

/**
 * @return alerting thresholds for single bands given by detector,
 * alerting time, and lookahead time. The single bands region is NEAR
 */
Alerter Alerter::SingleBands(const Detection3D* detector, double alerting_time, double lookahead_time) {
  Alerter alerter("");
  alerter.addLevel(AlertThresholds(detector,alerting_time,lookahead_time,BandsRegion::NEAR));
  return alerter;
}

/**
 * @return alerting thresholds for ACCoRD's CD3D, i.e.,
 * separation is given by cylinder of 5nmi/1000ft. Alerting time is
 * 180s.
 */
const Alerter& Alerter::CD3D_SingleBands() {
  static Alerter alerter = SingleBands(&CDCylinder::CD3DCylinder(),180,180);
  alerter.setId("CD3D");
  return alerter;
}

/**
 * @return alerting thresholds for DAIDALUS single bands WCV_TAUMOD , i.e.,
 * separation is given by cylinder of DTHR=0.66nmi, ZTHR=450ft, TTHR=35s, TCOA=0,
 * alerting time = 55s, early alerting time = 75s.
 */
const Alerter& Alerter::WCV_TAUMOD_SingleBands() {
  static Alerter alerter = SingleBands(&WCV_TAUMOD::DO_365_DWC_Phase_I(),55,75);
  alerter.setId("WCV_TAUMOD");
  return alerter;
}

void Alerter::clear() {
  levels_.clear();
}

int Alerter::mostSevereAlertLevel() const {
  return levels_.size();
}

int Alerter::alertLevelForRegion(BandsRegion::Region region) const {
  for (int i=0; i < (int) levels_.size(); ++i) {
    if (levels_[i].getRegion() == region) {
      return i+1;
    }
  }
  return 0;
}

Detection3D* Alerter::getDetectorPtr(int alert_level) const {
  if (1 <= alert_level && alert_level <= (int) levels_.size()) {
    return levels_[alert_level-1].getCoreDetectionPtr();
  } else {
    return NULL;
  }
}

void Alerter::setLevel(int level, const AlertThresholds& thresholds) {
  if (1 <= level && level <= (int) levels_.size()) {
    levels_[level-1] = AlertThresholds(thresholds);
  }
}


int Alerter::addLevel(const AlertThresholds& thresholds) {
  levels_.push_back(AlertThresholds(thresholds));
  int sz = levels_.size();
  Detection3D* cd = levels_[sz-1].getCoreDetectionPtr();
  cd->setIdentifier("det_"+Fmi(sz));
  return sz;
}

const AlertThresholds& Alerter::getLevel(int alert_level) const {
  if (1 <= alert_level && alert_level <= (int) levels_.size()) {
    return levels_[alert_level-1];
  } else {
    return AlertThresholds::INVALID();
  }
}

ParameterData Alerter::getParameters() const {
  Detection3DParameterReader::registerDefaults();
  ParameterData p;
  updateParameterData(p);
  return p;
}

void Alerter::updateParameterData(ParameterData& p) const {
  // create the base parameterdata object storing the detector information
  // this also ensures they each have a unique identifier
  ParameterData pdmain;
  // add parameters for each alerter, ensuring they have an ordered set of identifiers
  for (int i = 0; i < (int)levels_.size(); i++) {
    const ParameterData& pd = levels_[i].getParameters();
    //make sure each instance has a unique, ordered name
    std::string prefix = "alert_"+Fmi(i+1)+"_";
    pdmain.copy(pd.copyWithPrefix(prefix),true);
    Detection3D* det = levels_[i].getCoreDetectionPtr();
    pdmain.copy(det->getParameters().copyWithPrefix(det->getIdentifier()+"_"),true);
    pdmain.set("load_core_detection_"+det->getIdentifier()+" = "+det->getCanonicalClassName());
    pdmain.remove(det->getIdentifier()+"_id");
  }
  p.copy(pdmain,true);
}

void Alerter::setParameters(const ParameterData& p) {
  Detection3DParameterReader::registerDefaults();
  // read in all detector information
  std::vector<Detection3D*> dlist = Detection3DParameterReader::readCoreDetection(p).first;
  // put in map for easy lookup
  std::map<std::string, Detection3D*> dmap;
  for (int i = 0; i < (int) dlist.size(); i ++) {
    std::string id = dlist[i]->getIdentifier();
    dmap[id] = dlist[i];
  }
  // extract parameters for each alertlevel:
  int counter = 1;
  std::string prefix = "alert_"+Fmi(counter)+"_";
  ParameterData pdsub = p.extractPrefix(prefix);
  if (pdsub.size() > 0) {
    levels_.clear();
  }
  while (pdsub.size() > 0) {
    // build the alertlevel
    AlertThresholds al;
    al.setCoreDetectionPtr(dmap[pdsub.getString("detector")]);
    al.setParameters(pdsub);
    // modify or add the alertlevel (this cannot remove levels)
    if (counter <= (int) levels_.size()) {
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

std::string Alerter::toString() const {
  std::string s = "Alerter: ";
  s += id_+"\n";
  for (int i=0; i < (int) levels_.size(); ++i) {
    s += "Level "+Fmi(i+1)+": "+levels_[i].toString()+"\n";
  }
  return s;
}

std::string Alerter::toPVS() const {
  std::string s = "(: ";
  bool first = true;
  for (int i=0; i < (int) levels_.size(); ++i) {
    if (first) {
      first = false;
    } else {
      s += ",";
    }
    s += levels_[i].toPVS();
  }
  return s+" :)";
}

std::string Alerter::listToPVS(const std::vector<Alerter>& alerters) {
  std::string s = "(: ";
  bool first = true;
  for (int i=0; i < (int) alerters.size(); ++i) {
    if (first) {
      first = false;
    } else {
      s += ",";
    }
    s += alerters[i].toPVS();
  }

  return s+" :)";
}



}
