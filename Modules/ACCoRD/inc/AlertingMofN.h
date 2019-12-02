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
  AlertingMofN(int m, int n, double hysteresis_time, double persistence_time);

  /*
   * Reset M of N object.
   */
  void reset();

  /*
   * Reset M of N object with given parameters
   */
  void reset(int m, int n, double hysteresis_time, double persistence_time);

  /*
   * Set new values for m and n.
   * Assumes  m >= 1 and m <= n
   */
  void setMofN(int m, int n);

  bool isValid() const;

  /*
   * Return M of N value for a given alert level
   */
  int m_of_n(int alert_level);

  /*
   * Return M of N value for a given alert level at a current time.
   * In addition of m_of_n, this also applies hysteresis and persistence
   */
  int m_of_n(int alert_level, double current_time);

  virtual ~AlertingMofN() {};

private:
  int m_;
  int n_;
  double hysteresis_time_;
  double persistence_time_;
  double _init_time_;
  double _last_time_;
  int    _alert_;
  int    _max_;
  std::deque<int> _queue_;
};

} /* namespace larcfm */

#endif /* ALERTINGMOFN_H_ */
