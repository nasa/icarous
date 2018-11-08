/*
 * File: PC_Quadcopter_Simulation_data.c
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

#include "PC_Quadcopter_Simulation.h"
#include "PC_Quadcopter_Simulation_private.h"

/* Invariant block signals (default storage) */
const ConstB_PC_Quadcopter_Simulati_T PC_Quadcopter_Simulation_ConstB = {
  0.0,                                 /* '<S29>/P' */
  0.0,                                 /* '<S29>/Q' */
  0.0,                                 /* '<S29>/R' */
  0.0,                                 /* '<S29>/Phi ' */
  0.0,                                 /* '<S29>/The' */
  0.0,                                 /* '<S29>/Psi' */
  0.0,                                 /* '<S29>/U' */
  0.0,                                 /* '<S29>/V' */
  0.0,                                 /* '<S29>/W' */
  0.0,                                 /* '<S29>/X' */
  0.0,                                 /* '<S29>/Y' */
  0.0,                                 /* '<S29>/Z' */
  0.0,                                 /* '<S23>/Add' */
  0.0,                                 /* '<S23>/Math Function' */
  0.0                                  /* '<S23>/Add1' */
};

/* Constant parameters (default storage) */
const ConstP_PC_Quadcopter_Simulati_T PC_Quadcopter_Simulation_ConstP = {
  /* Expression: quadModel.dctcq
   * Referenced by: '<S32>/dctdq'
   */
  { -2.3361013766946271E-8, 2.3361013766946271E-8, 2.925E-9,
    2.3361013766946271E-8, -2.3361013766946271E-8, 2.925E-9,
    2.3361013766946271E-8, 2.3361013766946271E-8, -2.925E-9,
    -2.3361013766946271E-8, -2.3361013766946271E-8, -2.925E-9 },

  /* Expression: eye(3)
   * Referenced by: '<S11>/Constant'
   */
  { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 },

  /* Expression: quadModel.Jbinv
   * Referenced by: '<S32>/Jbinv'
   */
  { 105.26426593964146, 0.0, 0.0, 0.0, 105.26426593964146, 0.0, 0.0, 0.0,
    53.832902670111977 },

  /* Expression: quadModel.Jb
   * Referenced by: '<S32>/Jb'
   */
  { 0.0094999, 0.0, 0.0, 0.0, 0.0094999, 0.0, 0.0, 0.0, 0.018576 }
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
