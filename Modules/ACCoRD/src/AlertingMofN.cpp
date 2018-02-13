/*
 * Implementation of M of N algorithm for alerting
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "AlertingMofN.h"
#include <vector>

namespace larcfm {

/*
 * Creates an empty M of N object
 */
AlertingMofN::AlertingMofN() {
  m_ = 1;
  n_ = 0;
  max_ = 0;
}

/*
 * Creates a M of N object for given values of m, n, and maximum alert level.
 * Assumes  m >= 1 and m <= n
 */
AlertingMofN::AlertingMofN(int m, int n, int max_alert_level) {
  m_ = m;
  n_ = n;
  max_ = max_alert_level;
  for (int i=0;i<n_;++i) {
    queue_.push_back(0);
  }
}

AlertingMofN::~AlertingMofN() {
}


/*
 * Set new values for m and n.
 * Assumes  m >= 1 and m <= n
 */
void AlertingMofN::setMofN(int m, int n) {
  m_ = m;
  n_ = n;
  reset();
}

/*
 * Set new values for maximum alert level
 * Assumes max_alert_level >= 1
 */
void AlertingMofN::setMaxAlertLevel(int max_alert_level) {
  max_ = max_alert_level;
  reset();
}

/*
 * Reset M of N object.
 */
void AlertingMofN::reset() {
  queue_.clear();
  for (int i=0;i<n_;++i) {
    queue_.push_back(0);
  }
}

/*
 * Return M of N value for a given alert level
 */
int AlertingMofN::m_of_n(int alert_level) {
  if (queue_.empty() || alert_level < 0 || alert_level > max_ || m_ <= 1 || max_ <= 0) {
    return alert_level;
  }
  queue_.pop_front();
  queue_.push_back(alert_level);
  std::vector<int> count(max_,0);
  for (std::deque<int>::iterator it = queue_.begin(); it!=queue_.end(); ++it) {
    int alert = (*it);
    for (int i=alert-1;i >= 0;--i) {
      count[i]++;
    }
  }
  for (int i=max_-1; i >= 0; --i) {
    if (count[i] >= m_) {
      return i+1;
    }
  }
  return 0;
}

} /* namespace larcfm */
