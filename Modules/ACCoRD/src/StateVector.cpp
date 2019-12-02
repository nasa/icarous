/*
 * Copyright (c) 2013-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect3.h"
#include "Velocity.h"
#include "StateVector.h"

namespace larcfm {


   StateVector::StateVector(const Vect3& xx, const Velocity& yy, const double zz) : ss(xx), vv(yy), tt(zz) {
   }

   StateVector::StateVector(const std::pair<Vect3,Velocity>& sv, double t ) : ss(sv.first), vv(sv.second), tt(t) {
   }


   StateVector StateVector::makeXYZ(double x, double y, double z, double trk, double gs, double vs, double t){
		Vect3 s = Vect3::makeXYZ(x,"nm",y,"nm",z,"ft");
		Velocity v = Velocity::makeTrkGsVs(trk,gs,vs);
		return StateVector(s,v,t);
	}


   std::pair<Vect3,Velocity> StateVector::pair() const {
       return std::pair<Vect3,Velocity>(ss,vv);
   }

   Vect3 StateVector::s() const {
       return ss;
   }

   Velocity StateVector::v() const {
       return vv;
   }

   double StateVector::t() const {
       return tt;
   }

}
