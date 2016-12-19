/* 
 * GreatCircle
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
//#include "UnitSymbols.h"
#include "GreatCircle.h"
#include "Triple.h"

#include <cmath>
namespace larcfm {



  static const double EPS = 1.0e-15; // small number, about machine

  const double GreatCircle::spherical_earth_radius = 6366707.0194937070000000000; // Units::from(_m, 1.0 / angle_from_distance(1.0));

  const double GreatCircle::minDt = 1E-5;

  double GreatCircle::decimal_angle(double degrees, double minutes, double seconds, bool north_east) {
    return ((north_east) ? 1.0 : -1.0) * Units::from("deg", (degrees + minutes / 60.0 + seconds / 3600.0));
  }

  double GreatCircle::angle_from_distance(double distance) {
    return Units::to("nm", distance) * Pi / (180.0 * 60.0);
  }

  double GreatCircle::angle_from_distance(double distance, double h) {
    return angle_from_distance(distance * spherical_earth_radius
			       / (spherical_earth_radius + h));
  }

  double GreatCircle::distance_from_angle(double angle, double h) {
	return (spherical_earth_radius+h) * angle;
  }

  double GreatCircle::angular_distance(double lat1, double lon1, double lat2, double lon2) {
//	  if (Util::almost_equals(lat1,lat2) && Util::almost_equals(lon1,lon2)) return 0.0;
    double rtn= asin_safe(sqrt_safe(sq(sin((lat1 - lat2) / 2))
				    + cos(lat1) * cos(lat2)
				    * sq(sin((lon1 - lon2) / 2)))) * 2.0;
    return rtn;
  }

  double GreatCircle::angular_distance(const LatLonAlt& p1, const LatLonAlt& p2) {
    return angular_distance(p1.lat(), p1.lon(), p2.lat(), p2.lon());
  }

  double GreatCircle::distance(double lat1, double lon1, double lat2, double lon2) {
    return distance_from_angle(angular_distance(lat1, lon1, lat2, lon2), 0.0);
  }

  double GreatCircle::distance(const LatLonAlt& p1, const LatLonAlt& p2) {
//std::cout << p1.toString() << " , " << p2.toString() << std::endl;
	  //return distance_from_angle(angular_distance(p1, p2),(p1.alt() + p2.alt())/2.0);
    return distance_from_angle(angular_distance(p1, p2), 0.0);
  }

  bool GreatCircle::almost_equals(double lat1, double lon1, double lat2, double lon2) {
    return Constants::almost_equals_radian(angular_distance(lat1, lon1, lat2, lon2));
  }
	

  bool GreatCircle::almost_equals(double lat1, double lon1, double lat2,
		     double lon2, double epsilon) {
    return Util::within_epsilon(distance(lat1, lon1, lat2, lon2), epsilon);
  }

  // parameter d is the angular distance between lat/lon #1 and lat/lon #2
  static double initial_course_impl(const LatLonAlt& p1, const LatLonAlt& p2, double d) {
    double lat1 = p1.lat();
    double lon1 = p1.lon();
    double lat2 = p2.lat();
    double lon2 = p2.lon();
    
    if (cos(lat1) < EPS) { // EPS a small number, about machine precision
      if (lat1 > 0) {
	return Pi; // starting from North pole, all directions are south
      } else {
	return 2.0 * Pi; // starting from South pole, all directions are
	// north. JMM: why not 0?
      }
    }
    
    if (Constants::almost_equals_radian(d)) {
      return 0.0;
      // if the two points are almost the same, then any course is valid
      // returning 0.0 here avoids a 0/0 division (NaN) in the
      // calculations below.
    }
    
    double tc1;
    double acos1 = (sin(lat2) - sin(lat1) * cos(d)) / (sin(d) * cos(lat1));
    //std::cout << "GCICI num=" << (sin(lat2) - sin(lat1) * cos(d)) << std::endl;
    //std::cout << "GCICI denom=" << (sin(d)*cos(lat1)) << std::endl;
    //std::cout << "GCICI sin(d)=" << sin(d) << std::endl;
    //std::cout << "GCICI lat1=" << lat1 << std::endl;
    //std::cout << "GCICI cos(lat1)=" << cos(lat1) << std::endl;
    //std::cout << "GCICI " << acos1 << std::endl;
    if (Util::almost_equals(lon2, lon1, PRECISION13)) {
      tc1 = acos_safe(acos1);
    } else {		
      if (sin(lon2 - lon1) > 0) { 
	// slightly different than av
	// formulary because of +East convention that I use
	tc1 = acos_safe(acos1);
      } else {
	tc1 = 2 * Pi - acos_safe(acos1);
      }
    }
    //std::cout << "GCICI crs=" << tc1 << "  " << (tc1-Pi) << std::endl;
    return tc1;
  }
  
  double GreatCircle::initial_course(double lat1, double lon1, double lat2, double lon2) {
    LatLonAlt p1 = LatLonAlt::mk(lat1, lon1, 0.0);
    LatLonAlt p2 = LatLonAlt::mk(lat2, lon2, 0.0);
    double d = angular_distance(lat1, lon1, lat2, lon2);
    return initial_course_impl(p1, p2, d);
  }

  double GreatCircle::initial_course(LatLonAlt p1, LatLonAlt p2) {
     	    return initial_course(p1.lat(),p1.lon(),
                                  p2.lat(),p2.lon());
  }


  double GreatCircle::final_course(LatLonAlt p1, LatLonAlt p2) {
	  return initial_course(p2, p1)+M_PI;
  }

  // parameter d is the angular distance between lat/long #1 and #2
  static LatLonAlt interpolate_impl(const LatLonAlt& p1, const LatLonAlt& p2, double d, double f, double alt) {
    if (Constants::almost_equals_radian(d) ) {
      return p1.mkAlt(alt);
      // if the two points are almost the same, then consider the two
      // points the same and arbitrarily return one of them (in this case
      // p1)
      // with the altitude that was provided
    }
    
    double lat1 = p1.lat();
    double lon1 = p1.lon();
    double lat2 = p2.lat();
    double lon2 = p2.lon();
    
    double a = sin((1 - f) * d) / sin(d);
    double b = sin(f * d) / sin(d);
    double x = a * cos(lat1) * cos(lon1) + b * cos(lat2) * cos(lon2);
    double y = a * cos(lat1) * sin(lon1) + b * cos(lat2) * sin(lon2);
    double z = a * sin(lat1) + b * sin(lat2);
    return LatLonAlt::mk(atan2_safe(z, sqrt(x * x + y * y)), // lat
				  atan2_safe(y, x), // longitude
				  alt); // long
  }
  
  double GreatCircle::representative_course(double lat1, double lon1, double lat2, double lon2) {
    LatLonAlt p1 = LatLonAlt::mk(lat1, lon1, 0.0);
    LatLonAlt p2 = LatLonAlt::mk(lat2, lon2, 0.0);
    double d = angular_distance(lat1, lon1, lat2, lon2);
    LatLonAlt midPt = interpolate_impl(p1, p2, d, 0.5, 0.0);
    return initial_course_impl(midPt, p2, d / 2.0);
  }
  

  double GreatCircle::representative_course(const LatLonAlt& p1, const LatLonAlt& p2) {
	  return representative_course(p1.lat(),p1.lon(),p2.lat(),p2.lon());
  }

  LatLonAlt GreatCircle::interpolate(const LatLonAlt& p1, const LatLonAlt& p2, double f) {
    double d = angular_distance(p1, p2);
    return interpolate_impl(p1, p2, d, f, (p2.alt() - p1.alt())*f + p1.alt());
  }

  LatLonAlt GreatCircle::interpolateEst(const LatLonAlt& p1, const LatLonAlt& p2, double f) {
		return LatLonAlt::mk((p2.lat() - p1.lat()) * f + p1.lat(),
		                     (p2.lon() - p1.lon()) * f + p1.lon(),
              	             (p2.alt() - p1.alt()) * f + p1.alt());
  }

  static LatLonAlt linear_initial_impl(const LatLonAlt& s, double track, double d, double vertical) {
    double lat = asin_safe(sin(s.lat())*cos(d)+cos(s.lat())*sin(d)*cos(track));
    double dlon = atan2_safe(sin(track)*sin(d)*cos(s.lat()),cos(d)-sin(s.lat())*sin(lat));
    // slightly different from aviation formulary because I use
    // "east positive" convention
    double lon = to_pi(s.lon() + dlon);

    return LatLonAlt::mk(lat, lon, s.alt()+vertical);
  }

  static LatLonAlt linear_rhumb_impl(const LatLonAlt& s, double track, double d, double vertical) {
	// -- Based on the calculation in the "Rhumb line" section of the
	//    Aviation Formulary v1.44
	// -- Weird things happen to rhumb lines that go to the poles, therefore
	//    force any polar latitudes to be "near" the pole

	static const double eps = 1e-15;
	double s_lat = Util::max(Util::min(s.lat(), Pi/2-eps), -Pi/2+eps);
	double lat = s_lat + d * cos(track);
	lat = Util::max(Util::min(lat, Pi/2-eps), -Pi/2+eps);

	double q;
	if ( Constants::almost_equals_radian(lat, s_lat) ) {
		// (std::abs(lat - lat1) < EPS) {
		q = cos(s_lat);
	} else {
		double dphi = log(tan(lat / 2 + Pi / 4)
				/ tan(s_lat / 2 + Pi / 4));
		q = (lat - s_lat) / dphi;
	}
	double dlon = -d * sin(track) / q;

	// slightly different from aviation formulary because I use
	// "east positive" convention
	double lon = to_pi(s.lon() - dlon);
	return LatLonAlt::mk(lat, lon, s.alt()+vertical);
  }

  LatLonAlt GreatCircle::linear_gcgs(const LatLonAlt& p1, const LatLonAlt& p2, const Velocity& v, double t) {
	double d = GreatCircle::angular_distance(p1, p2);
	if ( Constants::almost_equals_radian(d) ) {
		// If the two points are about 1 meter apart, then count them as the
      // same.
      return p1;
    }
    double f = angle_from_distance(v.gs() * t) / d;
    return interpolate_impl(p1, p2, d, f, p1.alt() + v.z*t);
  }
  
  LatLonAlt GreatCircle::linear_gc(LatLonAlt p1, LatLonAlt p2, double d) {
    //return GreatCircle.linear_initial(p1, initial_course(p1,p2), d);
    double dist = angular_distance(p1,p2);
    double f = angle_from_distance(d)/dist;
    return interpolate_impl(p1, p2, dist, f, (p2.alt() - p1.alt())*f + p1.alt());
  }
  
  LatLonAlt GreatCircle::linear_rhumb(const LatLonAlt& s, const Velocity& v, double t) {
    return linear_rhumb_impl(s, v.trk(), GreatCircle::angle_from_distance(v.gs() * t), v.z*t);
  }

  LatLonAlt GreatCircle::linear_rhumb(const LatLonAlt& s, double track, double dist) {
    return linear_rhumb_impl(s, track, GreatCircle::angle_from_distance(dist), 0.0);
  }
	

	/**
	 * Solve the spherical triangle when one has a side (in angular distance), another side, and an angle between sides.
	 * The angle is <b>not</b> between the sides.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 *
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  A firstSolution == true
	 * will return a smaller angle, B, than firstSolution == false.
	 *
	 * @param b one side (in angular distance)
	 * @param a another side (in angular distance)
	 * @param A the angle opposite the side a
	 * @param firstSolution select which solution to use
	 * @return a Triple of angles B and C, and the side c.
	 */
	Triple<double,double,double> GreatCircle::side_side_angle(double b, double a, double A, bool firstSolution) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c

		// Law of sines
		double B = Util::asin_safe(std::sin(b)*std::sin(A)/std::sin(a));  // asin returns [-pi/2,pi/2]
		if ( ! firstSolution) {
			B = M_PI - B;
		}

		// one of Napier's analogies
		double c = 2 * Util::atan2_safe(std::sin(0.5*(a+b))*std::cos(0.5*(A+B)),std::cos(0.5*(a+b))*std::cos(0.5*(A-B)));

		// Law of cosines
		double C = Util::acos_safe(-std::cos(A)*std::cos(B)+std::sin(A)*std::sin(B)*std::cos(c));

		if ( gauss_check(a,b,c,A,B,C)) {
			return Triple<double,double,double>(Util::to_pi(B),C,Util::to_2pi(c));
		} else {
			return Triple<double,double,double>(0.0,0.0,0.0);
		}
	}

	/**
	 * Solve the spherical triangle when one has a side (in angular distance), and two angles.
	 * The side is <b>not</b> between the angles.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 *
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  A firstSolution == true
	 * will return a smaller side, b, than firstSolution == false.
	 *
	 * @param a one side (in angular distance)
	 * @param A the angle opposite the side a
	 * @param B another angle
	 * @param firstSolution select which solution to use
	 * @return a Triple of side b, angle C, and the side c.
	 */
	Triple<double,double,double> GreatCircle::side_angle_angle(double a, double A, double B, bool firstSolution) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c

		// Law of sines
		double b = Util::asin_safe(std::sin(a)*std::sin(B)/std::sin(A));  // asin returns [-pi/2,pi/2]
		if ( ! firstSolution) {
			b = M_PI - b;
		}

		// one of Napier's analogies
		double c = 2 * Util::atan2_safe(std::sin(0.5*(a+b))*std::cos(0.5*(A+B)),std::cos(0.5*(a+b))*std::cos(0.5*(A-B)));

		// Law of cosines
		double C = Util::acos_safe(-std::cos(A)*std::cos(B)+std::sin(A)*std::sin(B)*std::cos(c));

		if ( gauss_check(a,b,c,A,B,C)) {
			return Triple<double,double,double>(Util::to_2pi(b),Util::to_2pi(C),Util::to_2pi(c));
		} else {
			return Triple<double,double,double>(0.0,0.0,0.0);
		}
	}

	/**
	 * This implements the spherical cosine rule to complete a triangle on the unit sphere
	 * @param a side a (angular distance)
	 * @param C angle between sides a and b
	 * @param b side b (angular distance)
	 * @return triple of A,B,c (angle opposite a, angle opposite b, side opposite C)
	 */
	Triple<double,double,double> GreatCircle::side_angle_side(double a, double C, double b) {
		double c = Util::acos_safe(std::cos(a)*std::cos(b)+std::sin(a)*std::sin(b)*std::cos(C));
		double cRatio = std::sin(C)/std::sin(c);
		double A = Util::asin_safe(std::sin(a)*cRatio);
		double B = Util::asin_safe(std::sin(b)*cRatio);
		return Triple<double,double,double>(A,B,c);
	}

	/**
	 * This implements the supplemental (polar triangle) spherical cosine rule to complete a triangle on the unit sphere
	 * @param A angle A
	 * @param c side between A and B (angular distance
	 * @param B angle B
	 * @return triple of a,b,C (side opposite A, side opposite B, angle opposite c)
	 */
	Triple<double,double,double> GreatCircle::angle_side_angle(double A, double c, double B) {
		double C = Util::acos_safe(-std::cos(A)*std::cos(B)+std::sin(A)*std::sin(B)*std::cos(c));
		double cRatio = std::sin(c)/std::sin(C);
		double a = Util::asin_safe(std::sin(A)*cRatio);
		double b = Util::asin_safe(std::sin(B)*cRatio);
		return Triple<double,double,double>(a,b,C);
	}

	bool GreatCircle::gauss_check(double a, double b, double c, double A, double B, double C) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c
		A = Util::to_pi(A);
		B = Util::to_pi(B);
		C = Util::to_pi(C);
		a = Util::to_2pi(a);
		b = Util::to_2pi(b);
		c = Util::to_2pi(c);
		if (A==0.0 || A==M_PI || B==0.0 || B==M_PI || C==0.0 || C==M_PI) return false;
		if (a==0.0 || b==0.0 || c==0.0) return false;
//		f.pln("gauss "+std::cos(0.5*(A+B))*std::cos(0.5*c)+" "+std::cos(0.5*(a+b))*std::sin(0.5*C));
		return Util::almost_equals(std::cos(0.5*(A+B))*std::cos(0.5*c),std::cos(0.5*(a+b))*std::sin(0.5*C),PRECISION13);
	}



  LatLonAlt GreatCircle::linear_initial(const LatLonAlt& s, const Velocity& v, double t) {
    return linear_initial_impl(s, v.trk(), GreatCircle::angle_from_distance(v.gs() * t), v.z*t);
  }

  LatLonAlt GreatCircle::linear_initial(const LatLonAlt& s, double track, double dist) {
    return linear_initial_impl(s, track, GreatCircle::angle_from_distance(dist), 0.0);
  }

  double GreatCircle::cross_track_distance(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& offCircle) {
  	double dist_p1oc = angular_distance(p1,offCircle);
  	double trk_p1oc = initial_course_impl(p1,offCircle,dist_p1oc);
  	double trk_p1p2 = initial_course(p1,p2);
  	// This is a direct application of the "spherical law of sines"
  	return distance_from_angle(Util::asin_safe(sin(dist_p1oc)*sin(trk_p1oc-trk_p1p2)), (p1.alt()+p2.alt()+offCircle.alt())/3.0);
  }

  bool GreatCircle::collinear(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3) {
    	double epsilon = 1E-7;
    	return Util::within_epsilon(cross_track_distance(p1,p2,p3),epsilon);
    }


	/**
	 * This returns the point on the great circle running through p1 and p2 that is closest to point x.
	 * The altitude of the output is the same as x.<p>
	 * If p1 and p2 are the same point, then every great circle runs through them, thus x is on one of these great circles.  In this case, x will be returned.
	 * This assumes any 2 points will be within 90 degrees of each other (angular distance).
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
  LatLonAlt GreatCircle::closest_point_circle(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x) {
		// almost same point or antipode:
		if ((Util::almost_equals(p1.lat(),p2.lat()) && Util::almost_equals(p1.lon(),p2.lon())) ||
			(Util::almost_equals(p1.lat(),-p2.lat()) && Util::almost_equals(p1.lon(),Util::to_pi(p2.lon()+Pi)))) return x;
		Vect3 a = spherical2xyz(p1.lat(), p1.lon());
		Vect3 b = spherical2xyz(p2.lat(), p2.lon());
		if (a.almostEquals(b) ||a.almostEquals(b.Neg())) return x;
		Vect3 c = a.cross(b);
		Vect3 p = spherical2xyz(x.lat(), x.lon());
		Vect3 g = p.Sub(c.Scal(p.dot(c)/c.sqv()));
		double v = spherical_earth_radius/g.norm();
		return xyz2spherical(g.Scal(v)).mkAlt(x.alt()); // return to x's altitude
	}

	/**
	 * This returns the point on the great circle segment running through p1 and p2 that is closest to point x.
	 * This will return either p1 or p2 if the actual closest point is outside the segment.
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
	LatLonAlt GreatCircle::closest_point_segment(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x) {
		LatLonAlt c = closest_point_circle(p1,p2,x);
		double d12 = distance(p1,p2);
		double d1c = distance(p1,c);
		double d2c = distance(p2,c);
		if (d1c < d12 && d2c < d12) {
			return c;
		}
		if (d1c < d2c) {
			return p1;
		} else {
			return p2;
		}
	}


//  LatLonAlt GreatCircle::closest_point_circle(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x) {
//		double a = angular_distance(x,p2);
//		double b = angular_distance(p1,p2);
//		double c = angular_distance(p1,x);
//		double A = angle_between(p2,p1,x);
//		double B = angle_between(p1,x,p2);
//		double C = angle_between(x,p2,p1);
//		return closest_point_circle(p1,p2,x,a,b,c,A,B,C);
//  }

  LatLonAlt GreatCircle::closest_point_circle(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x, double a, double b, double c, double A, double B, double C) {
		//       x (B)
		//      / \
		// (A) p1--p2 (C)
//		double a = angular_distance(x,p2);
//		double b = angular_distance(p1,p2);
//		double c = angular_distance(p1,x);
//		double A = angle_between(p2,p1,x);
//		double B = angle_between(p1,x,p2);
//		double C = angle_between(x,p2,p1);

//		if (Util::almost_equals(A, 0.0) || Util::almost_equals(C, 0.0) || Util::almost_equals(A, M_PI) || Util::almost_equals(C, M_PI)) {			// collinear
		if (Util::within_epsilon(A, 0.000001) || Util::within_epsilon(C, 0.000001) || Util::within_epsilon(M_PI-A, 0.000001) || Util::within_epsilon(M_PI-C, 0.000001)) {
			return x;
		}
		if (Util::almost_equals(b,0.0)) {
			return x;   // if p1==p2, every great circle runs through them, thus x is on one of these great circles
		}
		if (A+B+C < M_PI || A+B+C >= M_PI*3) {
//			fpln("GreatCircle.closestPoint ERROR: not a triangle p1="+p1+"p2="+p2+"x="+x+" A+B+C="+(A+B+C)+" = "+Units.to("deg", A+B+C)+" deg");
			// if the triangle is relatively small, it is probably collinear
			if (a < M_PI/2 && b < M_PI/2 && c < M_PI/2) {
				return x;
			}
			return LatLonAlt::INVALID();
		}
		if (p1.almostEquals(x) || Util::almost_equals(A, M_PI/2)) {
			return p1.mkAlt(x.alt());
		}
		if (p2.almostEquals(x) || Util::almost_equals(C, M_PI/2)) {
			return p2.mkAlt(x.alt());
		}

		// general case p1 @ A, x @ B, p2 @ C
//		double d1 = 0;
//f.pln("GreatCircle.closest_point_circle DEG A="+Units.to("deg", A)+" B="+Units.to("deg", B)+" C="+Units.to("deg", C)+" / a="+Units.to("deg", a)+" b="+Units.to("deg", b)+" c="+Units.to("deg", c));
//f.pln("GreatCircle.closest_point_circle RAD A="+A+" B="+B+" C="+C+" / a="+a+" b="+b+" c="+c);
//f.pln("GreatCircle.closest_point_circle A/a="+(std::sin(A)/std::sin(a))+" B/b="+(std::sin(B)/std::sin(b))+" C/c="+(std::sin(C)/std::sin(c)));
		if (A <= M_PI/2 && C <= M_PI/2) {
			//   B       C1
			//  / \     / |
			// A---C   B1-A1
			if (A < C) {
				double a1 = c;
				double A1 = M_PI/2;
				double B1 = A;
				double c1 = side_angle_angle(a1,A1,B1, true).third;
				double ff = (c1/b);
//f.pln("GreatCircle.closest_point_circle a1) ff="+ff);
				return interpolate(p1,p2,ff);
			} else {
				//   B     C1
				//  / \    | \
				// A---C   A1-B1
				double a1 = a;
				double A1 = M_PI/2;
				double B1 = C;
				double c1 = side_angle_angle(a1,A1,B1, true).third;
				double ff = (c1/b);
//f.pln("GreatCircle.closest_point_circle a2) ff="+ff);
				return interpolate(p2,p1,ff);
			}

//			d1 = side_angle_angle(a,M_PI/2,C,true).third;
//f.pln("GreatCircle.closest_point_circle #1 d1="+d1+" "+Units.to("deg", d1)+Units.degreeStr);
//			double ff = 1-(d1/b);
//f.pln("GreatCircle.closest_point_circle p1="+p1+" p2="+p2+" ff="+ff);
//			return interpolate(p1, p2, ff);
		} else if (A <= M_PI/2 && C > M_PI/2) {
			//    -- B    C1
			//  /   /    / |
			// A---C    B1-A1
			double a1 = a;
			double A1 = M_PI/2;
			double B1 = M_PI-C;
			double c1 = side_angle_angle(a1,A1,B1, true).third;
			double ff = 1+(c1/b);
//f.pln("GreatCircle.closest_point_circle b) ff="+ff);
			return interpolate(p1,p2,ff);

//			d1 = side_angle_angle(a,M_PI/2,M_PI-C,true).third;
//f.pln("GreatCircle.closest_point_circle #2 d="+d1+" "+Units.to("deg", d1)+Units.degreeStr);
//			return linear_initial(p1,initial_course(p1,p2),distance_from_angle(b+d1,0)).mkAlt(x.alt());
		} else if (A > M_PI/2 && C <= M_PI/2) {
			// B--		  C1
			//  \   \	  | \
			//   A---C	  A1-B1
			double a1 = c;
			double A1 = M_PI/2;
			double B1 = M_PI-A;
			double c1 = side_angle_angle(a1,A1,B1, true).third;
			double ff = 1+(c1/b);
//f.pln("GreatCircle.closest_point_circle c) ff="+ff);
			return interpolate(p2,p1,ff);
//			d1 = side_angle_angle(a,M_PI/2,M_PI-A,true).third;
//f.pln("GreatCircle.closest_point_circle #3 d="+d1+" "+Units.to("deg", d1)+Units.degreeStr);
//			return linear_initial(p2,initial_course(p2,p1),distance_from_angle(b+d1,0)).mkAlt(x.alt());
		}
//f.pln("GreatCircle.closest_point_circle INVALID: weird triangle");
		return LatLonAlt::INVALID(); // weird triangle
    }
    


//LatLonAlt GreatCircle::closest_point_segment(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x) {
//	double a = angular_distance(x,p2);
//	double b = angular_distance(p1,p2);
//	double c = angular_distance(p1,x);
//	double A = angle_between(p2,p1,x);
//	double B = angle_between(p1,x,p2);
//	double C = angle_between(x,p2,p1);
//
//	// collinear
//	if (Util::within_epsilon(A, 0.000001) || Util::within_epsilon(C, 0.000001) || Util::within_epsilon(M_PI-A, 0.000001) || Util::within_epsilon(M_PI-C, 0.000001)) {
////		if (Util.almost_equals(A, 0.0) || Util.almost_equals(C, 0.0) || Util.almost_equals(A, Math.PI) || Util.almost_equals(C, Math.PI)) {
//		if (b >= a && b >= c) {
//			return x;
//		} else if (a >= b && a >= c) {
//			return p1;
//		} else {
//			return p2;
//		}
//	}
//
//	if (A <= M_PI/2 && C <= M_PI/2) {
//		//   B
//		//  / \
//		// A---C
//		return closest_point_circle(p1,p2,x,a,b,c,A,B,C);
//	} else if (A <= M_PI/2 && C > M_PI/2) {
//		//    -- B
//		//  /   /
//		// A---C
//		return p2;
//	} else {
//		// B--
//		//  \   \
//		//   A---C
//		return p1;
//	}
//}

 /**
  * Given two great circles defined by a1,a2 and b1,b2, return the intersection poin that is closest a1.  Use LatLonAlt.antipode() to get the other value.
  * This assumes that the arc distance between a1,a2 < 90 and b1,b2 < 90
  * This returns an INVALID value if both segments are collinear
  * EXPERIMENTAL
  */
 LatLonAlt GreatCircle::intersection(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2) {
 	Vect3 va = spherical2xyz(a1.lat(), a1.lon()).cross(spherical2xyz(a2.lat(), a2.lon()));
 	Vect3 vb = spherical2xyz(b1.lat(), b1.lon()).cross(spherical2xyz(b2.lat(), b2.lon()));
 	double r = GreatCircle::spherical_earth_radius;
 	Vect3 vavb = va.cross(vb);
 	if (vavb.almostEquals(Vect3::ZERO())) {
 		return LatLonAlt::INVALID();
 	}
 	Vect3 v1 = vavb.Scal(r / vavb.norm());
 	Vect3 v2 = vavb.Scal(-r / vavb.norm());
 	LatLonAlt x1 = xyz2spherical(v1).mkAlt(a1.alt());
 	LatLonAlt x2 = xyz2spherical(v2).mkAlt(a1.alt());
 	if (distance(a1,x1) < distance(a1,x2)) {
 		return x1;
 	} else {
 		return x2;
 	}
 }

	/**
	 * Given two great circles defined by so, so2 and si, si2 return the intersection point that is closest to so.
	 * (Note. because on a sphere there are two intersection points)
	 *  Calculate altitude of intersection using linear extrapolation from line (so,so2)
	 *
	 * @param so     first point of line o
	 * @param so2    second point of line o
	 * @param dto    the delta time between point so and point so2.
	 * @param si     first point of line i
	 * @param si2    second point of line i
	 * @return a pair: intersection point and the delta time from point "so" to the intersection, can be negative if intersect
	 *                 point is in the past. If intersection point is invalid then the returned delta time is -1
	 */
 std::pair<LatLonAlt,double>  GreatCircle::intersectionExtrapAlt(const LatLonAlt& so, const LatLonAlt& so2, double dto, const LatLonAlt& si, const LatLonAlt& si2) {
 	LatLonAlt lgc = GreatCircle::intersection(so, so2, si, si2);
 	if (lgc.isInvalid()) return std::pair<LatLonAlt,double>(lgc,-1.0);
 	double gso = distance(so,so2)/dto;
 	double intTm = distance(so,lgc)/gso;  // relative to so
  	bool behind = GreatCircle::behind(lgc, so, GreatCircle::velocity_average(so, so2, 1.0));
 	if (behind) intTm = -intTm;
 	// compute a better altitude
 	double vs = (so2.alt() - so.alt())/dto;
 	double nAlt = so.alt() + vs*(intTm);
 	LatLonAlt pgc = LatLonAlt::mk(lgc.lat(),lgc.lon(),nAlt);
 	return std::pair<LatLonAlt,double>(pgc,intTm);
 }

 std::pair<LatLonAlt,double>  GreatCircle::intersectionAvgAlt(const LatLonAlt& so, const LatLonAlt& so2, double dto, const LatLonAlt& si, const LatLonAlt& si2) {
		LatLonAlt interSec = GreatCircle::intersection(so, so2, si, si2);
		//f.pln(" %%% GreatCircle.intersection: lgc = "+lgc.toString(15));
		if (interSec.isInvalid()) return std::pair<LatLonAlt,double>(interSec,-1.0);
		double gso = distance(so,so2)/dto;
		double intTm = distance(so,interSec)/gso;  // relative to so
		//f.pln(" ## gso = "+Units.str("kn", gso)+" distance(so,lgc) = "+Units.str("NM",distance(so,lgc)));
		bool behind = GreatCircle::behind(interSec, so, GreatCircle::velocity_average(so, so2, 1.0)); //TODO: initial?
//		f.pln("behind="+behind+" interSec="+interSec+" so="+so+" vo="+GreatCircle.velocity_average(so, so2, 1.0));
		if (behind) intTm = -intTm;
		// compute a better altitude using average of near points
		double do1 = distance(so,interSec);
		double do2 = distance(so2,interSec);
		double alt_o = so.alt();
		if (do2 < do1) alt_o = so2.alt();
		double di1 = distance(si,interSec);
		double di2 = distance(si2,interSec);
		double alt_i = si.alt();
		if (di2 < di1) alt_i = si2.alt();
		double nAlt = (alt_o + alt_i)/2.0;
		//    	f.pln(" $$ LatLonAlt.intersection: so.alt() = "+Units.str("ft",so.alt())+" so2.alt() = "+Units.str("ft",so2.alt())+
		//    			" si.alt() = "+Units.str("ft",si.alt())+" si2.alt() = "+Units.str("ft",si2.alt())+
		//    			" nAlt() = "+Units.str("ft",nAlt));
		//f.pln(" $$ LatLonAlt.intersection: intTm = "+intTm+" vs = "+Units.str("fpm",vs)+" nAlt = "+Units.str("ft",nAlt)+" "+behind);
		LatLonAlt pgc = LatLonAlt::mk(interSec.lat(),interSec.lon(),nAlt);
		return std::pair<LatLonAlt,double>(pgc,intTm);

 }

 std::pair<LatLonAlt,double> GreatCircle::intersection(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi, bool checkBehind) {
 	LatLonAlt so2 = linear_initial(so, vo, 1000);
 	LatLonAlt si2 = linear_initial(si, vi, 1000);
 	LatLonAlt i = intersection(so, so2, si, si2);
 	if (checkBehind) {
    	if (i.isInvalid() || behind(i, so,vo) || behind(i, si,vi)) return std::pair<LatLonAlt,double>(LatLonAlt::INVALID(),-1.0); // collinear or (nearly) same position or cross in the past
 	}
 	double dt = distance(so,i)/vo.gs();
 	if (behind(i, so, vo)) dt = -dt;   // ??? RWB ???
 	return std::pair<LatLonAlt,double>(i,dt);
}

double GreatCircle::angleBetween(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2) {
	Vect3 va = spherical2xyz(a1.lat(), a1.lon()).cross(spherical2xyz(a2.lat(), a2.lon())).Hat(); // normal 1
	Vect3 vb = spherical2xyz(b1.lat(), b1.lon()).cross(spherical2xyz(b2.lat(), b2.lon())).Hat(); // normal 2
	double cosx = va.dot(vb);
	return std::acos(cosx);
}


double GreatCircle::angle_between(const LatLonAlt& a, const LatLonAlt& b, const LatLonAlt& c) {
	double a1 = angular_distance(c,b);
	double b1 = angular_distance(a,c);
	double c1 = angular_distance(b,a);
	double d = std::sin(c1)*std::sin(a1);
	if (d == 0.0) {
		return M_PI;
	}
	return Util::acos_safe( (std::cos(b1)-std::cos(c1)*std::cos(a1)) / d );
}


 /**
  * Return true if x is "behind" ll, considering its current direction of travel, v.
  * "Behind" here refers to the hemisphere aft of ll.
  * That is, x is within the region behind the perpendicular line to v through ll.
  * @param ll aircraft position
  * @param v aircraft velocity
  * @param x intruder positino
  * @return
  */
 bool GreatCircle::behind(const LatLonAlt& x, const LatLonAlt& ll, const Velocity& v) {
 	Velocity v2 = velocity_initial(ll, x, 100);
 	return Util::turnDelta(v.trk(), v2.trk()) > M_PI/2.0;
 }

	/**
	 * Returns values describing if the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si intruder position
	 * @param vi intruder velocity
	 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if collinear or parallel or closest intersection is behind you
	 */
 int GreatCircle::passingDirection(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi) {
 	std::pair<LatLonAlt,double> p = intersection(so,vo,si,vi,true);
 	if (p.second < 0) return 0;
 	LatLonAlt si3 = linear_initial(si,vi,p.second); // intruder position at time of intersection
 	if (behind(p.first, si3, vi)) return -1;
 	return 1;
 }

//    int dirForBehind(LatLonAlt so, Velocity vo, LatLonAlt si, Velocity vi) {
//    	LatLonAlt so2 = linear_initial(so, vo, 1000);
//    	LatLonAlt si2 = linear_initial(si, vi, 1000);
//    	LatLonAlt i = intersection(so, so2, si, si2);
//    	if (i.isInvalid() || behind(so,vo,i) || behind(si,vi,i)) return 0; // collinear or (nearly) same position or cross in the past
//    	double tso = distance(so,i)/vo.gs();
//    	if (behind(so,vo,i)) tso = -tso;
//    	LatLonAlt siXP = linear_initial(si,vi,tso);
//    	int ahead = (behind(siXP,vi,i) ? -1 : 1);
//    	int onRight = Util::sign(cross_track_distance(so,i,siXP));
//fpln("ahead="+ahead+"  onRight="+onRight+" siXP="+siXP.toStringNP(8)+" i="+i.toStringNP(8));
//    	return ahead*onRight;
//    }


 int GreatCircle::dirForBehind(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi) {
 	LatLonAlt so2 = linear_initial(so, vo, 1000);
 	LatLonAlt si2 = linear_initial(si, vi, 1000);
 	LatLonAlt i = intersection(so, so2, si, si2);
 	if (i.isInvalid() || behind(i,so,vo) || behind(i,si,vi)) return 0; // collinear or (nearly) same position or cross in the past
 	int onRight = Util::sign(cross_track_distance(si,si2,so));
 	return -onRight;
 }



  Velocity GreatCircle::velocity_initial(const LatLonAlt& p1, const LatLonAlt& p2, double t) {
    // p1 is the source position, p2 is another point to form a great circle
    // positive time is moving from p1 toward p2
    // negative time is moving from p1 away from p2
    if (std::abs(t) < minDt || Util::almost_equals(std::abs(t) + minDt, minDt,
			      PRECISION7)) {
      // time is negative or very small (less than 1 ms)
      return Velocity::ZEROV();
    }
    double d = angular_distance(p1, p2);
    if (Constants::almost_equals_radian(d)) {
      if (Constants::almost_equals_alt(p1.alt(), p2.alt())) {
	// If the two points are about 1 meter apart, then count them as
	// the same.
	return Velocity::ZEROV();
      } else {
	return Velocity::ZEROV().mkVs((p2.alt() - p1.alt()) / t);
      }
    }
    double gs = GreatCircle::distance_from_angle(d, 0.0) / t;
    double crs = initial_course_impl(p1, p2, d);
    return Velocity::mkTrkGsVs(crs, gs, (p2.alt() - p1.alt()) / t);
  }

  Velocity GreatCircle::velocity_average(const LatLonAlt& p1, const LatLonAlt& p2, double t) {
    // p1 is the source position, p2 is another point on that circle
    // positive time is moving from p1 toward p2
    // negative time is moving from p1 away from p2
    if (t >= 0.0) {
      return GreatCircle::velocity_initial(GreatCircle::interpolate(p1, p2, 0.5), p2, t / 2.0);
    } else {
      return GreatCircle::velocity_average(p1, GreatCircle::interpolate(p1, p2, -1.0), -t);
    }
  }

  Velocity GreatCircle::velocity_average_speed(const LatLonAlt& s1, const LatLonAlt& s2, double speed) {
	  double dist = GreatCircle::distance(s1, s2);
	  double dt = dist/speed;
	  return GreatCircle::velocity_average(s1, s2, dt);
  }


  Velocity GreatCircle::velocity_final(const LatLonAlt& p1, const LatLonAlt& p2, double t) {
    // p1 is the source position, p2 is another point on that circle
    // positive time is moving from p1 toward p2
    // negative time is moving from p1 away from p2 (final velocity is the
    // velocity at that time)
    if (t >= 0.0) {
      return GreatCircle::velocity_initial(p2, p1, -t);
    } else {
      return GreatCircle::velocity_initial(GreatCircle::interpolate(p1, p2, -1.0), p1, t);
    }
  }



  Vect3 GreatCircle::spherical2xyz(double lat, double lon) {
  	double r = GreatCircle::spherical_earth_radius;
  	// convert latitude to 0-PI
  	double theta = M_PI/2 - lat;
  	double phi = lon; //M_PI - lon;
  	double x = r*std::sin(theta)*std::cos(phi);
  	double y = r*std::sin(theta)*std::sin(phi);
  	double z = r*std::cos(theta);
  	return Vect3(x,y,z);
  }

  Vect3 GreatCircle::spherical2xyz(const LatLonAlt& lla) {
    return spherical2xyz(lla.lat(), lla.lon());
  }

  LatLonAlt GreatCircle::xyz2spherical(const Vect3& v) {
  	double r = GreatCircle::spherical_earth_radius;
  	double theta = Util::acos_safe(v.z/r);
  	double phi = Util::atan2_safe(v.y, v.x);
  	double lat = M_PI/2 - theta;
  	double lon = Util::to_pi(phi); //M_PI - phi);
  	return LatLonAlt::mk(lat, lon, 0);
  }



  
	double GreatCircle::chord_distance(double lat1, double lon1, double lat2, double lon2) {
		Vect3 v1 = spherical2xyz(lat1,lon1);
		Vect3 v2 = spherical2xyz(lat2,lon2);
		return v1.Sub(v2).norm();
	}


	double GreatCircle::chord_distance(double surface_dist) {
		double theta = angle_from_distance(surface_dist,0.0);
		return 2.0*sin(theta/2.0)*GreatCircle::spherical_earth_radius;
	}
	

  double GreatCircle::surface_distance(double chord_distance) {
    double theta = 2.0*Util::asin_safe(chord_distance*0.5 / GreatCircle::spherical_earth_radius);
		return distance_from_angle(theta,0.0);
	}


  LatLonAlt GreatCircle::small_circle_rotation(const LatLonAlt& so, const LatLonAlt& center, double angle) {
    if (Util::almost_equals(angle, 0)) return so;
    double R = angular_distance(so, center);
    Triple<double,double,double>ABc = side_angle_side(R, angle, R);
    double A = ABc.first;
    double c = distance_from_angle(ABc.third, 0.0);
    double crs = initial_course(so, center);
    if (crs > M_PI) crs = crs-2*M_PI;
    double trk = Util::to_2pi(crs - A);
    LatLonAlt ret = linear_initial(so, trk, c);
    return ret;
  }

	/**
	 * Accurately calculate the linear distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcAngle angular (radian) length of the arc.  This is the angle between two great circles that intersect at the small circle's center.
	 * @return linear distance of the small circle arc
	 * Note: A 100 km radius turn over 60 degrees produces about 4.3 m error.
	 */
	double GreatCircle::small_circle_arc_length(double radius, double arcAngle) {
		// use the chord of the diameter to determine the radius in the ECEF Euclidean frame
		double r2 = chord_distance(radius*2)/2;
		// because this is a circle in a Euclidean frame, use the normal calculations
		return arcAngle*r2;
	}

	/**
	 * Accurately calculate the angular distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcLength linear (m) length of the arc.  This is the along-line length of the arc.
	 * @return Angular distance of the arc around the small circle (from 0 o 2pi)
	 * Note: A 100 km radius turn over 100 km of turn produces about 0.0024 degrees of error.
	 */
	double GreatCircle::small_circle_arc_angle(double radius, double arcLength) {
		// use the chord of the diameter to determine the radius in the ECEF Euclidean frame
		double r2 = chord_distance(radius*2)/2;
		if (r2 == 0.0) return 0.0;
		// because this is a circle in a Euclidean frame, use the normal calculations
		return arcLength/r2;
	}

}
