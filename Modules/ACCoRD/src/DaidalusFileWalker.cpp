/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * DaidalusFileWalker.cpp
 *
 */

#include "DaidalusFileWalker.h"

namespace larcfm {

DaidalusFileWalker::DaidalusFileWalker(const std::string& filename) {
  sr_ = SequenceReader(filename);
  init();
}

void DaidalusFileWalker::resetInputFile(const std::string& filename) {
  sr_ = SequenceReader(filename);
  init();
}

void DaidalusFileWalker::init() {
  sr_.setWindowSize(1);
  index_ = 0;
  times_ = sr_.sequenceKeys();
  if (times_.size() > 0)
    sr_.setActive(times_[0]);
}

double DaidalusFileWalker::firstTime() const {
  if (!times_.empty()) {
    return times_[0];
  }
  return PINFINITY;
}

double DaidalusFileWalker::lastTime() const {
  if (!times_.empty()) {
    return times_[times_.size()-1];
  }
  return NINFINITY;
}

int DaidalusFileWalker::getIndex() const {
  return index_;
}

double DaidalusFileWalker::getTime() const {
  if (0 <= index_ && (unsigned int)index_ < times_.size()) {
    return times_[index_];
  } else {
    return NaN;
  }
}

bool DaidalusFileWalker::atBeginning() const {
  return index_ == 0;
}

bool DaidalusFileWalker::atEnd() const {
  return index_ >=0 && (unsigned int)index_ == times_.size();
}

bool DaidalusFileWalker::goToTime(double t) {
  return goToTimeStep(indexOfTime(t));
}

bool DaidalusFileWalker::goToTimeStep(int i) {
  if (0 <= i && (unsigned int)i < times_.size()) {
    index_ = i;
    sr_.setActive(times_[index_]);
    return true;
  }
  return false;
}

void DaidalusFileWalker::goToBeginning() {
  goToTimeStep(0);
}

void DaidalusFileWalker::goToEnd() {
  goToTimeStep(times_.size());
}

void DaidalusFileWalker::goNext() {
  bool ok = goToTimeStep(index_+1);
  if (!ok) {
    index_ = times_.size();
  }
}

void DaidalusFileWalker::goPrev() {
  if (!atBeginning()) {
    goToTimeStep(index_-1);
  }
}

int DaidalusFileWalker::indexOfTime(double t) const {
  int i = -1;
  if (t >= firstTime() && t <= lastTime()) {
    i = 0;
    for (; (unsigned int)i < times_.size()-1; ++i) {
      if (t >= times_[i] && t < times_[i+1]) {
        break;
      }
    }
  }
  return i;
}

ParameterData DaidalusFileWalker::extraColumnsToParameters(const SequenceReader& sr, double time, const std::string& ac_name) {
  ParameterData pd;
  std::vector<std::string> columns = sr.getExtraColumnList();
  std::vector<std::string>::const_iterator col_ptr;
  for (col_ptr = columns.begin(); col_ptr != columns.end(); ++col_ptr) {
    if (sr.hasExtraColumnData(time, ac_name,*col_ptr)) {
      std::string units = sr.getExtraColumnUnits(*col_ptr);
      if (units == "unitless" || units == "unspecified") {
        pd.set(*col_ptr, sr.getExtraColumnString(time,ac_name,*col_ptr));
      } else {
        pd.setInternal(*col_ptr, sr.getExtraColumnValue(time, ac_name,*col_ptr), units);
      }
    }
  }
  return pd;
}


void DaidalusFileWalker::readExtraColumns(Daidalus& daa, const SequenceReader& sr, int ac_idx) {
  ParameterData pcol = extraColumnsToParameters(sr,daa.getCurrentTime(),daa.getAircraftStateAt(ac_idx).getId());
  if (pcol.size() > 0) {
    daa.setParameterData(pcol);
    if (pcol.contains("alerter")) {
      daa.setAlerterIndex(ac_idx,pcol.getInt("alerter"));
    }
    double s_EW_std = 0.0;
    if (pcol.contains("s_EW_std")) {
      s_EW_std = pcol.getValue("s_EW_std");
    }
    double s_NS_std = 0.0;
    if (pcol.contains("s_NS_std")) {
      s_NS_std = pcol.getValue("s_NS_std");
    }
    double s_EN_std = 0.0;
    if (pcol.contains("s_EN_std")) {
      s_EN_std = pcol.getValue("s_EN_std");
    }
    daa.setHorizontalPositionUncertainty(ac_idx,s_EW_std,s_NS_std,s_EN_std);
    double sz_std = 0.0;
    if (pcol.contains("sz_std")) {
      sz_std = pcol.getValue("sz_std");
    }
    daa.setVerticalPositionUncertainty(ac_idx,sz_std);
    double v_EW_std = 0.0;
    if (pcol.contains("v_EW_std")) {
      v_EW_std = pcol.getValue("v_EW_std");
    }
    double v_NS_std = 0.0;
    if (pcol.contains("v_NS_std")) {
      v_NS_std = pcol.getValue("v_NS_std");
    }
    double v_EN_std = 0.0;
    if (pcol.contains("v_EN_std")) {
      v_EN_std = pcol.getValue("v_EN_std");
    }
    daa.setHorizontalVelocityUncertainty(ac_idx,v_EW_std,v_NS_std,v_EN_std);
    double vz_std = 0.0;
    if (pcol.contains("vz_std")) {
      vz_std = pcol.getValue("vz_std");
    }
    daa.setVerticalSpeedUncertainty(ac_idx,vz_std);
  }
}

void DaidalusFileWalker::readState(Daidalus& daa) {
  if (p_.size() > 0) {
    daa.setParameterData(p_);
    daa.reset();
  }
  for (int ac = 0; ac < sr_.size();++ac) {
    std::string ida = sr_.getName(ac);
    Position sa = sr_.getPosition(ac);
    Velocity va = sr_. getVelocity(ac);
    if (ac==0) {
      daa.setOwnshipState(ida,sa,va,getTime());
    } else {
      daa.addTrafficState(ida,sa,va);
    }
    readExtraColumns(daa,sr_,ac);
  }
  goNext();
}

// ErrorReporter Interface Methods

bool DaidalusFileWalker::hasError() const {
  return sr_.hasError();
}

bool DaidalusFileWalker::hasMessage() const {
  return sr_.hasMessage();
}

std::string DaidalusFileWalker::getMessage() {
  return sr_.getMessage();
}

std::string DaidalusFileWalker::getMessageNoClear() const {
  return sr_.getMessageNoClear();
}

}
