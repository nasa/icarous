/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSINTEGERBANDS_H_
#define DAIDALUSINTEGERBANDS_H_

#include "Velocity.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "Detection3D.h"
#include "Integerval.h"
#include "IntervalSet.h"
#include "CriteriaCore.h"

#include <vector>
#include <string>
#include "TrafficState.h"

namespace larcfm {


class DaidalusIntegerBands {

  // This class computes NONE bands

protected:
  /* Used in definition of kinematic trajectory */
  int j_step_;

public:
  virtual std::pair<Vect3,Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const = 0;
  virtual ~DaidalusIntegerBands() {}


  bool LOS_at(const Detection3D* det, bool trajdir, double tsk,
      const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: int_bands@first_los_step
private:
  int first_los_step(const Detection3D* det, double ts,bool trajdir,
      int min, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: kinematic_bands@first_los_search_index
  int first_los_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2, bool trajdir, int max,
      const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: kinematic_bands@bands_search_index
  // epsh == epsv == 0, if traffic is not the repulsive aircraft
  int bands_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

  /** In PVS:
   * NOT CD_future_traj(CD,B,T,traj,kts,si,vi)) AND (NOT (useLOS2 AND CD_future_traj(CD2,B2,T2,traj,kts,si,vi)
   */
public:
  bool no_CD_future_traj(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2,
      double T2, bool trajdir, double tsk, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: int_bands@traj_conflict_only_band, int_bands@nat_bands, and int_bands@nat_bands_rec
private:
  void traj_conflict_only_bands(std::vector<Integerval>& l,
      const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep, double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters)  const;

  // In PVS: kinematic_bands@kinematic_bands
  void kinematic_bands(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

public:
  static void append_intband(std::vector<Integerval>& l, std::vector<Integerval>& r);

  static void neg(std::vector<Integerval>& l);

  // INTERFACE FUNCTION
  // In PVS: combine_bands@kinematic_bands_combine
  void kinematic_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

  // INTERFACE FUNCTION
  // In PVS: kinematic_bands_exist@first_green
  int first_green(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

  bool all_int_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv, int dir) const;

private:
  int first_instantaneous_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv); // not const!

public:
  bool all_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv, int dir); // not const!

private:
  Vect3 linvel(const TrafficState& ownship, double tstep, bool trajdir, int k) const;

  // In PVS: int_bands@repulsive_at
  bool repulsive_at(double tstep, bool trajdir, int k, const TrafficState& ownship, const TrafficState& traffic, int epsh) const;

  // In PVS: int_bands@first_nonrepulsive_step
  int first_nonrepulsive_step(double tstep, bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, int epsh) const;

  // In PVS: int_bands@vert_repul_at
  bool vert_repul_at(double tstep, bool trajdir, int k, const TrafficState& ownship, const TrafficState& traffic, int epsv) const;

  // In PVS: int_bands@first_nonvert_repul_step
  int first_nonvert_repul_step(double tstep, bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, int epsv) const;

  // In PVS: CD(B,T,so,vo,si,vi)
  static bool conflict(const Detection3D* det, const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      double B, double T, double s_err, double sz_err, double v_err, double vz_err);

  // In PVS: int_bands@CD_future_traj
public:
  bool CD_future_traj(const Detection3D* det, double B, double T, bool trajdir, double tsk,
      const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: first_conflict_step(CD,B,T,traj,0,ts,si,vi,MaxN) >= 0
private:
  bool any_conflict_step(const Detection3D* det, double tstep, double B, double T, bool trajdir, int max,
      const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const;

  // In PVS: kinematic_bands_exist@red_band_exist
public:
  bool red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

private:
  bool instantaneous_red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv); // not const

  // INTERFACE FUNCTION
public:
  bool any_int_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv, int dir) const;

  bool any_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv, int dir); // not const

  // INTERFACE FUNCTION
  // In PVS: inst_bands@instant_track_bands, inst_bands@instant_gs_bands, inst_bands@instant_vs_bands
  void instantaneous_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv); // not const

  //In PVS: inst_bands@conflict_free_track_step, inst_bands@conflict_free_gs_step, inst_bands@conflict_free_vs_step
  bool no_instantaneous_conflict(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
      bool trajdir, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv) const;

  //In PVS: int_bands@nat_bands, int_bands@nat_bands_rec
private:
  void instantaneous_bands(std::vector<Integerval>& l,
      const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      int epsh, int epsv); // not const

};

}

#endif
