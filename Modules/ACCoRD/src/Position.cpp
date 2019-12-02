/* A single position represented in either Euclidean or Lat/Lon coordinates
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Position.h"
#include "Point.h"
#include "LatLonAlt.h"
//#include "UnitSymbols.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "Constants.h"
#include "VectFuns.h"
#include "Util.h" // NaN def
#include "format.h"
#include "string_util.h"
#include <memory>

namespace larcfm {
using std::string;
using std::cout;
using std::endl;


const double minDist = 1E-9;

// the follow is to avoid the static initialization fiasco

const Position& Position::INVALID() {
	static Position* pos = new Position(NaN, NaN, NaN);
	return *pos;
}
const Position& Position::ZERO_LL() {
	static Position* pos = new Position(LatLonAlt::ZERO());
	return *pos;
}
const Position& Position::ZERO_XYZ() {
	static Position* pos = new Position(Vect3::ZERO());
	return *pos;
}


Position::Position() : ll(LatLonAlt::ZERO()) , s3(Point::ZEROP()) {
	latlon = true;
}

Position::Position(const LatLonAlt& lla) : ll(lla) , s3(lla.lon(), lla.lat(), lla.alt()) {
	latlon = true;
}

Position::Position(double x, double y, double z) : ll(LatLonAlt::mk(y, x, z)) , s3(x, y, z) {
	latlon = false;
}

Position::Position(const Vect3& v) : ll(LatLonAlt::mk(v.y, v.x, v.z)) , s3(v.x, v.y, v.z) {
	latlon = false;
}

Position::Position(const Position& p) : ll(LatLonAlt::mk(p.lat(), p.lon(), p.alt())) , s3(p.x(), p.y(), p.z()) {
	latlon = p.latlon;
}


Position Position::makeLatLonAlt(double lat, double lon, double alt) {
	return Position(LatLonAlt::make(lat,lon,alt));
}

Position Position::mkLatLonAlt(double lat, double lon, double alt) {
	return Position(LatLonAlt::mk(lat,lon,alt));
}

Position Position::makeLatLonAlt(double lat, std::string lat_unit, double lon, std::string lon_unit, double alt, std::string alt_unit) {
	return Position(LatLonAlt::make(lat, lat_unit, lon, lon_unit, alt, alt_unit));
}

Position Position::makeXYZ(double x, double y, double z) {
	return Position(Units::from("nm", x), Units::from("nm", y), Units::from("ft",z));
}


Position Position::mkXYZ(double x, double y, double z) {
	return Position(x,y,z);
}


Position Position::makeXYZ(double x, std::string x_unit, double y, std::string y_unit, double z, std::string z_unit) {
	return Position(Units::from(x_unit, x), Units::from(y_unit, y), Units::from(z_unit,z));
}


bool Position::almostEquals(const Position& v) const {
	if (latlon) {
	  return GreatCircle::almostEquals(lla(),v.lla());
	} else
		return Constants::almost_equals_xy(s3.x,s3.y,v.s3.x,v.s3.y)
	&& Constants::almost_equals_alt(s3.z,v.s3.z);
}


bool Position::almostEquals(const Position& pp, double epsilon_horiz, double epsilon_vert) const {
	if (latlon) {
	  return GreatCircle::almostEquals(lla(),pp.lla(),epsilon_horiz,epsilon_vert);
	} else {
		return s3.within_epsilon(pp.vect3(),epsilon_vert);
	}
}

bool Position::almostEquals2D(const Position& pp, double epsilon_horiz) const {
	if (latlon) {
	  return GreatCircle::almostEquals2D(lla(),pp.lla(),epsilon_horiz);
	} else {
		return s3.almostEquals2D(pp.vect3(),epsilon_horiz);
	}
}



bool Position::operator == (const Position& v) const {  // strict equality
	return s3.x==v.s3.x && s3.y==v.s3.y && s3.z==v.s3.z && latlon == v.latlon && ll.lat()==v.ll.lat() && ll.lon()==v.ll.lon() && ll.alt()==v.ll.alt();
}


bool Position::operator != (const Position& v) const {  // strict disequality
	return s3.x!=v.s3.x || s3.y!=v.s3.y || s3.z!=v.s3.z || latlon != v.latlon;
}


bool Position::isInvalid() const {
	return s3.isInvalid() || ll.isInvalid();
}

Vect2 Position::vect2() const {
	return Vect2(s3.x,s3.y);
}

const Point& Position::vect3() const {
	return s3;
}

const LatLonAlt& Position::lla() const {
	return ll;
}

double Position::x() const {
	return s3.x;
}

double Position::y() const {
	return s3.y;
}

double Position::z() const {
	return s3.z;
}

double Position::lat() const {
	return ll.lat();
}

double Position::lon() const {
	return ll.lon();
}

double Position::alt() const {
	return ll.alt();
}

double Position::latitude() const {
	return ll.latitude();
}

double Position::longitude() const {
	return ll.longitude();
}

double Position::altitude() const {
	return ll.altitude();
}

double Position::xCoordinate() const {
	return Units::to("nm", s3.x);
}

double Position::yCoordinate() const {
	return Units::to("nm", s3.y);
}

double Position::zCoordinate() const {
	return Units::to("ft", s3.z);
}


bool Position::isLatLon() const {
	return latlon;
}

const Position Position::mkX(double x) const {
	if (latlon) {
		return Position(LatLonAlt::mk(ll.lat(), x, ll.alt()));
	} else {
		return Position(x, s3.y, s3.z);
	}
}

const Position Position::mkLon(double lon) const {
	return mkX(lon);
}

const Position Position::mkY(double y) const {
	if (latlon) {
		return Position(LatLonAlt::mk(y, ll.lon(), ll.alt()));
	} else {
		return Position(s3.x, y, s3.z);
	}
}

const Position Position::mkLat(double lat) const {
	return mkY(lat);
}

const Position Position::mkZ(double z) const {
	if (latlon) {
		return Position(LatLonAlt::mk(ll.lat(), ll.lon(), z));
	} else {
		return Position(s3.x, s3.y, z);
	}
}

const Position Position::mkAlt(double alt) const {
	return mkZ(alt);
}

const Position Position::zeroAlt() const {
	return mkZ(0);
}

double Position::distanceH(const Position& p) const {
	if (latlon) {
		return GreatCircle::distance(ll,p.ll);
	} else {
		return s3.vect2().Sub(p.vect2()).norm();
	}
}

double Position::distanceV(const Position& p) const {
	return std::abs(s3.z - p.s3.z);
}

double Position::signedDistanceV(const Position& p) const {
	return s3.z - p.s3.z;
}


const Position Position::linear(const Velocity& v, double time) const {
	if (time == 0 || v.isZero()) {
		return latlon ? Position(ll) : Position(s3);
	}
	if (latlon) {
		return Position(GreatCircle::linear_initial(ll,v,time));
	} else {
		return Position(s3.linear(v,time));
	}
}

const Position Position::linearEst(double dn, double de) const {
	Position newNP;
	if (latlon) {
		newNP = Position(lla().linearEst(dn,de));
	} else {
		return Position(Point::mk(s3.x + de, s3.y + dn, s3.z));
	}
	return newNP;
}


const Position Position::linearEst(const Velocity& vo, double time) const {
	Position newNP;
	if (latlon) {
		if (lat() > Units::from("deg",85) || lat() < Units::from("deg",-85)) {
			newNP = Position (GreatCircle::linear_initial(ll,vo,time));
		} else {
			newNP = Position(lla().linearEst(vo,time));
		}
	} else {
		newNP = linear(vo,time);
	}
	return newNP;
}

//const std::pair<Position,Velocity> Position::linearDist2D(const Velocity& v, double d) const {
//	  double track = v.trk();
//	  //f.pln(" $$$$$$ linearDist: v.track = "+Units::str("deg",v.compassAngle()));
//	  if (latlon) {
//		  LatLonAlt sEnd = GreatCircle::linear_initial(ll,track,d);
//		  //sEnd = sEnd.mkAlt(altAtd);
//		  double finalTrk = track;
//		  if (d > minDist) {  // final course has problems if no distance between points (USE 1E-9), 1E-10 NOT GOOD
//		     finalTrk = GreatCircle::final_course(ll, sEnd);
//		  }
//		  //f.pln(" $$$$$$ linearDist: v = "+v+" finalTrk = "+Units::str("deg",finalTrk)+" d = "+Units::str("ft",d));
//		  Velocity vEnd = v.mkTrk(finalTrk);
//		  return std::pair<Position,Velocity>(Position(sEnd),vEnd);
//	  } else {
//		  //Velocity vEnd = Velocity.mkTrkGsVs(track,fakeGs,0.0);
//		  Vect3 sNew = s3.linearByDist2D(track,d);
//		  //sNew = sNew.mkZ(altAtd);
//		  return std::pair<Position,Velocity>(Position(sNew),v);
//	  }
// }

const std::pair<Position,Velocity> Position::linearDist2D(double track, double d, double gsAt_d) const {
	  //f.pln(" $$$$$$ linearDist: v.track = "+Units::str("deg",v.compassAngle()));
	  if (latlon) {
		  LatLonAlt sEnd = lla();
		  double finalTrk = track;
		  if (d > minDist) {  // final course has problems if no distance between points (USE 1E-9), 1E-10 NOT GOOD
			  sEnd = GreatCircle::linear_initial(ll,track,d);
			  finalTrk = GreatCircle::final_course(ll, sEnd);
		  }
		  //f.pln(" $$$$$$ linearDist: v = "+v+" finalTrk = "+Units::str("deg",finalTrk)+" d = "+Units::str("ft",d));
		  Velocity vEnd = Velocity::mkTrkGsVs(finalTrk,gsAt_d,0.0);
		  return std::pair<Position,Velocity>(Position(sEnd),vEnd);
	  } else {
		  Vect3 sNew = s3.linearByDist2D(track,d);
		  Velocity vNew = Velocity::mkTrkGsVs(track,gsAt_d,0.0);
		  return std::pair<Position,Velocity>(Position(sNew),vNew);
	  }

}

const Position Position::linearDist2D(double track, double d) const {
	  if (latlon) {
		  LatLonAlt sEnd = GreatCircle::linear_initial(ll,track,d);
		  return Position(sEnd);
	  } else {
	      Vect3 sEnd = s3.linearByDist2D(track, d);
	      return Position(sEnd);
	  }
}


const Position Position::midPoint(const Position& p2) const{
	if (latlon) {
		return Position(GreatCircle::interpolate(ll,p2.lla(),0.5));
	} else {
		return Position(VectFuns::midPoint(s3,p2.vect3()));
	}
}




//  const Position Position::relativeProjection(const Position& si) const {
//    if (latlon) {
//      Vect3 s3 = larcfm::Projection::createProjection(si.lla()).project(lla());
//      return Position(LatLonAlt::mk(s3.y, s3.x, s3.z));
//    } else {
//      return Position(s3 - si.point());
//    }
//  }

Position Position::interpolate(const Position& p2, double f) const {
  if (latlon) {
    return Position(GreatCircle::interpolate(ll,p2.lla(),f));
  } else {
    return Position(Point::mk(VectFuns::interpolate(s3,Point::mk(p2.vect3()),f)));
  }
}

double Position::track(const Position& p) const {
	if (p.latlon != latlon) {
		fdln("Position.track call given an inconsistent argument.");
		return 0;
	}
	if (latlon) {
		return GreatCircle::initial_course(ll,p.ll);
	} else {
		Vect2 v = p.s3.Sub(s3).vect2();
		return v.compassAngle();
	}
}

Velocity Position::initialVelocity(const Position& p, double dt) const {
	if (dt<=0) {
		return Velocity::ZEROV();
	} else {
		if (isLatLon()) {
			return GreatCircle::velocity_initial(lla(), p.lla(), dt);
		} else {
			return Velocity::make((p.vect3().Sub(vect3())).Scal(1.0/dt));
		}
	}
}

Velocity Position::finalVelocity(const Position& p, double dt) const {
	if (dt<=0) {
		return Velocity::ZEROV();
	} else {
		if (isLatLon()) {
			return GreatCircle::velocity_final(lla(), p.lla(), dt);
		} else {
			return Velocity::make((p.vect3().Sub(vect3())).Scal(1.0/dt));
		}
	}
}






double Position::representativeTrack(const Position& p) const {
	if (p.latlon != latlon) {
		fdln("Position.representativeTrack call given an inconsistent argument.");
		return 0.0;
	}
	if (latlon) {
		return GreatCircle::representative_course(ll,p.ll);
	} else {
		Vect2 v = p.s3.Sub(s3).vect2();
		return v.compassAngle();
	}
}


// returns intersection point and time of intersection relative to position so
// a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
std::pair<Position,double> Position::intersection(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi) {
	if (so.latlon != si.latlon) {
		fdln("Position.intersection call was given an inconsistent argument.");
		return std::pair<Position,double>(Position::INVALID(),-1.0);
	}
	if (so.latlon) {
		std::pair<LatLonAlt,double> pgc = GreatCircle::intersection(so.lla(),vo, si.lla(),vi);
		return std::pair<Position,double>(Position(pgc.first),pgc.second );
	} else {
		std::pair<Vect3,double> pvt = VectFuns::intersection(so.vect3(),vo,si.vect3(),vi);
		return std::pair<Position,double>(Position(pvt.first),pvt.second );
	}
}


/** Returns intersection point and time of intersection relative to the time of position so
 *  for time return value, it assumes that an aircraft travels from so1 to so2 in dto seconds and the other aircraft from si to si2
 *  a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
 */
std::pair<Position,double> Position::intersection(const Position& so, const Position& so2, double dto, const Position& si, const Position& si2) {
	if (so.latlon != si.latlon && so2.latlon != si2.latlon && so.latlon != so2.latlon) {
		fdln("Position.intersection call was given an inconsistent argument.");
		return std::pair<Position,double>(Position::INVALID(),-1.0);
	}
	if (so.latlon) {
		std::pair<LatLonAlt,double> lgc = GreatCircle::intersectionAvgAlt(so.lla(),so2.lla(), dto, si.lla(),si2.lla());
		return std::pair<Position,double>(Position(lgc.first),lgc.second);
	} else {
		std::pair<Vect3,double> pvt = VectFuns::intersectionAvgZ(so.vect3(),so2.vect3(),dto,si.vect3(),si2.vect3());
		return std::pair<Position,double>(Position(pvt.first),pvt.second);
	}
}


/** Returns intersection point and time of intersection relative to the time of position so
 *  for time return value, it assumes that an aircraft travels from so1 to so2 in dto seconds and the other aircraft from si to si2
 *  a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
 */
Position Position::intersection2D(const Position& so, const Position& so2, const Position& si, const Position& si2) {
	if (so.latlon != si.latlon && so2.latlon != si2.latlon && so.latlon != so2.latlon) {
		fpln("Position.intersection call was given an inconsistent argument.");
		return Position::INVALID();
	}
	if (so.latlon) {
		LatLonAlt lgc = GreatCircle::intersection(so.lla(),so2.lla(), si.lla(), si2.lla());
		return Position(lgc);
	} else {
		std::pair<Vect2,double> pvt = VectFuns::intersection2D(so.vect2(),so2.vect2(),1.0,si.vect2(),si2.vect2());
		Vect3 p3 = Vect3(pvt.first,so.z());
		return Position(p3);
	}

}





//  Velocity Position::averageVelocity(const Position& p2, double speed) {
//	  if (p2.latlon != latlon) {
//		  fdln("Position.averageVelocity call given an inconsistent argument.");
//		  return Velocity::ZEROV();
//	  }
//	  if (latlon) {
//		  return GreatCircle::velocity_average_speed(ll,p2.ll,speed);
//	  } else {
//		  return Velocity::makeVel(s3,p2.s3,speed);
//	  }
//}

/** Return the average velocity between the current position and the given position, with the given speed [internal units]. */
Velocity Position::averageVelocity(const Position& p2, double speed) const {
  if (p2.latlon != latlon) {
	fpln("Position.averageVelocity call given an inconsistent argument.");
    return Velocity::ZEROV();
  }
  if (latlon) {
    return GreatCircle::velocity_average_speed(ll,p2.ll,speed);
  } else {
    return Velocity::mkVel(s3,p2.s3,speed);
  }
}

/** Return the average velocity between the current position and the given position, with the given delta time dt. */
Velocity Position::avgVelocity(const Position& p2, double dt) const {
  if (p2.latlon != latlon) {
    fpln("Position.averageVelocity call given an inconsistent argument.");
    return Velocity::ZEROV();
  }
  if (latlon) {
    return GreatCircle::velocity_average(ll,p2.ll,dt);
  } else {
    return Velocity::genVel(s3,p2.s3,dt);
  }

}

bool Position::isWest(const Position& a) const {
	  if (isLatLon()) {
		  return lla().isWest(a.lla());
	  } else {
		  return x() < a.x();
	  }
}


bool Position::LoS(const Position& p2, double D, double H) {
	if (p2.isInvalid()) return false;
	if (p2.latlon != latlon) {
		fdln("Position.LoS call given an inconsistent argument: "+toString()+" "+p2.toString());
		return false;
	}
	double distH = distanceH(p2);
	double distV = distanceV(p2);
	//f.pln ("distH "+distH+"  distV "+distV+" D "+D+" H "+H);
	return (distH < D && distV < H);
}

bool Position::collinear(Position p1, Position p2) const {
	if (latlon)
		return GreatCircle::collinear(lla(),p1.lla(),p2.lla());
	else
		return VectFuns::collinear(vect3(),p1.vect3(),p2.vect3());
}


std::string Position::toUnitTest() const{
	if (latlon) {
		return "Position.makeLatLonAlt("+ Fm12(Units::to("deg",lat()))
		       +", "+Fm16(Units::to("deg",lon()))+", "+Fm12(Units::to("ft",alt()))+")";
	} else {
		return "Position.makeXYZ("+(Fm8(Units::to("NM",x()))+", "+Fm8(Units::to("NM",y()))
		       +", "	+Fm8(Units::to("ft",z()))+")");
	}
}

std::string Position::toUnitTestSI() const {
	if (latlon) {
		return "Position.mkLatLonAlt("+ Fm12(lat())+", "+Fm12(lon())+", "+Fm12(alt())+")";
	} else {
		return "Position.mkXYZ("+(Fm8(x())+", "+Fm8(y())+", "+Fm8(z())+")");
	}
}


std::string Position::toString() const {
	return toString(Constants::get_output_precision());
}

std::string Position::toString(int prec) const {
	if (latlon)
		return "("+Units::str("deg",ll.lat(),prec)+", "+Units::str("deg",ll.lon(),prec)+", "+Units::str("ft",ll.alt(),prec)+")";
	else
		return "("+Units::str("NM",s3.x,prec)+", "+Units::str("NM",s3.y,prec)+", "+Units::str("ft",s3.z,prec)+")";
}

std::string Position::toString2D(int prec) const {
	if (latlon)
		return "("+Units::str("deg",ll.lat(),prec)+", "+Units::str("deg",ll.lon(),prec)+")";
	else
		return "("+Units::str("NM",s3.x,prec)+", "+Units::str("NM",s3.y,prec)+")";
}


/**
 * Return a string representation using the given unit conversions (latitude and longitude,
 * if appropriate, are always in degrees, so only the z unit is used in that case)
 */
std::string Position::toStringUnits() const {
	return toStringUnits("NM", "NM", "ft");
}

/**
 * Return a string representation using the given unit conversions (latitude and longitude,
 * if appropriate, are always in degrees,
 * so only the z unit is used in that case).
 */
std::string Position::toStringUnits(const string& xUnits, const string& yUnits, const string& zUnits) const {
	if (latlon)
		return "("+Units::str("deg",ll.lat())+", "+Units::str("deg",ll.lon())+", "+Units::str(zUnits,ll.alt())+")";
	else
		return "("+Units::str(xUnits,s3.x)+", "+Units::str(yUnits,s3.y)+", "+Units::str(zUnits,s3.z)+")";
}

std::string Position::toStringNP() const {
	return toStringNP(Constants::get_output_precision());
}

std::string Position::toStringNP(int precision) const {
	if (latlon)
		return ll.toString(precision);
	else
		return s3.toStringNP(precision,"NM","NM","ft");
}

std::vector<std::string> Position::toStringList() const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else if (latlon) {
		ret.push_back(Fm12(ll.latitude()));
		ret.push_back(Fm12(ll.longitude()));
		ret.push_back(Fm12(ll.altitude()));
	} else {
		ret.push_back(Fm12(Units::to("NM",s3.x)));
		ret.push_back(Fm12(Units::to("NM",s3.y)));
		ret.push_back(Fm12(Units::to("ft",s3.z)));
	}
	return ret;
}

std::vector<std::string> Position::toStringList(int precision) const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else if (latlon) {
		ret.push_back(FmPrecision(ll.latitude(),precision));
		ret.push_back(FmPrecision(ll.longitude(),precision));
		ret.push_back(FmPrecision(ll.altitude(),precision));
	} else {
		ret.push_back(FmPrecision(Units::to("NM",s3.x),precision));
		ret.push_back(FmPrecision(Units::to("NM",s3.y),precision));
		ret.push_back(FmPrecision(Units::to("ft",s3.z),precision));
	}
	return ret;
}

/** This interprets a string as a LatLonAlt position with units of deg/deg/ft or in the specified units (inverse of toString()) */
const Position Position::parseLL(const std::string& s) {
	return Position(LatLonAlt::parse(s));
}

/** This interprets a string as a XYZ position with units of nmi/nmi/ft or in the specified units (inverse of toString()) */
const Position Position::parseXYZ(const std::string& s) {
	Point v = Point::parse(s); // Vect3.parse assumes internal units
	//return makeXYZ(v.x, v.y, v.z);
	return Position(v);
}

const Position Position::parse(const std::string& s) {
	std::vector<std::string> fields = split(s, Constants::wsPatternParens);
	while (fields.size() > 0 && equals(fields[0], "")) {
		fields.erase(fields.begin());
	}
	if (fields.size() == 6) {
		if (Units::isCompatible(Units::clean(fields[1]), "deg") && Units::isCompatible(Units::clean(fields[3]), "deg") && Units::isCompatible(Units::clean(fields[5]), "ft")) return parseLL(s);
		if (Units::isCompatible(Units::clean(fields[1]), "m") && Units::isCompatible(Units::clean(fields[3]), "m") && Units::isCompatible(Units::clean(fields[5]), "m")) return parseXYZ(s);
	}
	return Position::INVALID();

}

}
