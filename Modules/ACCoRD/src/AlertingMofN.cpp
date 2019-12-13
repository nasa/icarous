/*
 * Implementation of M of N algorithm for alerting
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "AlertingMofN.h"
#include "Util.h"

#include <vector>

namespace larcfm {

/*
 * Creates an empty M of N object
 */
AlertingMofN::AlertingMofN() {
  m_ = 0;
  n_ = 0;
  hysteresis_time_ = 0;
  persistence_time_ = 0;
  reset();
}

/*
 * Creates a M of N object for given values of m, n
 * Assumes  m >= 1 and m <= n
 */
AlertingMofN::AlertingMofN(int m, int n, double hysteresis_time, double persistence_time) {
  reset(m,n,hysteresis_time,persistence_time);
}

/*
 * Reset M of N object.
 */
void AlertingMofN::reset() {
  _init_time_ = NaN;
  _last_time_ = NaN;
  _alert_ = -1;
  _max_ = 0;
  _queue_.clear();
  for (int i=0;i<n_;++i) {
    _queue_.push_back(0);
  }
}

/*
 * Creates a M of N object for given values of m, n
 * Assumes  m >= 1 and m <= n
 */
void AlertingMofN::reset(int m, int n, double hysteresis_time, double persistence_time) {
  m_ = m;
  n_ = n;
  hysteresis_time_ = Util::max(hysteresis_time,persistence_time);
  persistence_time_ = persistence_time;
  reset();
}

/*
 * Set new values for m and n.
 * Assumes  m >= 1 and m <= n
 */
void AlertingMofN::setMofN(int m, int n) {
  if (m_ != m || n_ != n) {
    m_ = m;
    n_ = n;
    reset();
  }
}

bool AlertingMofN::isValid() const {
  return m_ > 0 && m_ <= n_;
}

/*
 * Return M of N value for a given alert level
 */
int AlertingMofN::m_of_n(int alert_level) {
  if (_queue_.empty() || alert_level < 0 ||  !isValid()) {
    return alert_level;
  }
  if (alert_level > _max_) {
    _max_ = alert_level;
  }
  _queue_.pop_front();
  _queue_.push_back(alert_level);
  if (_max_ == 0) {
    return 0;
  }
  std::vector<int> count(_max_,0);
  for (std::deque<int>::const_iterator alert_ptr = _queue_.begin(); alert_ptr != _queue_.end(); ++alert_ptr) {
    for (int i=(*alert_ptr)-1;i >= 0;--i) {
      count[i]++;
    }
  }
  for (int i=_max_-1; i >= 0; --i) {
    if (count[i] >= m_) {
      return i+1;
    }
  }
  return 0;
}

/*
 * Return M of N value for a given alert level at a current time.
 * In addition of m_of_n, this also applies hysteresis and persistence
 */
int AlertingMofN::m_of_n(int alert_level, double current_time) {
  if (!ISNAN(_last_time_)) {
    if (current_time == _last_time_) {
      return _alert_;
    }
    if (current_time < _last_time_ || current_time - _last_time_ > hysteresis_time_) {
      reset();
    }
  }
  int alert_mofn = m_of_n(alert_level);
  if (alert_mofn < 0) {
    _last_time_ = current_time;
    _alert_ = alert_mofn;
    return alert_mofn;
  }
  if (!ISNAN(_init_time_) && _alert_ > 0 && alert_mofn < _alert_ && current_time >= _init_time_ &&
      current_time - _init_time_ < persistence_time_) {
    // Do nothing. Keep the previous alert_
  } else {
    if (alert_mofn > 0 && alert_mofn != _alert_) {
      _init_time_ = current_time;
    }
    _alert_ = alert_mofn;
  }
  _last_time_ = current_time;
  return _alert_;
}

} /* namespace larcfm */
