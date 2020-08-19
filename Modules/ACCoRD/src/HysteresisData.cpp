/*
 * Implementation of hysteresis logic that includes MofN and persistence.
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "HysteresisData.h"

#include "Util.h"
#include "format.h"

#include <vector>

namespace larcfm {

/*
 * Creates an empty object
 */
HysteresisData::HysteresisData() {
  hysteresis_time_ = 0;
  persistence_time_ = 0;
  init_time_ = NaN;
  last_time_ = NaN;
  last_value_ = -1;
  outdated_ = true;
}

/*
 * Creates an object for given values of hysteresis, persistence time, and M of N parameters
 */
HysteresisData::HysteresisData(double hysteresis_time, double persistence_time, int m, int n) {
  mofn_.setMofN(m,n);
  hysteresis_time_ = hysteresis_time;
  persistence_time_ = persistence_time;
  init_time_ = NaN;
  last_time_ = NaN;
  last_value_ = -1;
  outdated_ = true;
}

void HysteresisData::outdateIfCurrentTime(double current_time) {
  if (!ISNAN(last_time_) && last_time_ >= current_time) {
    outdated_ = true;
  }
}

bool HysteresisData::isUpdatedAtCurrentTime(double current_time) const {
  return last_time_ == current_time && !outdated_;
}

double HysteresisData::getInitTime() const {
  return init_time_;
}


double HysteresisData::getLastTime() const {
  return last_time_;
}

int HysteresisData::getLastValue() const {
  return last_value_;
}

/*
 * Reset object with given M of N value
 */
void HysteresisData::reset(int val) {
  mofn_.reset(val);
  init_time_ = NaN;
  last_time_ = NaN;
  last_value_ = -1;
  outdated_   = true;
}

/*
 * In addition of m_of_n, this also applies hysteresis and persistence
 */
int HysteresisData::applyHysteresisLogic(int current_value, double current_time) {
  if (ISNAN(last_time_) ||
      current_time <= last_time_ || current_time-last_time_ > hysteresis_time_) {
    // Reset hysteresis if current_time is in the past of later than deadline
    reset(current_value);
  }
  // Add current value into M of N logic even if negative
  int value_mofn = mofn_.m_of_n(current_value);
  if (value_mofn < 0) {
    // Return invalid output since input is invalid (negative)
    last_value_ = current_value;
    init_time_ = NaN;
  } else if (!ISNAN(init_time_) && last_value_ > 0 && value_mofn < last_value_ &&
      current_time >= init_time_ &&
      current_time-init_time_ < persistence_time_) {
    // Do nothing. Keep the previous value (persistence logic prevails)
  } else {
    if (value_mofn > 0 && value_mofn > last_value_) {
      init_time_ = current_time;
    }
    // Return a valid output (i.e., >= 0), since input is valid
    last_value_ = Util::max(0,value_mofn);
  }
  last_time_ = current_time;
  outdated_ = false;
  return last_value_;
}

std::string HysteresisData::toString() const {
  std::string s = "<";
  s += "hysteresis_time: "+FmPrecision(hysteresis_time_);
  s += ", persistence_time: "+FmPrecision(persistence_time_);
  s += ", init_time: "+FmPrecision(init_time_);
  s += ", last_time: "+FmPrecision(last_time_);
  s += ", last_value: "+Fmi(last_value_);
  s += ", outdated: "+Fmb(outdated_);
  s += ", "+mofn_.toString();
  s += ">";
  return s;
}


} /* namespace larcfm */
