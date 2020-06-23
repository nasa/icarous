/*
 * Implementation of hysteresis logic that includes MofN and persistence.
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef HYSTERESISDATA_H_
#define HYSTERESISDATA_H_

#include "MofN.h"
#include <deque>

namespace larcfm {

class HysteresisData {
public:
  /*
   * Creates an empty object
   */
  HysteresisData();

  /*
   * Creates an object for given values of hysteresis, persistence time, and M of N parameters
   */
  HysteresisData(double hysteresis_time, double persistence_time, int m, int n);

  void outdateIfCurrentTime(double current_time);

  bool isUpdatedAtCurrentTime(double current_time) const;

  double getInitTime() const;

  double getLastTime() const;

  int getLastValue() const;

  /*
   * Reset object with given M of N value
   */
  void reset(int val);

  std::string toString() const;

  /*
   * In addition of m_of_n, this function applies persistence logic
   */
  int applyHysteresisLogic(int current_value, double current_time);

  virtual ~HysteresisData() {};

private:
  MofN   mofn_;
  double hysteresis_time_;
  double persistence_time_;
  double init_time_;
  double last_time_;
  int    last_value_;
  // When this flag is true, setting a value at current time
  // resets hysteresis values
  bool outdated_;

};

} /* namespace larcfm */

#endif /* HYSTERESISDATA_H_ */
