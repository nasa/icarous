/*
 * File: PC_Quadcopter_Simulation.h
 *
 * Code generated for Simulink model 'PC_Quadcopter_Simulation'.
 *
 * Model version                  : 1.66
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Aug 17 11:28:00 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_PC_Quadcopter_Simulation_h_
#define RTW_HEADER_PC_Quadcopter_Simulation_h_
#include <float.h>
#include <math.h>
#include <string.h>
#ifndef PC_Quadcopter_Simulation_COMMON_INCLUDES_
# define PC_Quadcopter_Simulation_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* PC_Quadcopter_Simulation_COMMON_INCLUDES_ */

#include "PC_Quadcopter_Simulation_types.h"
#include "rtGetNaN.h"
#include "rt_nonfinite.h"
#include "rt_defines.h"
#include "rtGetInf.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T DiscreteTimeIntegrator_DSTATE[12];/* '<S5>/Discrete-Time Integrator' */
  real_T UD_DSTATE[3];                 /* '<S26>/UD' */
  real_T DiscreteTimeIntegrator_DSTATE_i;/* '<S6>/Discrete-Time Integrator' */
  real_T UD_DSTATE_m;                  /* '<S10>/UD' */
  real_T DiscreteTimeIntegrator_DSTATE_c;/* '<S9>/Discrete-Time Integrator' */
  real_T UD_DSTATE_e;                  /* '<S15>/UD' */
  real_T UD_DSTATE_c;                  /* '<S17>/UD' */
  real_T DiscreteTimeIntegrator_DSTATE_m;/* '<S16>/Discrete-Time Integrator' */
  real_T UD_DSTATE_j;                  /* '<S21>/UD' */
  real_T DiscreteTimeIntegrator_DSTATE_d;/* '<S20>/Discrete-Time Integrator' */
  real_T DiscreteTimeIntegrator_DSTATE_a;/* '<S19>/Discrete-Time Integrator' */
  real_T DiscreteTimeIntegrator_DSTATE_e;/* '<S18>/Discrete-Time Integrator' */
  real_T UnitDelay_DSTATE;             /* '<S33>/Unit Delay' */
  real_T UnitDelay_DSTATE_f;           /* '<S34>/Unit Delay' */
  real_T UnitDelay_DSTATE_h;           /* '<S35>/Unit Delay' */
  real_T UnitDelay_DSTATE_a;           /* '<S36>/Unit Delay' */
  real_T PrevY;                        /* '<S20>/Rate Limiter' */
} DW_PC_Quadcopter_Simulation_T;

/* Invariant block signals (default storage) */
typedef struct {
  const real_T P;                      /* '<S29>/P' */
  const real_T Q;                      /* '<S29>/Q' */
  const real_T R;                      /* '<S29>/R' */
  const real_T Phi;                    /* '<S29>/Phi ' */
  const real_T The;                    /* '<S29>/The' */
  const real_T Psi;                    /* '<S29>/Psi' */
  const real_T U;                      /* '<S29>/U' */
  const real_T V;                      /* '<S29>/V' */
  const real_T W;                      /* '<S29>/W' */
  const real_T X;                      /* '<S29>/X' */
  const real_T Y;                      /* '<S29>/Y' */
  const real_T Z;                      /* '<S29>/Z' */
  const real_T Add;                    /* '<S23>/Add' */
  const real_T MathFunction;           /* '<S23>/Math Function' */
  const real_T Add1;                   /* '<S23>/Add1' */
} ConstB_PC_Quadcopter_Simulati_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: quadModel.dctcq
   * Referenced by: '<S32>/dctdq'
   */
  real_T dctdq_Value[12];

  /* Expression: eye(3)
   * Referenced by: '<S11>/Constant'
   */
  real_T Constant_Value[9];

  /* Expression: quadModel.Jbinv
   * Referenced by: '<S32>/Jbinv'
   */
  real_T Jbinv_Value[9];

  /* Expression: quadModel.Jb
   * Referenced by: '<S32>/Jb'
   */
  real_T Jb_Value[9];
} ConstP_PC_Quadcopter_Simulati_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Xd_I;                         /* '<Root>/Xd_I' */
  real_T Yd_I;                         /* '<Root>/Yd_I' */
  real_T Zd_I;                         /* '<Root>/Zd_I' */
} ExtU_PC_Quadcopter_Simulation_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T yout[30];                     /* '<Root>/yout ' */ //15, next 4 motor rpms, 20,21,22,23, throttle, 24,25,26,27, roll,pitch,vdot commands, xdot, ydot, zdot 
}ExtY_PC_Quadcopter_Simulation_T;

/* Real-time Model Data Structure */
struct tag_RTM_PC_Quadcopter_Simulat_T {
  const char_T * volatile errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
  } Timing;
};

/* Block states (default storage) */
extern DW_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_Y;
extern const ConstB_PC_Quadcopter_Simulati_T PC_Quadcopter_Simulation_ConstB;/* constant block i/o */

/* Constant parameters (default storage) */
extern const ConstP_PC_Quadcopter_Simulati_T PC_Quadcopter_Simulation_ConstP;

/* Model entry point functions */
extern void PC_Quadcopter_Simulation_initialize(void);
extern void PC_Quadcopter_Simulation_step(void);
extern void PC_Quadcopter_Simulation_terminate(void);

/* Real-time Model object */
extern RT_MODEL_PC_Quadcopter_Simula_T *const PC_Quadcopter_Simulation_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S10>/Data Type Duplicate' : Unused code path elimination
 * Block '<S6>/Scope1' : Unused code path elimination
 * Block '<S6>/Scope3' : Unused code path elimination
 * Block '<S15>/Data Type Duplicate' : Unused code path elimination
 * Block '<S9>/Scope1' : Unused code path elimination
 * Block '<S9>/Scope3' : Unused code path elimination
 * Block '<S21>/Data Type Duplicate' : Unused code path elimination
 * Block '<S16>/Scope' : Unused code path elimination
 * Block '<S16>/Scope1' : Unused code path elimination
 * Block '<S16>/Scope2' : Unused code path elimination
 * Block '<S17>/Data Type Duplicate' : Unused code path elimination
 * Block '<S18>/Scope' : Unused code path elimination
 * Block '<S19>/Scope' : Unused code path elimination
 * Block '<S19>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope2' : Unused code path elimination
 * Block '<S4>/Constant' : Unused code path elimination
 * Block '<S4>/Product' : Unused code path elimination
 * Block '<S4>/Scope' : Unused code path elimination
 * Block '<S4>/Scope1' : Unused code path elimination
 * Block '<S4>/Scope2' : Unused code path elimination
 * Block '<S25>/Sum' : Unused code path elimination
 * Block '<S25>/Sum1' : Unused code path elimination
 * Block '<S25>/Sum11' : Unused code path elimination
 * Block '<S25>/Sum2' : Unused code path elimination
 * Block '<S25>/Sum3' : Unused code path elimination
 * Block '<S25>/Sum4' : Unused code path elimination
 * Block '<S25>/Sum5' : Unused code path elimination
 * Block '<S25>/Sum6' : Unused code path elimination
 * Block '<S25>/Sum7' : Unused code path elimination
 * Block '<S25>/Sum8' : Unused code path elimination
 * Block '<S26>/Data Type Duplicate' : Unused code path elimination
 * Block '<S5>/Math Function' : Unused code path elimination
 * Block '<S30>/Scope' : Unused code path elimination
 * Block '<S5>/Sqrt' : Unused code path elimination
 * Block '<S5>/Sum of Elements' : Unused code path elimination
 * Block '<S5>/To Workspace' : Unused code path elimination
 * Block '<Root>/Sum' : Unused code path elimination
 * Block '<Root>/To Workspace1' : Unused code path elimination
 * Block '<Root>/To Workspace2' : Unused code path elimination
 * Block '<Root>/To Workspace3' : Unused code path elimination
 * Block '<S20>/Kps' : Eliminated nontunable gain of 1
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'PC_Quadcopter_Simulation'
 * '<S1>'   : 'PC_Quadcopter_Simulation/Simulation'
 * '<S2>'   : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control'
 * '<S3>'   : 'PC_Quadcopter_Simulation/Simulation/Controller'
 * '<S4>'   : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Control Mixing'
 * '<S5>'   : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics'
 * '<S6>'   : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Phi Command Control'
 * '<S7>'   : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Psi Command Control1'
 * '<S8>'   : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Rotation to DCM'
 * '<S9>'   : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Theta Command Control'
 * '<S10>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Phi Command Control/Discrete Derivative1'
 * '<S11>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Rotation to DCM/Quaternion to Direction Cosine Matrix (DCM rotates a vector in the "inertial" frame into the body frame)'
 * '<S12>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Rotation to DCM/Rotation Angles to Quaternions'
 * '<S13>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Rotation to DCM/Quaternion to Direction Cosine Matrix (DCM rotates a vector in the "inertial" frame into the body frame)/3 Element Vector  Euclidian Norm1'
 * '<S14>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Rotation to DCM/Quaternion to Direction Cosine Matrix (DCM rotates a vector in the "inertial" frame into the body frame)/Build Skew Symetric  Cross Product Matrix'
 * '<S15>'  : 'PC_Quadcopter_Simulation/Simulation/Attitude Command Control/Theta Command Control/Discrete Derivative1'
 * '<S16>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Altitude (Z) Control'
 * '<S17>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Discrete Derivative'
 * '<S18>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Pitch (Theta) Control'
 * '<S19>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Roll (Phi) Control '
 * '<S20>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Yaw (Psi) Control'
 * '<S21>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Altitude (Z) Control/Discrete Derivative'
 * '<S22>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Yaw (Psi) Control/Embedded MATLAB Function'
 * '<S23>'  : 'PC_Quadcopter_Simulation/Simulation/Controller/Yaw (Psi) Control/Wrap 0 to 2pi'
 * '<S24>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Control Mixing/X Config Control Mixing'
 * '<S25>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Control Mixing/X Config Control Mixing1'
 * '<S26>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Discrete Derivative'
 * '<S27>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/EOM'
 * '<S28>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/External Disturbances'
 * '<S29>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/IC'
 * '<S30>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics'
 * '<S31>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Wrap 0 to 2pi'
 * '<S32>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/quad'
 * '<S33>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Discrete Varying State Space'
 * '<S34>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Discrete Varying State Space1'
 * '<S35>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Discrete Varying State Space2'
 * '<S36>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Discrete Varying State Space3'
 * '<S37>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_1'
 * '<S38>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_2'
 * '<S39>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_3'
 * '<S40>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_4'
 * '<S41>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_1/Minimum Throttle  Cutoff'
 * '<S42>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_2/Minimum Throttle  Cutoff'
 * '<S43>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_3/Minimum Throttle  Cutoff'
 * '<S44>'  : 'PC_Quadcopter_Simulation/Simulation/Quadcopter Dynamics/Motor Dynamics/Throttle To RPM Gain and Cutoff_4/Minimum Throttle  Cutoff'
 */
#endif                                 /* RTW_HEADER_PC_Quadcopter_Simulation_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
