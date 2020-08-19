/*
 * Copyright (c) 2012-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef TCAS3D_H_
#define TCAS3D_H_

#include "Detection3D.h"
#include "TCAS2D.h"
#include "Vect2.h"
#include "TCASTable.h"
#include "ConflictData.h"
#include "Triple.h"

namespace larcfm {
class TCAS3D : public Detection3D {

private:
  TCASTable table_;
  std::string id;

public:

  /** Constructor that uses the default TCAS tables. */
  TCAS3D();

  /** Constructor that specifies a particular instance of the TCAS tables. */
  explicit TCAS3D(const TCASTable& tables);

  /**
   * @return one static TCAS3D
   */
  static const TCAS3D& A_TCAS3D();

  /**
   * @return one static TCASII_RA
   */
  static const TCAS3D& TCASII_RA();

  /**
   * @return one static TCASII_TA
   */
  static const TCAS3D& TCASII_TA();

  /** Make TCAS3D object with empty Table **/
  static TCAS3D make_Empty();

  /** Make TCAS3D object with an RA Table **/
  static TCAS3D make_TCASII_RA();

  /** Make TCAS3D objec with a TA Table **/
  static TCAS3D make_TCASII_TA();

  /** This returns a copy of the internal TCAS table */
  TCASTable& getTCASTable();

  /**
   * Set table to TCASII Thresholds (RA Table when ra is true, TA Table when ra is false)
   */
  void setDefaultTCASIIThresholds(bool ra);


  /** This sets the internal table to be a deep copy of the supplied one.  Any previous links will be broken. */
  void setTCASTable(const TCASTable& tables);

  // if true, then ownship has a TCAS resolution advisory at current time
  bool TCASII_RA(const Vect3& so, const Vect3& vo, const Vect3& si, const Vect3& vi) const;

  Triple<double,double,double> RA3D_interval(int sl, const Vect2& so2, double soz, const Vect2& vo2, double voz,
      const Vect2& si2, double siz, const Vect2& vi2, double viz, double B, double T) const;

  // if true, within lookahead time T, the ownship has a TCAS resolution advisory (at time time before T) (effectively conflict detection)
  ConflictData RA3D(const Vect3& so, const Velocity& vo,  const Vect3& si, const Velocity& vi, double B, double T) const;

  // The methods violation and conflict are inherited from Detection3DSum. This enable a uniform
  // treatment of border cases in the generic bands algorithms

  ConflictData conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;

  TCAS3D* copy() const;
  TCAS3D* make() const;

  static bool vertical_RA(double sz, double vz, double ZTHR, double TCOA);


  //  static double time_coalt(double sz, double voz, double viz, double h);


  static bool cd2d_TCAS_after(double HMD, Vect2 s, Vect2 vo, Vect2 vi, double t);

  static bool cd2d_TCAS(double HMD, Vect2 s, Vect2 vo, Vect2 vi);


  virtual std::string getSimpleClassName() const;


  /**
   * Returns TAU threshold for sensitivity level sl in seconds
   */
  double getTAU(int sl) const;

  /**
   * Returns TCOA threshold for sensitivity level sl in seconds
   */
  double getTCOA(int sl) const;

  /**
   * Returns DMOD for sensitivity level sl in internal units.
   */
  double getDMOD(int sl) const;

  /**
   * Returns DMOD for sensitivity level sl in u units.
   */
  double getDMOD(int sl, const std::string& u) const;

  /**
   * Returns Z threshold for sensitivity level sl in internal units.
   */
  double getZTHR(int sl) const;

  /**
   * Returns Z threshold for sensitivity level sl in u units.
   */
  double getZTHR(int sl,const std::string& u) const;

  /**
   * Returns HMD for sensitivity level sl in internal units.
   */
  double getHMD(int sl) const;

  /**
   * Returns HMD for sensitivity level sl in u units.
   */
  double getHMD(int sl, const std::string& u) const;

  /** Modify the value of Tau Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds
   */
  void setTAU(int sl, double val);

  /** Modify the value of TCOA Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds
   */
  void setTCOA(int sl, double val);

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setDMOD(int sl, double val);

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setDMOD(int sl, double val, const std::string& u);

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setZTHR(int sl, double val);

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setZTHR(int sl, double val, const std::string& u);

  /**
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setHMD(int sl, double val);

  /**
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setHMD(int sl, double val, const std::string& u);

  void setHMDFilter(bool flag);

  bool getHMDFilter();

  virtual std::string toString() const;
  virtual std::string toPVS() const;

  virtual std::string getIdentifier() const;
  virtual void setIdentifier(const std::string& id);

  virtual bool equals(Detection3D* d) const;

  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);

  bool contains(const Detection3D* cd) const;

  virtual void horizontalHazardZone(std::vector<Position>& haz, const TrafficState& ownship, const TrafficState& intruder,
      double T) const;

};

}
#endif
