/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef ALERTER_H_
#define ALERTER_H_

#include "AlertThresholds.h"
#include "BandsRegion.h"
#include "ParameterData.h"
#include "ParameterAcceptor.h"
#include "Detection3D.h"

namespace larcfm {

/** 
 * It is assumed that for all alert level i >= 1: detection(i+1) => detection(i) and that there is at least one
 * guidance level whose region is different from NONE. 
 */
class Alerter : ParameterAcceptor {


private:
  std::vector<AlertThresholds> levels_; // This list is 1-indexed at the user level. 0 means none.
  std::string id_;

public:

  Alerter();

  ~Alerter() {};

  static const Alerter& INVALID();

  bool isValid() const;

  Alerter(const std::string& id);

  /**
   * Set alerter identifier.
   * @param id
   */
  void setId(const std::string& id);

  /**
   * @return alerter identifier.
   */
  const std::string& getId() const;

  /**
   * @return alerting thresholds (unbuffered) as defined in RTCA DO-365.
   * Maneuver guidance logic produces multilevel bands:
   * MID: Corrective
   * NEAR: Warning
   */
  static const Alerter& DWC_Phase_I();

  /**
   * @return alerting thresholds (buffered) as defined in RTCA DO-365.
   * Maneuver guidance logic produces multilevel bands:
   * MID: Corrective
   * NEAR: Warning
   */
  static const Alerter& Buffered_DWC_Phase_I();

  /**
   * @return alerting thresholds for single bands given by detector,
   * alerting time, and lookahead time. The single bands region is NEAR
   */
  static Alerter SingleBands(const Detection3D& detector, double alerting_time, double lookahead_time);

  /**
   * @return alerting thresholds for ACCoRD's CD3D, i.e.,
   * Separation is given by cylinder of 5nmi/1000ft. Lookahead time is 180s.
   */
  static const Alerter& CD3D();

  /**
   * Clears alert levels
   **/
  void clear();

  /**
   * @return most severe alert level.
   */
  int mostSevereAlertLevel() const;

  /**
   * @return first alert level whose region is equal to given one. Returns 0 if none.
   */
  int alertLevelForRegion(BandsRegion::Region region) const;

  /**
   * @return detector for given alert level starting from 1.
   */
  Detection3D* getDetectorPtr(int alert_level) const;


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
  const AlertThresholds& getLevel(int alert_level) const;

  ParameterData getParameters() const;

  void updateParameterData(ParameterData& p) const;

  void setParameters(const ParameterData& p);

  std::string toString() const;

  std::string toPVS() const;

  static std::string listToPVS(const std::vector<Alerter>& alerter);


};
}
#endif
