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

#ifndef ALERTINGHYSTERESIS_H_
#define ALERTINGHYSTERESIS_H_

#include "MofN.h"
#include <deque>

namespace larcfm {

class AlertingHysteresis {
public:
  /*
   * Creates an empty object
   */
  AlertingHysteresis();

  /*
   * Creates an object for given values of hysteresis, persistence time, and M of N parameters
   */
  AlertingHysteresis(double hysteresis_time, double persistence_time, int m, int n);

  /*
   * Sets hysteresis and persistence time
   */
  void setHysteresisPersistence(double hysteresis_time, double persistence_time);

  double getLastTime() const;

  int getLastAlert() const;

  /*
   * Reset object with given M of N value
   */
  void reset(int val=-1);

  /*
   * Reset object if current_time is less than or equal to last_time
   */
  void resetIfCurrentTime(double current_time);

  std::string toString() const;

  /*
   * In addition of m_of_n, this function applies persistence logic
   */
  int alertingHysteresis(int alert_level, double current_time);

  virtual ~AlertingHysteresis() {};

private:
  MofN   mofn_;
  double hysteresis_time_;
  double persistence_time_;
  double init_time_;
  double last_time_;
  int    last_alert_;

};

} /* namespace larcfm */

#endif /* ALERTINGHYSTERESIS_H_ */
