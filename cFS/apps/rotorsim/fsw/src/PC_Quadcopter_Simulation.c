/*
 * File: PC_Quadcopter_Simulation.c
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

/* Block states (default storage) */
DW_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_DW;

/* External inputs (root inport signals with default storage) */
ExtU_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_PC_Quadcopter_Simulation_T PC_Quadcopter_Simulation_Y;

/* Real-time model */
RT_MODEL_PC_Quadcopter_Simula_T PC_Quadcopter_Simulation_M_;
RT_MODEL_PC_Quadcopter_Simula_T *const PC_Quadcopter_Simulation_M =
  &PC_Quadcopter_Simulation_M_;
real_T rt_modd_snf(real_T u0, real_T u1)
{
  real_T y;
  boolean_T yEq;
  real_T q;
  y = u0;
  if (!((!rtIsNaN(u0)) && (!rtIsInf(u0)) && ((!rtIsNaN(u1)) && (!rtIsInf(u1)))))
  {
    if (u1 != 0.0) {
      y = (rtNaN);
    }
  } else if (u0 == 0.0) {
    y = u1 * 0.0;
  } else {
    if (u1 != 0.0) {
      y = fmod(u0, u1);
      yEq = (y == 0.0);
      if ((!yEq) && (u1 > floor(u1))) {
        q = fabs(u0 / u1);
        yEq = (fabs(q - floor(q + 0.5)) <= DBL_EPSILON * q);
      }

      if (yEq) {
        y = u1 * 0.0;
      } else {
        if ((u0 < 0.0) != (u1 < 0.0)) {
          y += u1;
        }
      }
    }
  }

  return y;
}

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  int32_T u0_0;
  int32_T u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      u0_0 = 1;
    } else {
      u0_0 = -1;
    }

    if (u1 > 0.0) {
      u1_0 = 1;
    } else {
      u1_0 = -1;
    }

    y = atan2(u0_0, u1_0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

/* Model step function */
void PC_Quadcopter_Simulation_step(void)
{
  real_T y;
  real_T x;
  real_T Attdot[3];
  real_T Rbi[9];
  real_T vb[3];
  real_T b_dv[3];
  real_T b_z1[4];
  int32_T c_k;
  real_T rtb_uvwcommand[3];
  real_T rtb_Product_n;
  real_T rtb_The_cmd;
  real_T rtb_The_cmd_e;
  real_T rtb_Divide[4];
  real_T rtb_Add;
  real_T rtb_VectorConcatenate[9];
  real_T rtb_VelocityError;
  real_T rtb_VelocityError_k;
  real_T rtb_TSamp_g;
  real_T rtb_TSamp_o;
  real_T rtb_Error;
  real_T rtb_TSamp_l;
  real_T rtb_Du;
  real_T rtb_Error_e;
  real_T rtb_Cx;
  real_T rtb_xk_k;
  real_T rtb_Error_h;
  real_T rtb_Signal_Saturation_1;
  real_T rtb_Ax;
  real_T rtb_Product;
  real_T rtb_Bu_e;
  real_T rtb_Product_f;
  real_T rtb_Add1;
  real_T rtb_Sum1_a;
  real_T rtb_Sum1_c;
  real_T rtb_Sum1_b;
  real_T rtb_sincos_o2[3];
  int32_T i;
  real_T tmp[9];
  real_T tmp_0[3];
  real_T tmp_1[12];
  real_T rtb_TSamp_idx_0;
  real_T rtb_TSamp_idx_1;
  real_T rtb_TSamp_idx_2;
  int32_T tmp_2;

  /* Sum: '<S31>/Add1' incorporates:
   *  Constant: '<S31>/Range'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *  Math: '<S31>/Math Function'
   */
  rtb_Add1 = rt_modd_snf
    (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[5],
     6.2831853071795862);

  /* SampleTimeMath: '<S26>/TSamp' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *
   * About '<S26>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_TSamp_idx_0 = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[9]
    * 20.0;
  rtb_TSamp_idx_1 = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[10]
    * 20.0;
  rtb_TSamp_idx_2 = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[11]
    * 20.0;

  /* Gain: '<S12>/1//2' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_uvwcommand[0] = 0.5 *
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[3];
  rtb_uvwcommand[1] = 0.5 *
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4];
  rtb_uvwcommand[2] = 0.5 * rtb_Add1;

  /* Trigonometry: '<S12>/sincos' */
  rtb_sincos_o2[0] = cos(rtb_uvwcommand[0]);
  rtb_uvwcommand[0] = sin(rtb_uvwcommand[0]);
  rtb_sincos_o2[1] = cos(rtb_uvwcommand[1]);
  rtb_uvwcommand[1] = sin(rtb_uvwcommand[1]);
  rtb_sincos_o2[2] = cos(rtb_uvwcommand[2]);
  rtb_VelocityError = sin(rtb_uvwcommand[2]);

  /* Fcn: '<S12>/q0' incorporates:
   *  Fcn: '<S12>/q3'
   */
  rtb_Add = rtb_sincos_o2[0] * rtb_sincos_o2[1];
  rtb_VelocityError_k = rtb_uvwcommand[0] * rtb_uvwcommand[1];

  /* SignalConversion: '<S13>/TmpSignal ConversionAtMath FunctionInport1' incorporates:
   *  Fcn: '<S12>/q0'
   */
  rtb_Divide[0] = rtb_Add * rtb_sincos_o2[2] - rtb_VelocityError_k *
    rtb_VelocityError;

  /* Fcn: '<S12>/q1' incorporates:
   *  Fcn: '<S12>/q2'
   */
  rtb_TSamp_g = rtb_sincos_o2[0] * rtb_uvwcommand[1];
  rtb_TSamp_o = rtb_uvwcommand[0] * rtb_sincos_o2[1];

  /* SignalConversion: '<S13>/TmpSignal ConversionAtMath FunctionInport1' incorporates:
   *  Fcn: '<S12>/q1'
   *  Fcn: '<S12>/q2'
   *  Fcn: '<S12>/q3'
   */
  rtb_Divide[1] = rtb_TSamp_g * rtb_VelocityError + rtb_TSamp_o * rtb_sincos_o2
    [2];
  rtb_Divide[2] = rtb_TSamp_g * rtb_sincos_o2[2] - rtb_TSamp_o *
    rtb_VelocityError;
  rtb_Divide[3] = rtb_Add * rtb_VelocityError + rtb_VelocityError_k *
    rtb_sincos_o2[2];

  /* Sqrt: '<S13>/Sqrt' incorporates:
   *  Math: '<S13>/Math Function'
   *  Sum: '<S13>/Sum of Elements'
   */
  rtb_The_cmd_e = sqrt(((rtb_Divide[0] * rtb_Divide[0] + rtb_Divide[1] *
    rtb_Divide[1]) + rtb_Divide[2] * rtb_Divide[2]) + rtb_Divide[3] *
                       rtb_Divide[3]);

  /* Product: '<S11>/Divide' */
  rtb_Divide[0] /= rtb_The_cmd_e;
  rtb_Divide[1] /= rtb_The_cmd_e;
  rtb_Divide[2] /= rtb_The_cmd_e;
  rtb_VelocityError = rtb_Divide[3] / rtb_The_cmd_e;
  rtb_Divide[3] = rtb_VelocityError;

  /* Sum: '<S11>/Add' incorporates:
   *  DotProduct: '<S11>/Dot Product'
   *  Math: '<S11>/Math Function1'
   */
  rtb_Add = rtb_Divide[0] * rtb_Divide[0] - ((rtb_Divide[1] * rtb_Divide[1] +
    rtb_Divide[2] * rtb_Divide[2]) + rtb_VelocityError * rtb_VelocityError);

  /* SignalConversion: '<S14>/ConcatBufferAtVector ConcatenateIn1' incorporates:
   *  Constant: '<S14>/Zero '
   *  Gain: '<S14>/Gain'
   */
  rtb_VectorConcatenate[0] = 0.0;
  rtb_VectorConcatenate[1] = rtb_VelocityError;
  rtb_VectorConcatenate[2] = -rtb_Divide[2];

  /* SignalConversion: '<S14>/ConcatBufferAtVector ConcatenateIn2' incorporates:
   *  Constant: '<S14>/Zero '
   *  Gain: '<S14>/Gain1'
   */
  rtb_VectorConcatenate[3] = -rtb_VelocityError;
  rtb_VectorConcatenate[4] = 0.0;
  rtb_VectorConcatenate[5] = rtb_Divide[1];

  /* SignalConversion: '<S14>/ConcatBufferAtVector ConcatenateIn3' incorporates:
   *  Constant: '<S14>/Zero '
   *  Gain: '<S14>/Gain2'
   */
  rtb_VectorConcatenate[6] = rtb_Divide[2];
  rtb_VectorConcatenate[7] = -rtb_Divide[1];
  rtb_VectorConcatenate[8] = 0.0;

  /* Product: '<S11>/Product1' incorporates:
   *  Gain: '<S11>/Gain1'
   *  Math: '<S11>/Math Function3'
   *  SignalConversion: '<S11>/ConcatBufferAtVector ConcatenateIn1'
   */
  for (i = 0; i < 3; i++) {
    Rbi[i] = rtb_Divide[1 + i] * rtb_Divide[1];
    Rbi[i + 3] = rtb_Divide[1 + i] * rtb_Divide[2];
    Rbi[i + 6] = rtb_Divide[1 + i] * rtb_VelocityError;
  }

  /* End of Product: '<S11>/Product1' */

  /* Product: '<S11>/Product3' */
  rtb_VelocityError = rtb_Divide[0];

  /* Sum: '<S11>/Add1' incorporates:
   *  Constant: '<S11>/Constant'
   *  Gain: '<S11>/Gain1'
   *  Gain: '<S11>/Gain2'
   *  Product: '<S11>/Product2'
   *  Product: '<S11>/Product3'
   *  Product: '<S2>/Product'
   */
  for (i = 0; i < 3; i++) {
    tmp[3 * i] = (PC_Quadcopter_Simulation_ConstP.Constant_Value[3 * i] *
                  rtb_Add + Rbi[3 * i] * 2.0) - rtb_VectorConcatenate[3 * i] *
      2.0 * rtb_VelocityError;

    /* Product: '<S2>/Product' incorporates:
     *  Constant: '<S11>/Constant'
     *  Gain: '<S11>/Gain1'
     *  Gain: '<S11>/Gain2'
     *  Product: '<S11>/Product2'
     *  Product: '<S11>/Product3'
     */
    tmp_2 = 3 * i + 1;
    tmp[1 + 3 * i] = (PC_Quadcopter_Simulation_ConstP.Constant_Value[tmp_2] *
                      rtb_Add + Rbi[tmp_2] * 2.0) - rtb_VectorConcatenate[tmp_2]
      * 2.0 * rtb_VelocityError;

    /* Product: '<S2>/Product' incorporates:
     *  Constant: '<S11>/Constant'
     *  Gain: '<S11>/Gain1'
     *  Gain: '<S11>/Gain2'
     *  Product: '<S11>/Product2'
     *  Product: '<S11>/Product3'
     */
    tmp_2 = 3 * i + 2;
    tmp[2 + 3 * i] = (PC_Quadcopter_Simulation_ConstP.Constant_Value[tmp_2] *
                      rtb_Add + Rbi[tmp_2] * 2.0) - rtb_VectorConcatenate[tmp_2]
      * 2.0 * rtb_VelocityError;
  }

  /* End of Sum: '<S11>/Add1' */

  /* Product: '<S2>/Product' incorporates:
   *  Inport: '<Root>/Xd_I'
   *  Inport: '<Root>/Yd_I'
   *  Inport: '<Root>/Zd_I'
   */
  for (i = 0; i < 3; i++) {
    rtb_uvwcommand[i] = tmp[i + 6] * PC_Quadcopter_Simulation_U.Zd_I + (tmp[i +
      3] * PC_Quadcopter_Simulation_U.Yd_I + tmp[i] *
      PC_Quadcopter_Simulation_U.Xd_I);
  }

  /* Sum: '<S6>/Sum3' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_VelocityError = rtb_uvwcommand[1] -
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[7];

  /* SampleTimeMath: '<S10>/TSamp' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *
   * About '<S10>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_Add = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[7] * 20.0;

  /* Sum: '<S6>/Sum6' incorporates:
   *  DiscreteIntegrator: '<S6>/Discrete-Time Integrator'
   *  Gain: '<S6>/Kd'
   *  Gain: '<S6>/Ki'
   *  Gain: '<S6>/Ki1'
   *  Sum: '<S10>/Diff'
   *  UnitDelay: '<S10>/UD'
   *
   * Block description for '<S10>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S10>/UD':
   *
   *  Store in Global RAM
   */
  rtb_The_cmd_e = (0.05 *
                   PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i +
                   0.16 * rtb_VelocityError) - (rtb_Add -
    PC_Quadcopter_Simulation_DW.UD_DSTATE_m) * 0.07;

  /* Saturate: '<S6>/Attitude Limits_Phi' */
  if (rtb_The_cmd_e > 0.43633231299858238) {
    rtb_The_cmd_e = 0.43633231299858238;
  } else {
    if (rtb_The_cmd_e < -0.43633231299858238) {
      rtb_The_cmd_e = -0.43633231299858238;
    }
  }

  /* End of Saturate: '<S6>/Attitude Limits_Phi' */

  /* Sum: '<S9>/Sum3' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_VelocityError_k = rtb_uvwcommand[0] -
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[6];

  /* SampleTimeMath: '<S15>/TSamp' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *
   * About '<S15>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_TSamp_g = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[6] *
    20.0;

  /* Gain: '<S9>/Gain' incorporates:
   *  DiscreteIntegrator: '<S9>/Discrete-Time Integrator'
   *  Gain: '<S9>/Kd'
   *  Gain: '<S9>/Ki'
   *  Gain: '<S9>/Ki1'
   *  Sum: '<S15>/Diff'
   *  Sum: '<S9>/Sum6'
   *  UnitDelay: '<S15>/UD'
   *
   * Block description for '<S15>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S15>/UD':
   *
   *  Store in Global RAM
   */
  rtb_The_cmd = -((0.05 *
                   PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c +
                   0.16 * rtb_VelocityError_k) - (rtb_TSamp_g -
    PC_Quadcopter_Simulation_DW.UD_DSTATE_e) * 0.07);

  /* Saturate: '<S9>/Attitude Limits_Theta' */
  if (rtb_The_cmd > 0.43633231299858238) {
    rtb_The_cmd = 0.43633231299858238;
  } else {
    if (rtb_The_cmd < -0.43633231299858238) {
      rtb_The_cmd = -0.43633231299858238;
    }
  }

  /* End of Saturate: '<S9>/Attitude Limits_Theta' */

  /* SampleTimeMath: '<S17>/TSamp' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *
   * About '<S17>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_TSamp_o = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[11] *
    20.0;

  /* Sum: '<S17>/Diff' incorporates:
   *  UnitDelay: '<S17>/UD'
   *
   * Block description for '<S17>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S17>/UD':
   *
   *  Store in Global RAM
   */
  rtb_Product_n = rtb_TSamp_o - PC_Quadcopter_Simulation_DW.UD_DSTATE_c;

  /* Sum: '<S16>/Sum' incorporates:
   *  Inport: '<Root>/Zd_I'
   */
  rtb_Error = PC_Quadcopter_Simulation_U.Zd_I - rtb_Product_n;

  /* SampleTimeMath: '<S21>/TSamp'
   *
   * About '<S21>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_TSamp_l = rtb_Product_n * 20.0;

  /* Sum: '<S16>/Sum1' incorporates:
   *  Constant: '<S16>/Gravity Offset (Level Flight)'
   *  DiscreteIntegrator: '<S16>/Discrete-Time Integrator'
   *  Gain: '<S16>/Kdz'
   *  Gain: '<S16>/Kiz'
   *  Gain: '<S16>/Kpz'
   *  Sum: '<S16>/Sum2'
   *  Sum: '<S21>/Diff'
   *  UnitDelay: '<S21>/UD'
   *
   * Block description for '<S21>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S21>/UD':
   *
   *  Store in Global RAM
   */
  rtb_Product_n = ((2.0 * rtb_Error + 1.1 *
                    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m)
                   - (rtb_TSamp_l - PC_Quadcopter_Simulation_DW.UD_DSTATE_j) *
                   1.2) - 38.867249311525107;

  /* MATLAB Function: '<S20>/Embedded MATLAB Function' */
  x = cos(PC_Quadcopter_Simulation_ConstB.Add1 / 2.0);
  rtb_Error_e = sin(PC_Quadcopter_Simulation_ConstB.Add1 / 2.0);
  y = cos(rtb_Add1 / 2.0) * rtb_Error_e - x * sin(rtb_Add1 / 2.0);
  x = cos(rtb_Add1 / 2.0) * x + rtb_Error_e * sin(rtb_Add1 / 2.0);
  if (fabs(x) > 1.0E-12) {
    if (x < 0.0) {
      rtb_Error_e = -1.0;
    } else if (x > 0.0) {
      rtb_Error_e = 1.0;
    } else if (x == 0.0) {
      rtb_Error_e = 0.0;
    } else {
      rtb_Error_e = (rtNaN);
    }

    y *= rtb_Error_e;
  }

  x = fabs(x);
  x = 2.0 * rt_atan2d_snf(y, x);

  /* End of MATLAB Function: '<S20>/Embedded MATLAB Function' */

  /* RateLimiter: '<S20>/Rate Limiter' */
  rtb_Error_e = x - PC_Quadcopter_Simulation_DW.PrevY;
  if (rtb_Error_e > 0.013089969389957471) {
    x = PC_Quadcopter_Simulation_DW.PrevY + 0.013089969389957471;
  } else {
    if (rtb_Error_e < -0.013089969389957471) {
      x = PC_Quadcopter_Simulation_DW.PrevY + -0.013089969389957471;
    }
  }

  PC_Quadcopter_Simulation_DW.PrevY = x;

  /* End of RateLimiter: '<S20>/Rate Limiter' */

  /* Sum: '<S20>/Sum1' incorporates:
   *  DiscreteIntegrator: '<S20>/Discrete-Time Integrator'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *  Gain: '<S20>/Gain'
   *  Gain: '<S20>/Kit'
   */
  rtb_Du = (0.0 * PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d +
            x) - 4.0 *
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[2];

  /* Sum: '<S24>/Sum2' */
  rtb_xk_k = rtb_Du - rtb_Product_n;

  /* Sum: '<S19>/Sum' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_Error_e = rtb_The_cmd_e -
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[3];

  /* Sum: '<S19>/Sum1' incorporates:
   *  DiscreteIntegrator: '<S19>/Discrete-Time Integrator'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *  Gain: '<S19>/Kdp'
   *  Gain: '<S19>/Kip'
   *  Gain: '<S19>/Kpp'
   */
  rtb_Cx = (2.0 * rtb_Error_e + 1.1 *
            PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a) - 1.2 *
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[0];

  /* Sum: '<S18>/Sum' incorporates:
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_Error_h = rtb_The_cmd -
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4];

  /* Sum: '<S18>/Sum1' incorporates:
   *  DiscreteIntegrator: '<S18>/Discrete-Time Integrator'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *  Gain: '<S18>/Kdt'
   *  Gain: '<S18>/Kit'
   *  Gain: '<S18>/Kpt'
   */
  y = (2.0 * rtb_Error_h + 1.1 *
       PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e) - 1.2 *
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[1];

  /* Sum: '<S24>/Sum5' incorporates:
   *  Sum: '<S24>/Sum1'
   */
  rtb_Signal_Saturation_1 = (rtb_xk_k - rtb_Cx) + y;

  /* Saturate: '<S30>/Signal_Saturation_1' */
  if (rtb_Signal_Saturation_1 > 100.0) {
    rtb_Signal_Saturation_1 = 100.0;
  } else {
    if (rtb_Signal_Saturation_1 < 0.0) {
      rtb_Signal_Saturation_1 = 0.0;
    }
  }

  /* End of Saturate: '<S30>/Signal_Saturation_1' */

  /* Product: '<S37>/Product' incorporates:
   *  Constant: '<S37>/Constant'
   *  Constant: '<S41>/Constant'
   *  Gain: '<S37>/Gain'
   *  RelationalOperator: '<S41>/Compare'
   *  Sum: '<S37>/Sum1'
   */
  rtb_Product = (80.584 * rtb_Signal_Saturation_1 + 976.2) * (real_T)
    (rtb_Signal_Saturation_1 >= 5.0);

  /* Sum: '<S33>/Sum1' incorporates:
   *  Constant: '<S30>/C'
   *  Constant: '<S30>/D'
   *  Product: '<S33>/Product2'
   *  Product: '<S33>/Product3'
   *  UnitDelay: '<S33>/Unit Delay'
   */
  rtb_Sum1_a = 13.157894736842106 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE
    + 0.0 * rtb_Product;

  /* Sum: '<S24>/Sum6' incorporates:
   *  Sum: '<S24>/Sum8'
   */
  rtb_Ax = (rtb_xk_k + rtb_Cx) - y;

  /* Saturate: '<S30>/Signal_Saturation_2' */
  if (rtb_Ax > 100.0) {
    rtb_Ax = 100.0;
  } else {
    if (rtb_Ax < 0.0) {
      rtb_Ax = 0.0;
    }
  }

  /* End of Saturate: '<S30>/Signal_Saturation_2' */

  /* Product: '<S38>/Product' incorporates:
   *  Constant: '<S38>/Constant'
   *  Constant: '<S42>/Constant'
   *  Gain: '<S38>/Gain'
   *  RelationalOperator: '<S42>/Compare'
   *  Sum: '<S38>/Sum1'
   */
  rtb_xk_k = (80.584 * rtb_Ax + 976.2) * (real_T)(rtb_Ax >= 5.0);

  /* Sum: '<S34>/Sum1' incorporates:
   *  Constant: '<S30>/C'
   *  Constant: '<S30>/D'
   *  Product: '<S34>/Product2'
   *  Product: '<S34>/Product3'
   *  UnitDelay: '<S34>/Unit Delay'
   */
  rtb_Sum1_c = 13.157894736842106 *
    PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_f + 0.0 * rtb_xk_k;

  /* Sum: '<S24>/Sum4' */
  rtb_Du = (0.0 - rtb_Du) - rtb_Product_n;

  /* Sum: '<S24>/Sum' incorporates:
   *  Sum: '<S24>/Sum3'
   */
  rtb_Bu_e = (rtb_Cx + rtb_Du) + y;

  /* Saturate: '<S30>/Signal_Saturation_3' */
  if (rtb_Bu_e > 100.0) {
    rtb_Bu_e = 100.0;
  } else {
    if (rtb_Bu_e < 0.0) {
      rtb_Bu_e = 0.0;
    }
  }

  /* End of Saturate: '<S30>/Signal_Saturation_3' */

  /* Product: '<S39>/Product' incorporates:
   *  Constant: '<S39>/Constant'
   *  Constant: '<S43>/Constant'
   *  Gain: '<S39>/Gain'
   *  RelationalOperator: '<S43>/Compare'
   *  Sum: '<S39>/Sum1'
   */
  rtb_Product_f = (80.584 * rtb_Bu_e + 976.2) * (real_T)(rtb_Bu_e >= 5.0);

  /* Sum: '<S35>/Sum1' incorporates:
   *  Constant: '<S30>/C'
   *  Constant: '<S30>/D'
   *  Product: '<S35>/Product2'
   *  Product: '<S35>/Product3'
   *  UnitDelay: '<S35>/Unit Delay'
   */
  rtb_Sum1_b = 13.157894736842106 *
    PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_h + 0.0 * rtb_Product_f;

  /* Sum: '<S24>/Sum7' incorporates:
   *  Sum: '<S24>/Sum11'
   */
  y = (rtb_Du - rtb_Cx) - y;

  /* Saturate: '<S30>/Signal_Saturation_4' */
  if (y > 100.0) {
    y = 100.0;
  } else {
    if (y < 0.0) {
      y = 0.0;
    }
  }

  /* End of Saturate: '<S30>/Signal_Saturation_4' */

  /* Product: '<S40>/Product' incorporates:
   *  Constant: '<S40>/Constant'
   *  Constant: '<S44>/Constant'
   *  Gain: '<S40>/Gain'
   *  RelationalOperator: '<S44>/Compare'
   *  Sum: '<S40>/Sum1'
   */
  rtb_Product_n = (80.584 * y + 976.2) * (real_T)(y >= 5.0);

  /* Sum: '<S36>/Sum1' incorporates:
   *  Constant: '<S30>/C'
   *  Constant: '<S30>/D'
   *  Product: '<S36>/Product2'
   *  Product: '<S36>/Product3'
   *  UnitDelay: '<S36>/Unit Delay'
   */
  rtb_Du = 13.157894736842106 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_a +
    0.0 * rtb_Product_n;

  /* Outport: '<Root>/yout ' incorporates:
   *  Constant: '<S2>/Constant'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   *  Inport: '<Root>/Xd_I'
   *  Inport: '<Root>/Yd_I'
   *  Inport: '<Root>/Zd_I'
   *  Sum: '<S26>/Diff'
   *  UnitDelay: '<S26>/UD'
   *
   * Block description for '<S26>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S26>/UD':
   *
   *  Store in Global RAM
   */
  for (i = 0; i < 5; i++) {
    PC_Quadcopter_Simulation_Y.yout[i] =
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[i];
  }

  PC_Quadcopter_Simulation_Y.yout[5] = rtb_Add1;
  for (i = 0; i < 6; i++) {
    PC_Quadcopter_Simulation_Y.yout[i + 6] =
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[i + 6];
  }

  PC_Quadcopter_Simulation_Y.yout[12] = rtb_TSamp_idx_0 -
    PC_Quadcopter_Simulation_DW.UD_DSTATE[0];
  PC_Quadcopter_Simulation_Y.yout[13] = rtb_TSamp_idx_1 -
    PC_Quadcopter_Simulation_DW.UD_DSTATE[1];
  PC_Quadcopter_Simulation_Y.yout[14] = rtb_TSamp_idx_2 -
    PC_Quadcopter_Simulation_DW.UD_DSTATE[2];
  PC_Quadcopter_Simulation_Y.yout[15] = rtb_Sum1_a;
  PC_Quadcopter_Simulation_Y.yout[16] = rtb_Sum1_c;
  PC_Quadcopter_Simulation_Y.yout[17] = rtb_Sum1_b;
  PC_Quadcopter_Simulation_Y.yout[18] = rtb_Du;
  PC_Quadcopter_Simulation_Y.yout[19] = rtb_Signal_Saturation_1;
  PC_Quadcopter_Simulation_Y.yout[20] = rtb_Ax;
  PC_Quadcopter_Simulation_Y.yout[21] = rtb_Bu_e;
  PC_Quadcopter_Simulation_Y.yout[22] = y;
  PC_Quadcopter_Simulation_Y.yout[23] = rtb_The_cmd_e;
  PC_Quadcopter_Simulation_Y.yout[24] = rtb_The_cmd;
  PC_Quadcopter_Simulation_Y.yout[25] = 0.0;
  PC_Quadcopter_Simulation_Y.yout[26] = PC_Quadcopter_Simulation_U.Zd_I;
  PC_Quadcopter_Simulation_Y.yout[27] = PC_Quadcopter_Simulation_U.Xd_I;
  PC_Quadcopter_Simulation_Y.yout[28] = PC_Quadcopter_Simulation_U.Yd_I;
  PC_Quadcopter_Simulation_Y.yout[29] = PC_Quadcopter_Simulation_U.Zd_I;

  /* End of Outport: '<Root>/yout ' */

  /* MATLAB Function: '<S5>/EOM' incorporates:
   *  Constant: '<S32>/Jb'
   *  Constant: '<S32>/Jbinv'
   *  Constant: '<S32>/Jm'
   *  Constant: '<S32>/dctdq'
   *  Constant: '<S32>/mass1'
   *  DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
   */
  rtb_The_cmd = rtb_Sum1_a * rtb_Sum1_a;
  b_z1[0] = rtb_Sum1_a * rtb_Sum1_a * 1.4865E-7;
  rtb_Signal_Saturation_1 = rtb_Sum1_c * rtb_Sum1_c;
  b_z1[1] = rtb_Sum1_c * rtb_Sum1_c * 1.4865E-7;
  rtb_Cx = rtb_Sum1_b * rtb_Sum1_b;
  b_z1[2] = rtb_Sum1_b * rtb_Sum1_b * 1.4865E-7;
  rtb_Ax = rtb_Du * rtb_Du;
  b_z1[3] = rtb_Du * rtb_Du * 1.4865E-7;
  y = b_z1[0];
  rtb_uvwcommand[0] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE
    [0];
  rtb_uvwcommand[1] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE
    [1];
  rtb_uvwcommand[2] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE
    [2];
  tmp_0[0] = (((-rtb_Sum1_a - rtb_Sum1_c) + rtb_Sum1_b) + rtb_Du) *
    (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[1] *
     3.7882224860398754E-6);
  tmp_0[1] = (((-rtb_Sum1_a - rtb_Sum1_c) + rtb_Sum1_b) + rtb_Du) *
    (-PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[0] *
     3.7882224860398754E-6);
  tmp_0[2] = 0.0;
  tmp[0] = 0.0;
  tmp[3] = -PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[2];
  tmp[6] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[1];
  tmp[1] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[2];
  tmp[4] = 0.0;
  tmp[7] = -PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[0];
  tmp[2] = -PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[1];
  tmp[5] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[0];
  tmp[8] = 0.0;
  for (c_k = 0; c_k < 3; c_k++) {
    y += b_z1[c_k + 1];
    rtb_Add1 = PC_Quadcopter_Simulation_ConstP.dctdq_Value[c_k + 9] * rtb_Ax +
      (PC_Quadcopter_Simulation_ConstP.dctdq_Value[c_k + 6] * rtb_Cx +
       (PC_Quadcopter_Simulation_ConstP.dctdq_Value[c_k + 3] *
        rtb_Signal_Saturation_1 +
        PC_Quadcopter_Simulation_ConstP.dctdq_Value[c_k] * rtb_The_cmd));
    rtb_The_cmd_e = 0.0;
    for (i = 0; i < 3; i++) {
      tmp_2 = c_k + 3 * i;
      Rbi[tmp_2] = 0.0;
      Rbi[tmp_2] = Rbi[3 * i + c_k] + PC_Quadcopter_Simulation_ConstP.Jb_Value[3
        * i] * tmp[c_k];
      Rbi[tmp_2] = PC_Quadcopter_Simulation_ConstP.Jb_Value[3 * i + 1] * tmp[c_k
        + 3] + Rbi[3 * i + c_k];
      Rbi[tmp_2] = PC_Quadcopter_Simulation_ConstP.Jb_Value[3 * i + 2] * tmp[c_k
        + 6] + Rbi[3 * i + c_k];
      rtb_The_cmd_e += Rbi[3 * i + c_k] * rtb_uvwcommand[i];
    }

    b_dv[c_k] = (rtb_Add1 + tmp_0[c_k]) - rtb_The_cmd_e;
  }

  tmp[0] = 1.0;
  rtb_Add1 = tan(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4]);
  rtb_The_cmd_e = sin(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE
                      [3]);
  tmp[3] = rtb_Add1 * rtb_The_cmd_e;
  rtb_The_cmd = cos(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[3]);
  tmp[6] = rtb_Add1 * rtb_The_cmd;
  tmp[1] = 0.0;
  tmp[4] = rtb_The_cmd;
  tmp[7] = -rtb_The_cmd_e;
  tmp[2] = 0.0;
  rtb_Add1 = cos(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4]);
  tmp[5] = rtb_The_cmd_e / rtb_Add1;
  tmp[8] = rtb_The_cmd / rtb_Add1;
  rtb_Sum1_a = cos(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[5]);
  rtb_VectorConcatenate[0] = rtb_Sum1_a * rtb_Add1;
  rtb_Sum1_c = sin(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4]);
  rtb_Sum1_b = rtb_Sum1_a * rtb_Sum1_c;
  rtb_Du = sin(PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[5]);
  rtb_VectorConcatenate[3] = rtb_Sum1_b * rtb_The_cmd_e - rtb_Du * rtb_The_cmd;
  rtb_VectorConcatenate[6] = rtb_Sum1_b * rtb_The_cmd + rtb_Du * rtb_The_cmd_e;
  rtb_VectorConcatenate[1] = rtb_Du * rtb_Add1;
  rtb_Sum1_b = rtb_Du * rtb_Sum1_c;
  rtb_VectorConcatenate[4] = rtb_Sum1_b * rtb_The_cmd_e + rtb_Sum1_a *
    rtb_The_cmd;
  rtb_VectorConcatenate[7] = rtb_Sum1_b * rtb_The_cmd - rtb_Sum1_a *
    rtb_The_cmd_e;
  rtb_VectorConcatenate[2] = -rtb_Sum1_c;
  rtb_VectorConcatenate[5] = rtb_Add1 * rtb_The_cmd_e;
  rtb_VectorConcatenate[8] = rtb_Add1 * rtb_The_cmd;
  for (i = 0; i < 3; i++) {
    Rbi[3 * i] = rtb_VectorConcatenate[i];
    Rbi[1 + 3 * i] = rtb_VectorConcatenate[i + 3];
    Rbi[2 + 3 * i] = rtb_VectorConcatenate[i + 6];
    rtb_sincos_o2[i] = PC_Quadcopter_Simulation_ConstP.Jbinv_Value[i + 6] *
      b_dv[2] + (PC_Quadcopter_Simulation_ConstP.Jbinv_Value[i + 3] * b_dv[1] +
                 PC_Quadcopter_Simulation_ConstP.Jbinv_Value[i] * b_dv[0]);
    Attdot[i] = tmp[i + 6] * rtb_uvwcommand[2] + (tmp[i + 3] * rtb_uvwcommand[1]
      + tmp[i] * rtb_uvwcommand[0]);
  }

  vb[0] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[6];
  vb[1] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[7];
  vb[2] = PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[8];
  tmp_0[0] = 0.0;
  tmp_0[1] = 0.0;
  tmp_0[2] = -y;
  for (i = 0; i < 3; i++) {
    rtb_Add1 = Rbi[i + 3] * 0.0;
    b_dv[i] = (((rtb_Add1 + Rbi[i] * 0.0) + Rbi[i + 6] * 9.81) + (tmp_0[i] -
                0.017079677275431294 * (vb[i] * vb[i])) * 0.97751710654936474) +
      (Rbi[i + 6] * 0.0 + (rtb_Add1 + Rbi[i] * 0.0));
    rtb_uvwcommand[i] = rtb_VectorConcatenate[i + 6] * vb[2] +
      (rtb_VectorConcatenate[i + 3] * vb[1] + rtb_VectorConcatenate[i] * vb[0]);
  }

  /* Sum: '<S36>/Sum' incorporates:
   *  Constant: '<S30>/A'
   *  Constant: '<S30>/B'
   *  Product: '<S36>/Product'
   *  Product: '<S36>/Product1'
   *  UnitDelay: '<S36>/Unit Delay'
   */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_a = 0.05 * rtb_Product_n +
    0.34210526315789469 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_a;

  /* Sum: '<S33>/Sum' incorporates:
   *  Constant: '<S30>/A'
   *  Constant: '<S30>/B'
   *  Product: '<S33>/Product'
   *  Product: '<S33>/Product1'
   *  UnitDelay: '<S33>/Unit Delay'
   */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE = 0.05 * rtb_Product +
    0.34210526315789469 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE;

  /* Sum: '<S34>/Sum' incorporates:
   *  Constant: '<S30>/A'
   *  Constant: '<S30>/B'
   *  Product: '<S34>/Product'
   *  Product: '<S34>/Product1'
   *  UnitDelay: '<S34>/Unit Delay'
   */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_f = 0.05 * rtb_xk_k +
    0.34210526315789469 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_f;

  /* Sum: '<S35>/Sum' incorporates:
   *  Constant: '<S30>/A'
   *  Constant: '<S30>/B'
   *  Product: '<S35>/Product'
   *  Product: '<S35>/Product1'
   *  UnitDelay: '<S35>/Unit Delay'
   */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_h = 0.05 * rtb_Product_f +
    0.34210526315789469 * PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_h;

  /* Update for DiscreteIntegrator: '<S5>/Discrete-Time Integrator' incorporates:
   *  MATLAB Function: '<S5>/EOM'
   */
  tmp_1[0] = 0.05 * rtb_sincos_o2[0];
  tmp_1[1] = 0.05 * rtb_sincos_o2[1];
  tmp_1[2] = 0.05 * rtb_sincos_o2[2];
  tmp_1[3] = 0.05 * Attdot[0];
  tmp_1[4] = 0.05 * Attdot[1];
  tmp_1[5] = 0.05 * Attdot[2];
  tmp_1[6] = 0.05 * b_dv[0];
  tmp_1[7] = 0.05 * b_dv[1];
  tmp_1[8] = 0.05 * b_dv[2];
  tmp_1[9] = 0.05 * rtb_uvwcommand[0];
  tmp_1[10] = 0.05 * rtb_uvwcommand[1];
  tmp_1[11] = 0.05 * rtb_uvwcommand[2];
  for (i = 0; i < 12; i++) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[i] += tmp_1[i];
  }

  /* End of Update for DiscreteIntegrator: '<S5>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S26>/UD'
   *
   * Block description for '<S26>/UD':
   *
   *  Store in Global RAM
   */
  PC_Quadcopter_Simulation_DW.UD_DSTATE[0] = rtb_TSamp_idx_0;
  PC_Quadcopter_Simulation_DW.UD_DSTATE[1] = rtb_TSamp_idx_1;
  PC_Quadcopter_Simulation_DW.UD_DSTATE[2] = rtb_TSamp_idx_2;

  /* Update for DiscreteIntegrator: '<S6>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i += 0.05 *
    rtb_VelocityError;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i >= 10.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i = 10.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i <= -10.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_i = -10.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S6>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S10>/UD'
   *
   * Block description for '<S10>/UD':
   *
   *  Store in Global RAM
   */
  PC_Quadcopter_Simulation_DW.UD_DSTATE_m = rtb_Add;

  /* Update for DiscreteIntegrator: '<S9>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c += 0.05 *
    rtb_VelocityError_k;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c >= 10.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c = 10.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c <= -10.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_c = -10.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S9>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S15>/UD'
   *
   * Block description for '<S15>/UD':
   *
   *  Store in Global RAM
   */
  PC_Quadcopter_Simulation_DW.UD_DSTATE_e = rtb_TSamp_g;

  /* Update for UnitDelay: '<S17>/UD'
   *
   * Block description for '<S17>/UD':
   *
   *  Store in Global RAM
   */
  PC_Quadcopter_Simulation_DW.UD_DSTATE_c = rtb_TSamp_o;

  /* Update for DiscreteIntegrator: '<S16>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m += 0.05 *
    rtb_Error;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m >= 10.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m = 10.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m <= -10.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_m = -10.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S16>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S21>/UD'
   *
   * Block description for '<S21>/UD':
   *
   *  Store in Global RAM
   */
  PC_Quadcopter_Simulation_DW.UD_DSTATE_j = rtb_TSamp_l;

  /* Update for DiscreteIntegrator: '<S20>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d += 0.05 * x;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d >= 10.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d = 10.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d <= -10.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_d = -10.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S20>/Discrete-Time Integrator' */

  /* Update for DiscreteIntegrator: '<S19>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a += 0.05 *
    rtb_Error_e;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a >= 1.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a = 1.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a <= -1.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_a = -1.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S19>/Discrete-Time Integrator' */

  /* Update for DiscreteIntegrator: '<S18>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e += 0.05 *
    rtb_Error_h;
  if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e >= 1.0) {
    PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e = 1.0;
  } else {
    if (PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e <= -1.0) {
      PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE_e = -1.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S18>/Discrete-Time Integrator' */

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The resolution of this integer timer is 0.05, which is the step size
   * of the task. Size of "clockTick0" ensures timer will not overflow during the
   * application lifespan selected.
   */
  PC_Quadcopter_Simulation_M->Timing.clockTick0++;
}

/* Model initialize function */
void PC_Quadcopter_Simulation_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)PC_Quadcopter_Simulation_M, 0,
                sizeof(RT_MODEL_PC_Quadcopter_Simula_T));

  /* states (dwork) */
  (void) memset((void *)&PC_Quadcopter_Simulation_DW, 0,
                sizeof(DW_PC_Quadcopter_Simulation_T));

  /* external inputs */
  (void)memset((void *)&PC_Quadcopter_Simulation_U, 0, sizeof
               (ExtU_PC_Quadcopter_Simulation_T));

  /* external outputs */
  (void) memset(&PC_Quadcopter_Simulation_Y.yout[0], 0,
                30U*sizeof(real_T));

  /* InitializeConditions for DiscreteIntegrator: '<S5>/Discrete-Time Integrator' */
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[0] =
    PC_Quadcopter_Simulation_ConstB.P;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[1] =
    PC_Quadcopter_Simulation_ConstB.Q;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[2] =
    PC_Quadcopter_Simulation_ConstB.R;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[3] =
    PC_Quadcopter_Simulation_ConstB.Phi;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[4] =
    PC_Quadcopter_Simulation_ConstB.The;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[5] =
    PC_Quadcopter_Simulation_ConstB.Psi;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[6] =
    PC_Quadcopter_Simulation_ConstB.U;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[7] =
    PC_Quadcopter_Simulation_ConstB.V;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[8] =
    PC_Quadcopter_Simulation_ConstB.W;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[9] =
    PC_Quadcopter_Simulation_ConstB.X;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[10] =
    PC_Quadcopter_Simulation_ConstB.Y;
  PC_Quadcopter_Simulation_DW.DiscreteTimeIntegrator_DSTATE[11] =
    PC_Quadcopter_Simulation_ConstB.Z;

  /* InitializeConditions for RateLimiter: '<S20>/Rate Limiter' */
  PC_Quadcopter_Simulation_DW.PrevY = 0.0;

  /* InitializeConditions for UnitDelay: '<S33>/Unit Delay' */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE = 312.20799999999997;

  /* InitializeConditions for UnitDelay: '<S34>/Unit Delay' */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_f = 312.20799999999997;

  /* InitializeConditions for UnitDelay: '<S35>/Unit Delay' */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_h = 312.20799999999997;

  /* InitializeConditions for UnitDelay: '<S36>/Unit Delay' */
  PC_Quadcopter_Simulation_DW.UnitDelay_DSTATE_a = 312.20799999999997;
}

/* Model terminate function */
void PC_Quadcopter_Simulation_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
