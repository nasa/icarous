/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <vector>
#include "MovingPolygon2D.h"
#include "format.h"

namespace larcfm {

  
//
//  static class BadPolygonException extends Exception {
//    private static final long serialVersionUID = 1L;
//    BadPolygonException() {
//      super();
//    }
//    BadPolygonException(std::string s) {
//      super(s);
//    }
//  }
  
  MovingPolygon2D::MovingPolygon2D() {
    knownStable = false;
    knownUnstable = false;
  }

  MovingPolygon2D::MovingPolygon2D(std::vector<Vect2> polystart_v, std::vector<Vect2> polyvel_v, double tend_d) {
    //super();
    polystart = polystart_v;
    polyvel = polyvel_v;
    tend = tend_d;
    knownStable = false;
    knownUnstable = false;
  }
  
  
  MovingPolygon2D::MovingPolygon2D(const Poly2D& polygon, const Vect2& polyvel_d, double tend_d) {
       //super();
	   for (int i = 0; i < (int) polygon.size(); i++) {
           Vect2 v = polygon.get(i); //    getVerticesRef()[i];
           polystart.push_back(v);
           polyvel.push_back(polyvel_d);              // same velocity vector for all vertices
       }
	   tend = tend_d;
	    knownStable = false;
	    knownUnstable = false;
  }

  MovingPolygon2D::MovingPolygon2D(const Poly2D& polygon, const std::vector<Vect2>& vels, double tend_d) {
       //super();
	   for (int i = 0; i < (int) polygon.size(); i++) {
           Vect2 v = polygon.get(i); // getVerticesRef()[i];
           Vect2 vv = vels[i];
           polystart.push_back(v);
           polyvel.push_back(vv);              // same velocity vector for all vertices
       }
	   tend = tend_d;
	    knownStable = false;
	    knownUnstable = false;
  }

  
  Poly2D MovingPolygon2D::position(double t) const {
	  std::vector<Vect2> vs = std::vector<Vect2>();
	  for (int i = 0; i < (int) polystart.size(); i++) {
		  vs.push_back(polystart[i].AddScal(t, polyvel[i]));
	  }
	  return Poly2D(vs);
  }
  
  bool MovingPolygon2D::isStable() const {
	  if (polyvel.size() < 2) return true;
	  if (knownStable) return true;
	  if (knownUnstable) return false;
	  Vect2 base = polyvel[0];
	  for (int i = 1; i < (int) polyvel.size(); ++i) {
		  if (!polyvel[i].almostEquals(base)) {
			  knownUnstable = true;
			  return false;
		  }
	  }
	  knownStable = true;
	  return true;
  }

  int MovingPolygon2D::size() const {
    return polystart.size();
  }
  
  std::vector<Vect2> MovingPolygon2D::getPolyvel() const {
     return polyvel;
  }

  std::vector<Vect2> MovingPolygon2D::getPolystart() const {
     return polystart;
  }

  double MovingPolygon2D::getTend() const {
	  return tend;
  }


//  std::string MovingPolygon2D::toStringOLD() const {
//	  return "vertices="+f.Fobj(polystart)+" vels="+f.Fobj(polyvel)+" tend="+tend;
//  }
  
  std::string MovingPolygon2D::toString() const {
	  std::string rtn = "vertices = [";
	  //for (Vect2 s: polystart) {
	  for (int i = 0; i < (int) polystart.size(); i++) {
		  Vect2 s = polystart[i];
		  rtn = rtn + s.toString();
	  }
	  rtn = rtn +"]\n                     vels= ";
	  //for (Vect2 v: polyvel) {
	  for (int j = 0; j < (int) polyvel.size(); j++) {
		  Vect2 v = polyvel[j];
	       rtn = rtn + v.toString();
	  }
	  rtn = rtn + " tend = "+Fm2(tend);
	  return rtn;
  }

	MovingPolygon2D MovingPolygon2D::reverseOrder() const {
		std::vector<Vect2> p;
		std::vector<Vect2> v;
		for (int i = size()-1; i >= 0; i--) {
			p.push_back(polystart[i]);
			v.push_back(polyvel[i]);
		}
		return MovingPolygon2D(p,v,tend);
	}


}
