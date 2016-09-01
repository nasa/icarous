/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef KINEMATICBANDS_H_
#define KINEMATICBANDS_H_

#include "KinematicMultiBands.h"

#include <vector>
#include <string>

namespace larcfm {


class KinematicBands : public KinematicMultiBands {

public:

  /**
   * Construct a KinematicBands with default parameters. The initial state detector
   * is specified by the parameters.
   * @param detector        State detector
   */
  KinematicBands(const Detection3D* detector);

  /**
   * Construct a KinematicBands object with default values for
   * configuration parameters. The default state detector is cylinder.
   */
  KinematicBands();

  /**
   * Construct a KinematicBands object from an existing KinematicBands object. This copies all traffic data.
   */
  KinematicBands(const KinematicBands& b);

  /**
   * Sets lookahead time in seconds. This is the time horizon used to compute bands.
   */
  virtual void setLookaheadTime(double t);

  /**
   * @return alerting time in seconds. This is the first time prior to a violation when bands are
   * computed. When this value is 0, lookahead time is used instead.
   */
  double getAlertingTime() const;

  /**
   * Sets alerting time in seconds. This is the first time prior to a violation when bands are
   * computed. When this value is 0, lookahead time is used instead.
   */
  void setAlertingTime(double t);

  /** Experimental. Set the underlying Detection3D object that will be used to determine conflicts.
   * This will also clear any results (but not traffic info).
   */
  void setCoreDetection(const Detection3D* detector);

};
}
#endif
