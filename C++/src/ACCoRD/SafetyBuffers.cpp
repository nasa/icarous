/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include <cfloat>
#include <limits>
#include "SafetyBuffers.h"
#include "Units.h"
#include "Util.h"
//#include "UnitSymbols.h"
#include "Position.h"
#include "Velocity.h"
#include "Vect2.h"
#include "EuclideanProjection.h"
#include "Projection.h"

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
  
using std::numeric_limits;
  
  // values are from DO-242A tables
   static const double NICHorizValues[] = {DBL_MAX,       // NIC 0
                                           Units::from("nm", 20.0), // NIC 1
                                           Units::from("nm", 8.0),  // NIC 2
                                           Units::from("nm", 4.0),  // NIC 3
                                           Units::from("nm", 2.0),  // NIC 4
                                           Units::from("nm", 1.0),  // NIC 5
                                           Units::from("nm", 0.6),  // NIC 6
                                           Units::from("nm", 0.2),  // NIC 7
                                           Units::from("nm", 0.1),  // NIC 8
                                           Units::from("m", 75.0),   // NIC 9
                                           Units::from("m", 25.0),   // NIC 10
                                           Units::from("m", 7.5)};   // NIC 11
   
   static const double NICVertValues[] =  {DBL_MAX,     // NIC 0
                                           DBL_MAX,     // NIC 1
                                           DBL_MAX,     // NIC 2
                                           DBL_MAX,     // NIC 3
                                           DBL_MAX,     // NIC 4
                                           DBL_MAX,     // NIC 5
                                           DBL_MAX,     // NIC 6
                                           DBL_MAX,     // NIC 7
                                           DBL_MAX,     // NIC 8
                                           Units::from("m", 112.0),// NIC 9
                                           Units::from("m", 37.5), // NIC 10
                                           Units::from("m", 11.0)};// NIC 11
   
//   static const double NACprob = 0.95;
   
   static const double NACpHorizValues[] = {DBL_MAX,       // NACp 0
                                            Units::from("nm", 10.0), // NACp 1
                                            Units::from("nm", 4.0),  // NACp 2
                                            Units::from("nm", 2.0),  // NACp 3
                                            Units::from("nm", 1.0),  // NACp 4
                                            Units::from("nm", 0.5),  // NACp 5
                                            Units::from("nm", 0.6),  // NACp 6
                                            Units::from("nm", 0.1),  // NACp 7
                                            Units::from("nm", 0.05), // NACp 8
                                            Units::from("m", 30.0),   // NACp 9
                                            Units::from("m", 10.0),   // NACp 10
                                            Units::from("m", 3.0)};   // NACp 11
   
   static const double NACpVertValues[] =  {DBL_MAX,     // NACp 0
                                            DBL_MAX,     // NACp 1
                                            DBL_MAX,     // NACp 2
                                            DBL_MAX,     // NACp 3
                                            DBL_MAX,     // NACp 4
                                            DBL_MAX,     // NACp 5
                                            DBL_MAX,     // NACp 6
                                            DBL_MAX,     // NACp 7
                                            DBL_MAX,     // NACp 8
                                            Units::from("m", 45.0), // NACp 9
                                            Units::from("m", 15.5), // NACp 10
                                            Units::from("m", 4.0)}; // NACp 11

   static const double NACvHorizValues[] = {DBL_MAX,           //NACv 0
                                            Units::from("mps", 10.0),    //NACv 1
                                            Units::from("mps", 3.0),     //NACv 2
                                            Units::from("mps", 1.0),     //NACv 3
                                            Units::from("mps", 0.3)};    //NAVv 4

   static const double NACvVertValues[] = {DBL_MAX,           //NACv 0
                                           Units::from("fps", 50.0),    //NACv 1
                                           Units::from("fps", 15.0),    //NACv 2
                                           Units::from("fps", 5.0),     //NACv 3
                                           Units::from("fps", 1.5)};    //NAVv 4

//   static const double SILValues[] = {0,            // SIL 0
//                                      0.001,        // SIL 1
//                                      0.00001,      // SIL 2
//                                      0.0000001};   // SIL 3


 double SafetyBuffers::psiHorizNAC(double relHDist, double relGS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
      double ao = NACpHorizValues[NACpOwn];
      double ai = NACpHorizValues[NACpIntr];
      double bo = NACvHorizValues[NACvOwn];
      double bi = NACvHorizValues[NACvIntr];
      return psi(relHDist, relGS, ao, bo, ai, bi, lambda, T);
  }

  double SafetyBuffers::psiHorizNIC(double relHDist, double relGS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T) {
      double ao = NICHorizValues[NICOwn];
      double ai = NICHorizValues[NICIntr];
      double bo = NACvHorizValues[NACvOwn];
      double bi = NACvHorizValues[NACvIntr];
      return psi(relHDist, relGS, ao, bo, ai, bi, lambda, T);
  }

  double SafetyBuffers::psiVertNAC(double relVDist, double relVS, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
      double ao = NACpVertValues[NACpOwn];
      double ai = NACpVertValues[NACpIntr];
      double bo = NACvVertValues[NACvOwn];
      double bi = NACvVertValues[NACvIntr];
      return psi(relVDist, relVS, ao, bo, ai, bi, lambda, T);
  }

  double SafetyBuffers::psiVertNIC(double relVDist, double relVS, int NICOwn, int NACvOwn, int NICIntr, int NACvIntr, double lambda, double T) {
      double ao = NICVertValues[NICOwn];
      double ai = NICVertValues[NICIntr];
      double bo = NACvVertValues[NACvOwn];
      double bi = NACvVertValues[NACvIntr];
      return psi(relVDist, relVS, ao, bo, ai, bi, lambda, T);
  }
  
  double SafetyBuffers::psi(double relDist, double relV, double ao, double ai, double bo, double bi, double lambda, double T) {
      double div = (relV-bo-bi);
      if (div == 0.0) return -1.0;
      double l = (relDist + ao + ai + lambda * (relV + bo + bi))/div;
      double rtn = ao + ai + (Util::min(T,l) + lambda)*(bo+bi);
      if (rtn == PINFINITY || rtn == NINFINITY ||
          rtn == NaN || rtn == SIGNaN) return -1.0;
      return rtn;
  }
 
  double SafetyBuffers::psiH(Vect2 sm, Vect2 vm, double ao, double ai, double bo, double bi, double lambda, double T) {
      double div = (vm.norm()-bo-bi)*(vm.norm()-bo-bi);
      if (div == 0.0) return -1.0;
      double l = Util::max(-sm.dot(vm), 0.0) / div +
                 ((ao+ai)*vm.norm() + (sm.norm()+ao+ai)*(bo+bi))/div;
      double rtn = ao + ai + (Util::min(T,l) + lambda)*(bo+bi);
      if (rtn == PINFINITY || rtn == NINFINITY ||
          rtn == NaN || rtn == SIGNaN) return -1.0;
      return rtn;
  }

    double SafetyBuffers::psiHorizNAC(Position po, Position pi, Velocity vo, Velocity vi, int NACpOwn, int NACvOwn, int NACpIntr, int NACvIntr, double lambda, double T) {
        double ao = NACpHorizValues[NACpOwn];
        double ai = NACpHorizValues[NACpIntr];
        double bo = NACvHorizValues[NACvOwn];
        double bi = NACvHorizValues[NACvIntr];
        Vect2 sm = po.vect2().Sub(pi.vect2());
        Vect2 vm = vo.vect2().Sub(vi.vect2());
        if (po.isLatLon()) {
          EuclideanProjection proj = Projection::createProjection(po.lla());
          sm = proj.project2(pi.lla());     // because ownship is the origin
          Velocity vmo = proj.projectVelocity(po, vo);
          Velocity vmi = proj.projectVelocity(pi, vi); 
          vm = vmo.vect2().Sub(vmi.vect2());
        }
        return psiH(sm, vm, ao, bo, ai, bi, lambda, T);
    }

  }    
