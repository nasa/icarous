/*
 * Copyright (c) 2015-2017 United States Government as represented by
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

void DaidalusFileWalker::init() {
  sr_.setWindowSize(1);
  index_ = 0;
  times_ = sr_.sequenceKeys();
  if (times_.size() > 0)
    sr_.setActive(times_[0]);
}

void DaidalusFileWalker::resetInputFile(const std::string& filename) {
  sr_ = SequenceReader(filename);
  init();
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
    return NAN;
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

void DaidalusFileWalker::readState(Daidalus& daa) {
  daa.reset();
  for (int ac = 0; ac < sr_.size();++ac) {
    std::string ida = sr_.getName(ac);
    Position sa = sr_.getPosition(ac);
    Velocity va = sr_. getVelocity(ac);
    if (ac==0) {
      daa.setOwnshipState(ida,sa,va,getTime());
    } else {
      daa.addTrafficState(ida,sa,va);
    }
  }
  goNext();
}

}
