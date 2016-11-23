/*
 * Horizontal.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef HORIZONTAL_H_
#define HORIZONTAL_H_

#include "TangentLine.h"
#include "Vect3.h"

namespace larcfm {

/** Horizontal solution */
class Horizontal: public Vect2 {
public:

  /** Construct a horizontal solution */
  Horizontal();

  /** Construct a horizontal solution */
  Horizontal(const Vect2& v);

  /** Construct a horizontal solution */
  Horizontal(double kk, const Vect2& v);

  /** Is this solution undefined? */
  bool undef() const;

  /** parameter */
  double  k;

  /** "Solution" indicating no solution */
  static const Horizontal NoHorizontalSolution;

  /** */
  static Horizontal best_horizontal(const Vect2& vo,const Horizontal& v1, const Horizontal& v2);

  /**
   * Time of closest point of approach.
   * 
   * @param v Vector
   * 
   * @return the time of horizontal closest point of approach between <code>this</code> point and the line defined
   * by the vector <code>v</code>. 
   */
  static double tcpa(const Vect2& s, const Vect2& v);

  /**
   * Distance closest point of approach.
   *
   * @param v Vector
   *
   * @return the horizontal distance at closest point of approach between <code>this</code> point and the line defined
   * by the vector <code>v</code>.
   */
  static double dcpa(const Vect2& s, const Vect2& v);

  /* Horizontal miss distance within lookahead time */
  static double hmd(const Vect2& s, const Vect2& v, double T);

  /**
   * Intersection time between line and circle.
   * 
   * @param v Vector
   * @param D Diameter of of circle
   * @param eps +-1
   * 
   * @return the time <i>t</i> such that the parametric line <code>s</code>+<i>t</i><code>v</code>
   * intersects the circle of radius <code>D</code>. If <code>eps == -1</code> the returned time is 
   * the entry time; if <code>eps == 1</code> the  returned time is the exit time.
   */
  static double Theta_D(const Vect2& s, const Vect2& v,
      const int eps, const double D);
  /**
   * Discriminant of intersection between line and circle.
   * 
   * @param v Vector
   * @param D Diameter of of circle
   * 
   * @return the discriminant of the intersection between the parametric line 
   * <code>s</code>+<i>t</i><code>v</code> and the circle of radius <code>D</code>. 
   * If the discriminant is less than <code>0</code> the intersection doesn't exist;
   * if it is <code>0</code> the line is tangent to the circle; otherwise, the line
   * intersects the circle in two different points. 
   */
  static double Delta(const Vect2& s, const Vect2& v, const double D);
  /** */
  static bool   almost_horizontal_los(const Vect2& s, const double D);
  /** */
  static bool   horizontal_sep(const Vect2& s, const double D);
  /** */
  static bool horizontal_dir(const Vect2& s, const Vect2& v, const int dir);
  /** */
  static bool horizontal_dir_at(const Vect2& s, const Vect2& v, const double t,
      const int dir);

  static bool horizontal_entry(const Vect2& s, const Vect2& v);

  /** */
  static Vect2 Vdir(const Vect2& s, const Vect2& v);
  /** */
  static Vect2 W0(const Vect2& s, const double j);

  /** Solve the following equation on k and l:
   *   k*nv = l*vo-vi.
   */
  static Horizontal gs_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi);

  /** */
  static Horizontal gs_line(const Vect2& nv,const Vect2& vo,const Vect2& vi);

  /** */
  static Horizontal gs_only_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
      const double j);

  /** */
  static Horizontal gs_only_vertical(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double th, const int dir, const double D);

  /** */
  static Horizontal gs_vertical(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const TangentLine& l, const int epsv,
      const double D, const double H);

  /** */
  static Horizontal gs_only(const TangentLine& nv,const Vect3& s,const Vect3& vo,const Vect3& vi,const int epsv,
      const double D, const double H);

  /** */
  static Horizontal gs_only_circle(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double t, const int dir, const int irt, const double D);

  /** */
  static Horizontal gs_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const int dir, const int irt, const double D, const double H);

  /** Solve the following equation on k:
   * || k*nv + vi || = || vo ||.
   */
  static Horizontal trk_only_line_irt(const Vect2& nv,const Vect2& vo,const Vect2& vi,
      const int irt);

  /** */
  static Horizontal trk_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi);

  /** */
  static Horizontal trk_line_irt(const Vect2& nv,const Vect2& vo,const Vect2& vi,
      const int irt);

  /** */
  static Horizontal trk_line(const Vect2& nv,const Vect2& vo,const Vect2& vi);

  /** */
  static Horizontal trk_only_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
      const double j,const int irt);

  /** */
  static Horizontal trk_only_vertical(const Vect2& s,const Vect2& vo,const Vect2& vi,
      const double th,const int dir,const int irt,const double D);

  /** */
  static Horizontal trk_vertical_irt(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
      const int epsv,const int irt,const double D,const double H);



  static Horizontal repulsiveTrk(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed);

  static Horizontal repulsiveGs(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed, double minGs, double maxGs);

  /** */
  static Horizontal trk_vertical(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
      const int epsv,const double D,const double H);

  /** */
  static Horizontal trk_only(const TangentLine& nv,const Vect3& s,const Vect3& vo,const Vect3& vi,
      const int epsv,const double D, const double H);

  /** */
  static Horizontal trk_only_circle(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double t, const int dir, const int irt, const double D);

  /** */
  static Horizontal trk_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const int dir, const int irt, const double D, const double H);

  /**
   * Solve the following equation on k and l:
   *   nv * (k*nv-v) = 0, where v = vo-vi.
   */
  static Horizontal opt_trk_gs_line(const Vect2& nv, const Vect2& vo, const Vect2& vi);

  /** */
  static Horizontal opt_line(const Vect2& nv,const Vect2& vo,const Vect2& vi);

  /** */
  static Horizontal opt_trk_gs(const TangentLine& nv,const Vect3& s,const Vect3& vo,const Vect3& vi,
      const int epsv,const double D,const double H);

  /** */
  static Horizontal opt_trk_gs_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
      const double j);

  /** */
  static Horizontal opt_trk_gs_vertical(const Vect2& s,const Vect2& vo,const Vect2& vi,
      const double th,const int dir,const double D);

  /** */
  static Horizontal opt_vertical(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
      const int epsv,const double D,const double H);

};

}

#endif /* HORIZONTAL_H_ */
