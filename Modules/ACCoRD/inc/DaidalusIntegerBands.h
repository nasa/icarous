/*
 * Copyright (c) 2015-2020 United States Government as represented by
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
#include "DaidalusParameters.h"

#include <vector>
#include <string>
#include "TrafficState.h"

namespace larcfm {


class DaidalusIntegerBands {

  // This class computes NONE bands

public:
  // trajdir == false is left/down
  // target_step is used by instantaneous_bands and altitude_bands
  virtual std::pair<Vect3,Velocity> trajectory(const DaidalusParameters& parameters, const TrafficState& ownship,
      double time, bool dir, int target_step, bool instantaneous) const = 0;

  virtual ~DaidalusIntegerBands() {}

  /*
   * In PVS: int_bands@CD_future_traj
   */
  bool CD_future_traj(const Detection3D* det, double B, double T, bool trajdir, double tsk,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int target_step, bool instantaneous) const;

  /**
   * In PVS:
   * NOT CD_future_traj(CD,B,T,traj,kts,si,vi)) AND (NOT (useLOS2 AND CD_future_traj(CD2,B2,T2,traj,kts,si,vi)
   */
  bool no_CD_future_traj(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T,  bool trajdir, double tsk,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int target_step, bool instantaneous) const;

  /**
   * In PVS:
   * LET kts = k*ts,
   * sot=traj(kts)`1,
   * vot=traj(kts)`2 IN
   * LOS_at(sot-kts*vot,vot,si,vi,kts)
   */
  bool LOS_at(const Detection3D* det, bool trajdir, double tsk,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int target_step, bool instantaneous) const;

private:
  // In PVS: int_bands@first_los_step
  int kinematic_first_los_step(const Detection3D* det, double tstep, bool trajdir,
      int min, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const;

  // In PVS: kinematic_bands@first_los_search_index
  int kinematic_first_los_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, bool trajdir, int max,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const;

  // In PVS: kinematic_bands@bands_search_index
  // epsh == epsv == 0, if traffic is not the repulsive aircraft
  int kinematic_bands_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  // In PVS: int_bands@traj_conflict_only_band, int_bands@nat_bands, and int_bands@nat_bands_rec
  void kinematic_traj_conflict_only_bands(std::vector<Integerval>& l,
      const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep, double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic)  const;

  // In PVS: kinematic_bands@kinematic_bands
  void kinematic_bands(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  // In PVS: kinematic_bands_exist@first_green
  int first_kinematic_green(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  Vect3 kinematic_linvel(const DaidalusParameters& parameters, const TrafficState& ownship, double tstep, bool trajdir, int k) const;

  // In PVS: int_bands@repulsive_at
  bool kinematic_repulsive_at(double tstep, bool trajdir, int k, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int epsh) const;

  // In PVS: int_bands@first_nonrepulsive_step
  int kinematic_first_nonrepulsive_step(double tstep, bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int epsh) const;

  // In PVS: int_bands@vert_repul_at
  bool kinematic_vert_repul_at(double tstep, bool trajdir, int k, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int epsv) const;

  // In PVS: int_bands@first_nonvert_repul_step
  int kinematic_first_nonvert_repul_step(double tstep, bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int epsv) const;

  // In PVS: first_conflict_step(CD,B,T,traj,0,ts,si,vi,MaxN) >= 0
  bool kinematic_any_conflict_step(const Detection3D* det, double tstep, double B, double T, bool trajdir, int max,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const;

  // In PVS: kinematic_bands_exist@red_band_exist
  bool kinematic_red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  int first_instantaneous_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  //In PVS: inst_bands@conflict_free_track_step, inst_bands@conflict_free_gs_step, inst_bands@conflict_free_vs_step
  bool no_instantaneous_conflict(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T,
      bool trajdir, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int target_step) const;

  //In PVS: int_bands@nat_bands, int_bands@nat_bands_rec
  void instantaneous_bands(std::vector<Integerval>& l,
      const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  bool instantaneous_red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T,
      bool trajdir, int max, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  // In PVS: combine_bands@kinematic_bands_combine
  void kinematic_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;


  // In PVS: inst_bands@instant_track_bands, inst_bands@instant_gs_bands, inst_bands@instant_vs_bands
  void instantaneous_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  bool all_kinematic_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

  bool all_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

  bool any_kinematic_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

  bool any_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

public:

  static void append_intband(std::vector<Integerval>& l, std::vector<Integerval>& r);

  static void neg(std::vector<Integerval>& l);

  // INTERFACE FUNCTIONS

  void integer_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv) const;

  bool all_integer_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

  bool any_integer_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
      double B, double T,
      int maxl, int maxr, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      int epsh, int epsv, int dir) const;

};

}

#endif
