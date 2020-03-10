/*
 * Vect4.cpp
 *
 * 4-D vectors.
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect4.h"

namespace larcfm {

  Vect4::Vect4(const double xx, const double yy, const double zz, const double tt) : x(xx), y(yy), z(zz), t(tt) {
  }

  Vect4::Vect4(const Vect3& v, double vt) : x(v.x), y(v.y), z(v.z), t(vt) {
  }

  bool Vect4::isZero() const {
    return x == 0.0 && y == 0.0 && z == 0.0 && t == 0.0;
  }

  bool Vect4::almostEquals(const Vect4& v) const {
    return Util::almost_equals(x,v.x) && Util::almost_equals(y,v.y) && Util::almost_equals(z,v.z) && Util::almost_equals(t,v.t);
  }

  bool Vect4::almostEquals(const Vect4& v, INT64FM maxUlps) const {
    return Util::almost_equals(x,v.x,maxUlps) && Util::almost_equals(y,v.y,maxUlps) && Util::almost_equals(z,v.z,maxUlps) && Util::almost_equals(t,v.t,maxUlps);
  }
  
  Vect4 Vect4::operator + (const Vect4& v) const {
	  return this->Add(v);
  }

  Vect4 Vect4::operator - (const Vect4& v) const {
	  return this->Sub(v);
  }

  Vect4 Vect4::operator - () const {
    return Vect4(-x,-y,-z,-t);
  }

  Vect4 Vect4::operator * (const double k) const {
	  return this->Scal(k);
  }

  double Vect4::operator * (const Vect4& v) const { // Dot product
    return dot(v.x,v.y,v.z,v.t);
  }

  bool Vect4::operator == (const Vect4& v) const {  // strict equality
    return x==v.x && y==v.y && z==v.z && t==v.t;
  }

  bool Vect4::operator != (const Vect4& v) const {  // strict disequality
    return x!=v.x || y!=v.y || z!=v.z || t!=v.t;
  }

  Vect2 Vect4::vect2() const {
    return Vect2(x,y);
  }


  Vect3 Vect4::vect3() const {
    return Vect3(x,y,z);
  }

  Vect4 Vect4::Add(const Vect4& v) const {
	  return Vect4(x+v.x,y+v.y,z+v.z,t+v.t);
  }

  Vect4 Vect4::Sub(const Vect4& v) const {
	  return Vect4(x-v.x,y-v.y,z-v.z,t-v.t);
  }

  Vect4 Vect4::Neg() const {
      return Vect4(-x,-y,-z,-t);
  }

  Vect4 Vect4::Scal(double k) const {
	  return Vect4(k*x,k*y,k*z,k*t);
  }

  Vect4 Vect4::ScalAdd(const double k, const Vect4& v) const {
	  return Vect4(k*x+v.x,k*y+v.y,k*z+v.z,k*t+v.t);
  }

  double Vect4::dot(const double x, const double y, const double z, const double t) const {
    return this->x*x + this->y*y + this->z*z + this->t*t;
  }

  double Vect4::sqv() const {
    return dot(x,y,z,t);
  }

  double Vect4::norm() const {
    return sqrt_safe(sqv());
  }
    

  double Vect4::lat() const {
    return y;
  }
  
  
  double Vect4::lon() const {
    return x;
  }
  
  
  std::string Vect4::toString() const {
    std::stringstream temp;
    temp << "(" << x << ", " << y << ", " << z << "," << t << ")";
    return temp.str();
}

  const Vect4& Vect4::ZERO() {
		static Vect4* v = new Vect4(0,0,0,0);
		return *v;
  }


}
