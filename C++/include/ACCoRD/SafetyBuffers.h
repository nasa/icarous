/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef SAFETYBUFFERS_H_
#define SAFETYBUFFERS_H_

#include "Vect2.h"
#include "Position.h"
#include "Velocity.h"

namespace larcfm {

/**
 * SafetyBuffers class.  This computes the psi value (increase in safety buffer size) needed to preserve a given certainty for
 * detection readings, based on relative distance and speed, and data accuracy values.
 * 
 * In general, these functions will fail (give a negative value) if invalid NIC/NAC values are given, or if a NIC/NAC 
 * corresponds to an unknown accuracy category for that dimension.
 * 
 * Calculations are based on A. Narkawicz, C. Munoz, H. Herencia-Zapana, G. Hagen. "Formal Verification of Safety Buffers for State-Based Conflict Detection."
 * 
 */
class SafetyBuffers {
  
public:

/**
 *    Calculate the increase of horizontal safety buffer size based on NACp values
 * @param relHDist Relative horizontal distance between ownship and intruder at last good reading (m)
 * @param relGS Relative ground speed between ownship and intruder at last good reading (m/s)
 * @param NACpOwn Ownship NACp (1-11)
 * @param NACvOwn Ownship NACv (1-4)
 * @param NACpIntr Intruder NACp (1-11)
 * @param NACvIntr Intruder NACv (1-4)
 * @param lambda Time since last good reading (s)
 * @param T Lookahead time (s)
 * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
 */
  static double psiHorizNAC(double relHDist, double relGS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T);

  /**
   *    Calculate the increase of horizontal safety buffer size based on NIC values
   * @param relHDist Relative horizontal distance between ownship and intruder at last good reading (m)
   * @param relGS Relative ground speed between ownship and intruder at last good reading (m/s)
   * @param NACpOwn Ownship NIC (1-11)
   * @param NACvOwn Ownship NACv (1-4)
   * @param NACpIntr Intruder NIC (1-11)
   * @param NACvIntr Intruder NACv (1-4)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
  static double psiHorizNIC(double relHDist, double relGS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T);

  /**
   *    Calculate the increase of vertical safety buffer size based on NACp values.
   * @param relVDist Relative vertical distance between ownship and intruder at last good reading (m)
   * @param relVS Relative vertical speed between ownship and intruder at last good reading (m/s)
   * @param NACpOwn Ownship NACp (9-11)
   * @param NACvOwn Ownship NACv (1-4)
   * @param NACpIntr Intruder NACp (9-11)
   * @param NACvIntr Intruder NACv (1-4)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
  static double psiVertNAC(double relVDist, double relVS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T);

  /**
   *    Calculate the increase of vertical safety buffer size based on NIC values.
   * @param relVDist Relative vertical distance between ownship and intruder at last good reading (m)
   * @param relVS Relative vertical speed between ownship and intruder at last good reading (m/s)
   * @param NACpOwn Ownship NIC (9-11)
   * @param NACvOwn Ownship NACv (1-4)
   * @param NACpIntr Intruder NIC (9-11)
   * @param NACvIntr Intruder NACv (1-4)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
  static double psiVertNIC(double relVDist, double relVS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T);
  
  /**
   * Calculate the increase in a safety buffer in either vertical or horizontal that preserves a given detection accuracy, based on arbitrary
   * measurement accuracy bounds.  The overall accuracy is 4-(p0+p1+p2+p3).  This uses horizontal distance and ground speed or vertical 
   * distance and vertical speed.
   * @param relDist The relative distance between ownship and intruder, at the last good reading (m)
   * @param relV The relative speed between ownship and intruder, at time of last good reading (m/s)
   * @param ao accuracy of ownship position information: the reported value is within this distance of the true value with a probability of p0 (m)
   * @param ai accuracy of intruder position information: the reported value is within this distance of the true value with a probability of p1 (m)
   * @param bo accuracy of ownship velocity information: the reported value is within this range of the true value with a probability of p2 (m/s)
   * @param bi accuracy of intruder velocity information: the reported value is within this range of the true value with a probability of p3 (m/s)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
  static double psi(double relDist, double relV, double ao, double ai, double bo, double bi, double lambda, double T);
 
  /**
   * Calculate the increase in a safety buffer in either vertical or horizontal that preserves a given detection accuracy, based on arbitrary
   * measurement accuracy bounds.  The overall accuracy is 4-(p0+p1+p2+p3).  This uses horizontal distance and ground speed or vertical 
   * distance and vertical speed.
   * @param sm The relative horizontal Euclidean positions between ownship and intruder, at the last good reading (m)
   * @param vm The relative horizontal Euclidean velocities between ownship and intruder, at time of last good reading (m/s)
   * @param ao accuracy of ownship position information: the reported value is within this distance of the true value with a probability of p0 (m)
   * @param ai accuracy of intruder position information: the reported value is within this distance of the true value with a probability of p1 (m)
   * @param bo accuracy of ownship velocity information: the reported value is within this range of the true value with a probability of p2 (m/s)
   * @param bi accuracy of intruder velocity information: the reported value is within this range of the true value with a probability of p3 (m/s)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
  static double psiH(Vect2 sm, Vect2 vm, double ao, double ai, double bo, double bi, double lambda, double T);


  /**
   * Calculate the increase of horizontal safety buffer size based on NACp values.  If positions are in lat/lon,
   * this uses the projection defined in Projection.getProjection, with the ownship as the origin.  This version does not
   * include inaccuracies resulting from the projection.
   * Be sure to call Projection.   
   * @param po position of ownship
   * @param pi position of intruder
   * @param vo velocity of ownship
   * @param vi velocity of intruder
   * @param relGS Relative ground speed between ownship and intruder at last good reading (m/s)
   * @param NACpOwn Ownship NACp (1-11)
   * @param NACvOwn Ownship NACv (1-4)
   * @param NACpIntr Intruder NACp (1-11)
   * @param NACvIntr Intruder NACv (1-4)
   * @param lambda Time since last good reading (s)
   * @param T Lookahead time (s)
   * @return Increase in safety buffer radius (m).  On an error, this returns a negative value.
   */
    static double psiHorizNAC(Position po, Position pi, Velocity vo, Velocity vi, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T);

};

}
#endif // SAFETYBUFFERS_H_
