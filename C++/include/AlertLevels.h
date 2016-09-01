/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef ALERTLEVELS_H_
#define ALERTLEVELS_H_

#include "AlertThresholds.h"
#include "ParameterData.h"
#include "ParameterAcceptor.h"

namespace larcfm {

class AlertLevels : public ParameterAcceptor {

public:

  std::vector<AlertThresholds> alertor_;

  int conflict_level_;

  // This numeric type is used for index variables over vectors of AlertThresholds
  typedef std::vector<AlertThresholds>::size_type nat;

  AlertLevels();

  ~AlertLevels();

  AlertLevels(const AlertLevels& alertor);

  /**
   * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
   * Maneuver guidance logic produces multilevel bands:
   * MID: Corrective
   * NEAR: Warning
   * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)
   *
   */
  static AlertLevels WC_SC_228();

  /**
   * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
   * Maneuver guidance logic produces single level bands:
   * NEAR: Corrective
   * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)
   *
   */
  static AlertLevels SingleBands_WC_SC_228();

  /**
   * @return alerting thresholds (buffered) as defined by SC-228 MOPS.
   * Maneuver guidance logic produces multilevel bands:
   * MID: Corrective
   * NEAR: Warning
   * Well-clear volume (buffered) is defined by alerting level 2 (conflict_alerting_level)
   *
   */
  static AlertLevels Buffered_WC_SC_228();

  /**
   * @return alerting thresholds for single bands given by detector,
   * alerting time, and lookahead time.
   */
  static AlertLevels SingleBands(const Detection3D* detector,
          double alerting_time, double lookahead_time);

  /**
   * Clears alert levels
   **/
  void clear();

  /**
   * @return numerical type of most severe alert level.
   */
  int mostSevereAlertLevel() const;

  /**
   * @return first guidance level whose region is different from NONE
   */
  int firstGuidanceLevel() const;

  /**
   * @return last guidance level whose region is different from NONE
   */
  int lastGuidanceLevel() const;

  /**
   * @return conflict alert level. When the numerical is value is 0, first guidance level
   */
  int conflictAlertLevel() const;

  /**
   * @return detector for given alert level
   */
  Detection3D* detectorRef(int alert_level) const;

  /**
   * @return detector for conflict alert level
   */
  Detection3D* conflictDetectorRef() const;

  /**
   * Set conflict alert level
   */
  void setConflictAlertLevel(int alert_level);

  /**
   * Set the threshold values of a given alert level.
   */
  void setLevel(int level, const AlertThresholds& thresholds);

  /**
   * Add an alert level and returns its numerical type, which is a positive number.
   */
  int addLevel(const AlertThresholds& thresholds);

  /**
   * @return threshold values of a given alert level
   */
  AlertThresholds const & getLevel(int level) const;


  /**
   * Set alertor to the same values as the given parameter
   */
  void copy(const AlertLevels& alertor);

  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);

  std::string toString() const;
  std::string toPVS(int prec) const;

};
}
#endif
