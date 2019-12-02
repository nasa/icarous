/*
 * Copyright (c) 2012-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TCAS2D_H_
#define TCAS2D_H_

#include "Vect2.h"

namespace larcfm {

class TCAS2D {
  
public:

  double t_in;
  double t_out;

  // Compute modified tau
  static double tau_mod(double DMOD, const Vect2& s, const Vect2& v);
  
  static bool horizontal_RA(double DMOD, double Tau, const Vect2& s, const Vect2& v);
  
  static bool horizontal_RA_at(double DMOD, double Tau, const Vect2& s, const Vect2& v, double t);
  
  static double nominal_tau(double B, double T, const Vect2& s, const Vect2& v, double rr);
  
  static double time_of_min_tau(double DMOD, double B, double T, const Vect2& s, const Vect2& v);

  static bool min_tau_undef(double DMOD, double B, double T, Vect2 s, Vect2 v);

  static bool RA2D(double DMOD, double Tau, double B, double T, const Vect2& s, const Vect2& v);

  void RA2D_interval(double DMOD, double Tau, double B, double T, Vect2 s, Vect2 vo, Vect2 vi);

  
};
}
#endif
