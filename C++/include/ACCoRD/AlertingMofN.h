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

#ifndef ALERTINGMOFN_H_
#define ALERTINGMOFN_H_

#include <deque>

namespace larcfm {

class AlertingMofN {
public:
  /*
   * Creates an empty M of N object
   */
  AlertingMofN();

  /*
   * Creates a M of N object for given values of m, n, and maximum alert level.
   * Assumes  m >= 1 and m <= n
   */
  AlertingMofN(int m, int n, int max_alert_level);

  /*
   * Set new values for m and n.
   * Assumes  m >= 1 and m <= n
   */
  void setMofN(int m, int n);

  /*
   * Set new values for maximum alert level
   * Assumes max_alert_level >= 1
   */
  void setMaxAlertLevel(int max_alert_level);

  /*
   * Reset M of N object.
   */
  void reset();

  /*
   * Return M of N value for a given alert level
   */
  int m_of_n(int alert_level);

  virtual ~AlertingMofN();
private:
  int m_;
  int n_;
  int max_;
  std::deque<int> queue_;
};

} /* namespace larcfm */

#endif /* SRC_ALERTINGMOFN_H_ */
