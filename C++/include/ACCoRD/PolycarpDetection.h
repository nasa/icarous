/*
 * PolycarpDetection - collision detection between and point and a 2D polygon
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPDETECTION_H_
#define POLYCARPDETECTION_H_

#include "Interval.h"
#include "MovingPolygon2D.h"
#include "Vect2.h"
#include <vector>

namespace larcfm {
class PolycarpDetection {
private:
  static std::vector<Vect2> polygon_2D_at(const std::vector<Vect2>& p, const Vect2& pv, double t);
  static double dot_zero_linear_2D_alg(double B, double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps);
  static double lookahead_proj(double B, double T, double t);
  static std::vector<double> swap_times(double B, double T, const Vect2& s, const Vect2& v, const Vect2& segstart, const Vect2& segend, const Vect2& startvel, const Vect2& endvel);
public:
  static Interval dot_nneg_linear_2D_alg(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps);
  static Interval dot_nneg_spec(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps, int eps2, double Fac);
  static bool edge_detect_simple(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, double Fac);
  static bool edge_detect(double T, const Vect2& s, const Vect2& v, const Vect2& segstart, const Vect2& segend, const Vect2& startvel, const Vect2& endvel, double Fac);
  static bool Collision_Detector(double B, double T, const MovingPolygon2D& mp, const Vect2& s, const Vect2& v, double BUFF, double Fac, bool insideBad);
  static bool Static_Collision_Detector(double B, double T, const std::vector<Vect2>& p, const Vect2 pv, const Vect2& s, const Vect2& v, double BUFF, bool insideBad);
  static std::vector<double> collisionTimesInOut(double B, double T, const MovingPolygon2D& mp, const Vect2& s, const Vect2& v, double BUFF, double Fac);
  static bool nice_moving_polygon_2D(double B, double T, const MovingPolygon2D& mp, double BUFF, double Fac);

  };
}
#endif

