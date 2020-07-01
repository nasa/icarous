#include "Commands.hpp"
#include "Interfaces.h"

#ifndef COGNITION_H
#define COGNITION_H

#ifdef __cplusplus
extern "C" {
#endif

void* CognitionInit(void);
void Reset(void *obj);
void ResetFlightPhases(void *obj);
void InputVehicleState(void *obj,const double pos[3],const double vel[3],const double heading);
void InputFlightPlanData(void *obj,
                         const char plan_id[],
                         const double scenario_time,
                         const int wp_id,
                         const double wp_position[3],
                         const int wp_metric,
                         const double wp_value);
void InputParameters(void *obj,const cognition_params_t *new_params);
void InputDitchStatus(void *obj,const double ditch_pos[3],const bool ditch_requested);
void InputMergeStatus(void *obj,const int merge_status);
void InputTrackBands(void *obj,const bands_t *track_bands);
void InputSpeedBands(void *obj,const bands_t *speed_bands);
void InputAltBands(void *obj,const bands_t *alt_bands);
void InputVSBands(void *obj,const bands_t *vs_bands);
void InputGeofenceConflictData(void *obj,const geofenceConflict_t *gf_conflict);
void ReachedWaypoint(void *obj, const char plan_id[], const int wp_reached_id);
int GetCognitionOutput(void *obj,Command *command);
void GetResolutionType(void *obj);
void StartMission(void *obj,const int mission_start_value,const double delay);
int FlightPhases(void *obj);

#ifdef __cplusplus
}
#endif

#endif
