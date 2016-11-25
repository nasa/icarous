/*
 * TangentLine.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Computes a vector that is tangent to the protected zone.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TANGENTLINE_H_
#define TANGENTLINE_H_

#include "Vect2.h"

namespace larcfm {

/** Tangent line to a circle */
class TangentLine: public Vect2 {
public:
  /** Construct a tangent line */
  TangentLine();
  /** Construct a tangent line */
  TangentLine(const Vect2& s, const double D, const int eps);
  /** Is the tangent line undefined? */
  bool undef() const;
  /** Has horizontal criterion been met? */
  bool horizontal_criterion(const Vect2& v) const;
  /** Epsilon value */
  int  get_eps() const;

  /** Q function */
  static Vect2 Q(const Vect2& s, const double D, const int eps);

private:
  int eps;
};

///**  Q function */
//  Vect2 Q(const Vect2& s, const double D, const int eps);

}

#endif /* TANGENTLINE_H_ */
