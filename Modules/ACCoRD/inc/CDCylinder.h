/*
 * CD3D.h 
 * Release: ACCoRD++-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * CD3D is an algorithm for 3-D conflict *detection*.
 *
 * Unit Convention
 * ---------------
 * All units in this file are *internal*:
 * - Units of distance are denoted [d]
 * - Units of time are denoted     [t]
 * - Units of speed are denoted    [d/t]
 *
 * REMARK: X points to East, Y points to North. 
 *
 * Naming Convention
 * -----------------
 *   The intruder is fixed at the origin of the coordinate system.
 * 
 *   D  : Diameter of the protected zone [d]
 *   H  : Height of the protected zone [d]
 *   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *   T  : Upper bound of lookahead time interval [t] (T < 0 means infinite lookahead time)
 *   s  : Relative 3-D position of the ownship [d,d,d]
 *   vo : Ownship velocity vector [d/t,d/t,d/t]
 *   vi : Traffic velocity vector [d/t,d/t,d/t]
 * 
 * Functions
 * ---------
 * LoS : Check for 3-D loss of separation
 * detection : 3-D conflict detection with calculation of conflict interval 
 * cd3d      : Check for predicted conflict
 * 
 * Global variables (modified by detection)
 * ----------------
 * t_in  : Time to loss of separation
 * t_out : Time to recovery from loss of separation
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CDCYLINDER_H_
#define CDCYLINDER_H_

#include "CD2D.h"
#include "CD3D.h"
#include "Vertical.h"
#include "Detection3D.h"
#include "LossData.h"
#include "ConflictData.h"

namespace larcfm {

class CDCylinder : public Detection3D {

private:
  std::string id;
  std::map<std::string,std::string> units_;
  double D_;
  double H_;

public:

  /**
   * Instantiates a new CD3D object.
   */
  CDCylinder(const std::string& s="");

  /**
   * This specifies the internal table is a copy of the provided table
   * @param tab
   */
  CDCylinder(const CDCylinder& cdc);

  CDCylinder(double d, double h);
  CDCylinder(double d, const std::string& dunit, double h, const std::string& hunit);

  /**
   * Create a new state-based conflict detection object using specified units.
   * 
   * @param distance the minimum horizontal separation distance in specified units
   * @param height the minimum vertical separation height in specified units.
   */
  static CDCylinder make(double distance, const std::string& dUnits, double height, const std::string& hUnits);

  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  static CDCylinder mk(double distance, double height);

  /**
   * @return one static CDCylinder
   */
  static const CDCylinder& A_CDCylinder();

  /**
   * @return CDCylinder thresholds, i.e., D=5nmi, H=1000ft.
   */
  static const CDCylinder& CD3DCylinder();

  std::string getUnits(const std::string& key) const;

  double getHorizontalSeparation() const;

  void setHorizontalSeparation(double d);

  double getVerticalSeparation() const;

  void setVerticalSeparation(double h);

  double getHorizontalSeparation(const std::string& u) const;

  void setHorizontalSeparation(double d, const std::string& u);

  double getVerticalSeparation(const std::string& u) const;

  void setVerticalSeparation(double h, const std::string& u);

  /**
   * Computes the conflict time interval in [B,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time (B < T)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [B,T].
   */
  LossData detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double B, const double T) const;

  /**
   * Computes the conflict time interval in [0,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the the lookahead time (T > 0)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [0,T].
   */
  LossData detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double T) const;

  /**
   * Computes the conflict time interval in [0,...).
   *
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return true, if the conflict time interval (t_in,t_out) is in [0,...)
   */
  LossData detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H) const;

  virtual ~CDCylinder() {};

  static ConflictData conflict_detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double B, double T);
  static double time_of_closest_approach(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double B, double T);


  // The non-static methods violation and conflict are
  // inherited from Detection3D. This enable a uniform
  // treatment of border cases in the generic bands algorithms

  virtual ConflictData conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;
  double timeOfClosestApproach(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;

  /** This returns a pointer to a new instance of this type of Detector3D.  You are responsible for destroying this instance when it is no longer needed. */
  virtual CDCylinder* copy() const;
  virtual CDCylinder* make() const;
  virtual ParameterData getParameters() const;
  virtual void updateParameterData(ParameterData& p) const;
  virtual void setParameters(const ParameterData& p);

  virtual std::string getSimpleClassName() const;
  virtual std::string toString() const;
  virtual std::string toPVS() const;

  virtual std::string getIdentifier() const;
  virtual void setIdentifier(const std::string& s);

  virtual bool equals(Detection3D* d) const;
  virtual bool contains(const Detection3D* cd) const;

};


}

#endif /* CDCYLINDER_H_ */
