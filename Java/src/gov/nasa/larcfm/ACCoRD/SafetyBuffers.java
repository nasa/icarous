/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;


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
public class SafetyBuffers {
  
  
  // values are from DO-242A tables
   public static final double[] NICHorizValues = {Double.MAX_VALUE,       // NIC 0
                                           Units.from("nm", 20.0), // NIC 1
                                           Units.from("nm", 8.0),  // NIC 2
                                           Units.from("nm", 4.0),  // NIC 3
                                           Units.from("nm", 2.0),  // NIC 4
                                           Units.from("nm", 1.0),  // NIC 5
                                           Units.from("nm", 0.6),  // NIC 6
                                           Units.from("nm", 0.2),  // NIC 7
                                           Units.from("nm", 0.1),  // NIC 8
                                           Units.from("m", 75.0),   // NIC 9
                                           Units.from("m", 25.0),   // NIC 10
                                           Units.from("m", 7.5)};   // NIC 11
   
   public static final double[] NICVertValues =  {Double.MAX_VALUE,     // NIC 0
                                           Double.MAX_VALUE,     // NIC 1
                                           Double.MAX_VALUE,     // NIC 2
                                           Double.MAX_VALUE,     // NIC 3
                                           Double.MAX_VALUE,     // NIC 4
                                           Double.MAX_VALUE,     // NIC 5
                                           Double.MAX_VALUE,     // NIC 6
                                           Double.MAX_VALUE,     // NIC 7
                                           Double.MAX_VALUE,     // NIC 8
                                           Units.from("m", 112.0),// NIC 9
                                           Units.from("m", 37.5), // NIC 10
                                           Units.from("m", 11.0)};// NIC 11
   
   static final double NACprob = 0.95;
   
   public static final double[] NACpHorizValues = {Double.MAX_VALUE,       // NACp 0
                                            Units.from("nm", 10.0), // NACp 1
                                            Units.from("nm", 4.0),  // NACp 2
                                            Units.from("nm", 2.0),  // NACp 3
                                            Units.from("nm", 1.0),  // NACp 4
                                            Units.from("nm", 0.5),  // NACp 5
                                            Units.from("nm", 0.6),  // NACp 6
                                            Units.from("nm", 0.1),  // NACp 7
                                            Units.from("nm", 0.05), // NACp 8
                                            Units.from("m", 30.0),   // NACp 9
                                            Units.from("m", 10.0),   // NACp 10
                                            Units.from("m", 3.0)};   // NACp 11
   
   public static final double[] NACpVertValues =  {Double.MAX_VALUE,     // NACp 0
                                            Double.MAX_VALUE,     // NACp 1
                                            Double.MAX_VALUE,     // NACp 2
                                            Double.MAX_VALUE,     // NACp 3
                                            Double.MAX_VALUE,     // NACp 4
                                            Double.MAX_VALUE,     // NACp 5
                                            Double.MAX_VALUE,     // NACp 6
                                            Double.MAX_VALUE,     // NACp 7
                                            Double.MAX_VALUE,     // NACp 8
                                            Units.from("m", 45.0), // NACp 9
                                            Units.from("m", 15.5), // NACp 10
                                            Units.from("m", 4.0)}; // NACp 11

   public static final double[] NACvHorizValues = {Double.MAX_VALUE,           //NACv 0
                                            Units.from("m/s", 10.0),    //NACv 1
                                            Units.from("m/s", 3.0),     //NACv 2
                                            Units.from("m/s", 1.0),     //NACv 3
                                            Units.from("m/s", 0.3),     //NAVv 4
                                            Units.from("m/s", 0.1),     // NONSTANDARD
                                            Units.from("m/s", 0.01),    // NONSTANDARD
                                            };

   public static final double[] NACvVertValues = {Double.MAX_VALUE,           //NACv 0
                                           Units.from("fps", 50.0),    //NACv 1
                                           Units.from("fps", 15.0),    //NACv 2
                                           Units.from("fps", 5.0),     //NACv 3
                                           Units.from("fps", 1.5),     //NAVv 4
                                           Units.from("fpm", 10.0),    // NONSTANDARD
                                           Units.from("fpm", 1.0)      // NONSTANDARD
                                           };

   public static final double[] SILValues = {0,            // SIL 0
                                      0.001,        // SIL 1
                                      0.00001,      // SIL 2
                                      0.0000001};   // SIL 3

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
  static double psiHorizNAC(double relHDist, double relGS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
    try {
      double ao = NACpHorizValues[NACpOwn];
      double ai = NACpHorizValues[NACpIntr];
      double bo = NACvHorizValues[NACvOwn];
      double bi = NACvHorizValues[NACvIntr];
      return psi(relHDist, relGS, ao, bo, ai, bi, lambda, T);
    } catch (Exception e) {
      System.out.println("Error in SafetyBuffers.psiHorizNAC "+e);
      return -1.0;
    }
  }

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
  static double psiHorizNIC(double relHDist, double relGS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T) {
    try {
      double ao = NICHorizValues[NICOwn];
      double ai = NICHorizValues[NICIntr];
      double bo = NACvHorizValues[NACvOwn];
      double bi = NACvHorizValues[NACvIntr];
      return psi(relHDist, relGS, ao, bo, ai, bi, lambda, T);
    } catch (Exception e) {
      System.out.println("Error in SafetyBuffers.psiHorizNIC "+e);
      return -1.0;
    }
  }

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
  static double psiVertNAC(double relVDist, double relVS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
    try {
      double ao = NACpVertValues[NACpOwn];
      double ai = NACpVertValues[NACpIntr];
      double bo = NACvVertValues[NACvOwn];
      double bi = NACvVertValues[NACvIntr];
      return psi(relVDist, relVS, ao, bo, ai, bi, lambda, T);
    } catch (Exception e) {
      System.out.println("Error in SafetyBuffers.psiVertNAC "+e);
      return -1.0;
    }
  }

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
  static double psiVertNIC(double relVDist, double relVS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T) {
    try {
      double ao = NICVertValues[NICOwn];
      double ai = NICVertValues[NICIntr];
      double bo = NACvVertValues[NACvOwn];
      double bi = NACvVertValues[NACvIntr];
      return psi(relVDist, relVS, ao, bo, ai, bi, lambda, T);
    } catch (Exception e) {
      System.out.println("Error in SafetyBuffers.psiVertNIC "+e);
      return -1.0;
    }

  }
  
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
  static double psi(double relDist, double relV, double ao, double ai, double bo, double bi, double lambda, double T) {
    try { 
      double l = (relDist + ao + ai + lambda * (relV + bo + bi))/(relV-bo-bi);
      double rtn = ao + ai + (Math.min(T,l) + lambda)*(bo+bi);
      if (Double.isInfinite(rtn) || Double.isNaN(rtn)) return -1.0;
      return rtn;
    } catch (Exception e) {
      return -1;
    }
  }
 
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
  static double psiH(Vect2 sm, Vect2 vm, double ao, double ai, double bo, double bi, double lambda, double T) {
    try { 
      double l = Math.max(-sm.dot(vm), 0.0) / ((vm.norm()-bo-bi)*(vm.norm()-bo-bi)) +
                 ((ao+ai)*vm.norm() + (sm.norm()+ao+ai)*(bo+bi))/(vm.norm()-bo-bi)*(vm.norm()-bo-bi);
      double rtn = ao + ai + (Math.min(T,l) + lambda)*(bo+bi);
      if (Double.isInfinite(rtn) || Double.isNaN(rtn)) return -1.0;
      return rtn;
    } catch (Exception e) {
      return -1;
    }
  }

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
    static double psiHorizNAC(Position po, Position pi, Velocity vo, Velocity vi, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
      try {
        double ao = NACpHorizValues[NACpOwn];
        double ai = NACpHorizValues[NACpIntr];
        double bo = NACvHorizValues[NACvOwn];
        double bi = NACvHorizValues[NACvIntr];
        Vect2 sm = po.vect2().Sub(pi.vect2());
        Vect2 vm = vo.vect2().Sub(vi.vect2());
        if (po.isLatLon()) {
          EuclideanProjection proj = Projection.createProjection(po.lla()); // this does NOT preserve altitudes (but this is a 2D calculation)
          sm = proj.project2(pi.lla());     // because ownship is the origin
          Velocity vmo = proj.projectVelocity(po, vo);
          Velocity vmi = proj.projectVelocity(pi, vi); 
          vm = vmo.vect2().Sub(vmi.vect2());
        }
        return psiH(sm, vm, ao, bo, ai, bi, lambda, T);
      } catch (Exception e) {
        System.out.println("Error in SafetyBuffers.psiHorizNAC "+e);
        return -1.0;
      }
    }

  }    