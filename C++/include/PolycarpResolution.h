/*
 * PolycarpResolution - closest point inside-outside a polygon to a given point - with a buffer
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * PolycarpResolution.h
 *
 *  Created on: Dec 18, 2015
 *      Author: ghagen
 */

#ifndef SRC_POLYCARPRESOLUTION_H_
#define SRC_POLYCARPRESOLUTION_H_

#include "Vect2.h"
#include <vector>

namespace larcfm {

class PolycarpResolution {
public:
  static Vect2 proj_vect(const Vect2& u, const Vect2& v, const Vect2& w, double BUFF);
  static std::vector<Vect2> expand_polygon_2D(double BUFF, double ResolBUFF, const std::vector<Vect2>& p);
  static std::vector<Vect2> contract_polygon_2D(double BUFF, double ResolBUFF, const std::vector<Vect2>& p);
  static int closest_edge(const std::vector<Vect2>& p, double BUFF, const Vect2& s);
  static Vect2 recovery_test_point(double BUFF, double ResolBUFF, const std::vector<Vect2>&, const Vect2& s, int eps);
  static Vect2 recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s, int eps);
  static Vect2 outside_recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s);
  static Vect2 inside_recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s);
};

} /* namespace larcfm */

#endif /* SRC_POLYCARPRESOLUTION_H_ */
