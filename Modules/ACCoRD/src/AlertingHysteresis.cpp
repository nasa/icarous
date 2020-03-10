/*
 * Implementation of alerting hysteresis logic that includes MofN and persistence.
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "AlertingHysteresis.h"

#include "Util.h"
#include "format.h"

#include <vector>

namespace larcfm {

/*
 * Creates an empty object
 */
AlertingHysteresis::AlertingHysteresis() {
  hysteresis_time_ = 0;
  persistence_time_ = 0;
  init_time_ = NaN;
  last_time_ = NaN;
  last_alert_ = -1;
}

/*
 * Creates an object for given values of hysteresis, persistence time, and M of N parameters
 */
AlertingHysteresis::AlertingHysteresis(double hysteresis_time, double persistence_time, int m, int n) {
  mofn_.setMofN(m,n);
  hysteresis_time_ = hysteresis_time;
  persistence_time_ = persistence_time;
  init_time_ = NaN;
  last_time_ = NaN;
  last_alert_ = -1;
}

/*
 * Sets hysteresis and persistence time
 */
void AlertingHysteresis::setHysteresisPersistence(double hysteresis_time, double persistence_time) {
  hysteresis_time_ = hysteresis_time;
  persistence_time_ = persistence_time;
  reset();
}

double AlertingHysteresis::getLastTime() const {
  return last_time_;
}

int AlertingHysteresis::getLastAlert() const {
  return last_alert_;
}

/*
 * Reset object with given M of N value
 */
void AlertingHysteresis::reset(int val) {
  mofn_.reset(val);
  init_time_ = NaN;
  last_time_ = NaN;
  last_alert_ = -1;
}

/*
 * Reset object if current_time is less than or equal to last_time
 */
void AlertingHysteresis::resetIfCurrentTime(double current_time) {
  if (ISNAN(last_time_) ||
      current_time <= last_time_ ||
      current_time-last_time_ > hysteresis_time_) {
    reset(-1);
  }
}

/*
 * Return value for a given alert level at a current time.
 * In addition of m_of_n, this also applies hysteresis and persistence
 */
int AlertingHysteresis::alertingHysteresis(int alert_level, double current_time) {
  if (ISNAN(last_time_) ||
      current_time <= last_time_ || current_time-last_time_ > hysteresis_time_) {
    // Reset hysteresis if current_time is in the past of later than deadline
    reset(alert_level);
  }
  // Add alert level into M of N logic even if negative
  int alert_mofn = mofn_.m_of_n(alert_level);
  if (alert_mofn < 0) {
    // Return invalid output since input is invalid (negative)
    last_alert_ = alert_level;
    init_time_ = NaN;
  } else if (!ISNAN(init_time_) && last_alert_ > 0 && alert_mofn < last_alert_ &&
      current_time >= init_time_ &&
      current_time-init_time_ < persistence_time_) {
    // Do nothing. Keep the previous alert_
  } else {
    if (alert_mofn > 0 && alert_mofn > last_alert_) {
      init_time_ = current_time;
    }
    // Return a valid output (i.e., >= 0), since input is valid
    last_alert_ = Util::max(0,alert_mofn);
  }
  last_time_ = current_time;
  return last_alert_;
}

std::string AlertingHysteresis::toString() const {
  std::string s = "<";
  s += "hysteresis_time: "+FmPrecision(hysteresis_time_);
  s += ", persistence_time: "+FmPrecision(persistence_time_);
  s += ", init_time: "+FmPrecision(init_time_);
  s += ", last_time: "+FmPrecision(last_time_);
  s += ", last_alert: "+Fmi(last_alert_);
  s += ", "+mofn_.toString();
  s += ">";
  return s;
}


} /* namespace larcfm */
