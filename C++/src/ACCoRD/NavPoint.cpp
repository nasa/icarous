/*
 * NavPoint.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "NavPoint.h"
#include "LatLonAlt.h"
//#include "UnitSymbols.h"
#include "GreatCircle.h"
#include "Constants.h"
#include "format.h"
#include "Util.h" // NaN definition
#include "string_util.h"
#include <stdexcept>
#include <algorithm>

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;

static const bool NavPoint_DEBUG = false;

const NavPoint& NavPoint::ZERO_LL() {
	static NavPoint* v = new NavPoint(Position::ZERO_LL(), 0.0);
	return *v;
}
const NavPoint& NavPoint::ZERO_XYZ() {
	static NavPoint* v = new NavPoint(Position::ZERO_XYZ(), 0.0);
	return *v;
}
const NavPoint& NavPoint::INVALID() {
	static NavPoint* v = new NavPoint(Position::INVALID(), NaN);
	return *v;
}

const std::string NavPoint::ORIG_STR = "Orig";
//const std::string NavPoint::ADDED_STR = "Added";
//const std::string NavPoint::MODIFIED_STR = "Modified";
const std::string NavPoint::VIRTUAL_STR = "Virtual";
const std::string NavPoint::ALTPRESERVE_STR = "AltPreserve";
//  const std::string NavPoint::FIXED_STR = "Fixed";
//  const std::string NavPoint::MUTABLE_STR = "Mutable";
const std::string NavPoint::ELSE_STR = "ELSE";

const std::string NavPoint::NONE_STR = "NONE";
const std::string NavPoint::BOT_STR = "BOT";
//const std::string NavPoint::TMID_STR = "TMID";
const std::string NavPoint::EOT_STR = "EOT";
const std::string NavPoint::EOTBOT_STR = "EOTBOT";
const std::string NavPoint::BVS_STR = "BVS";
const std::string NavPoint::EVS_STR = "EVS";
const std::string NavPoint::EVSBVS_STR = "EVSBVS";
const std::string NavPoint::BGS_STR = "BGS";
const std::string NavPoint::EGS_STR = "EGS";
const std::string NavPoint::EGSBGS_STR = "EGSBGS";

NavPoint::NavPoint(const Position& pp, double tt, WayType tty,
		   const std::string& llabel, 	Trk_TCPType tcp_t, Gs_TCPType tcp_g, Vs_TCPType tcp_v,
		   double sRadius, double a_gs, double a_vs,
		   const Velocity& v_velocityIn,  const Position& sourcePos, double sourceTime,
		   int linearIndex) :
  
    				 p(pp),
				 t(tt),
				 ty(tty),
				 label_s(llabel),
				 tcp_trk(tcp_t),
				 tcp_gs(tcp_g),
				 tcp_vs(tcp_v),
				 sourcePosition_p(sourcePos),
				 sourceTime_d(sourceTime),
				 //accel_d(d_accel),
				 sgnRadius(sRadius),		// signed radius
				 accel_gs(a_gs),            // signed gs-acceleration value
				 accel_vs(a_vs),            // signed vs-acceleration value
				 velocityInit_v(v_velocityIn),
				 linearIndex_i(linearIndex)
{ }



NavPoint::NavPoint() :
    				p(Position::ZERO_LL()),
					t(0.0),
					ty(Orig),
					//mut(PointMutability()),
					label_s(""),
					//tcp(NONE),
					tcp_trk(NONE),
					tcp_gs(NONEg),
					tcp_vs(NONEv),
					sourcePosition_p(p),
					sourceTime_d(t),
					//accel_d(0.0),
					sgnRadius(0.0),		      // signed radius
					accel_gs(0.0),            // signed gs-acceleration value
					accel_vs(0.0),            // signed vs-acceleration value
					velocityInit_v(Velocity::INVALIDV()),
                    linearIndex_i(-1)
{ }

NavPoint::NavPoint(const Position& pp, double tt) :
    				p(pp),
					t(tt),
					ty(Orig),
					//mut(PointMutability()),
					label_s(""),
					tcp_trk(NONE),
					tcp_gs(NONEg),
					tcp_vs(NONEv),
					sourcePosition_p(p),
					sourceTime_d(t),
					sgnRadius(0.0),           // signed radius
					accel_gs(0.0),            // signed gs-acceleration value
					accel_vs(0.0),            // signed vs-acceleration value
					velocityInit_v(Velocity::INVALIDV()),
					linearIndex_i(-1)
{ }

NavPoint::NavPoint(const Position& pp, double tt, const string& llabel) :
    				p(pp),
					t(tt),
					ty(Orig),
					//mut(pm),
					label_s(llabel),
					tcp_trk(NONE),
					tcp_gs(NONEg),
					tcp_vs(NONEv),
					sourcePosition_p(p),
					sourceTime_d(t),
					sgnRadius(0.0),           // signed radius
					accel_gs(0.0),            // signed gs-acceleration value
					accel_vs(0.0),            // signed vs-acceleration value
					velocityInit_v(Velocity::INVALIDV()),
					linearIndex_i(-1)
{ }


NavPoint NavPoint::makeFull(const Position& p, double t, WayType ty, const std::string& label,
	      Trk_TCPType tcp_trk, Gs_TCPType tcp_gs, Vs_TCPType tcp_vs,
	      double sRadius, double accel_gs, double accel_vs,
			    const Velocity& velocityIn, const Position& sourcePosition, double sourceTime) {
  return NavPoint(p, t, ty, label, tcp_trk, tcp_gs, tcp_vs, sRadius,  accel_gs,  accel_vs,  velocityIn, sourcePosition,  sourceTime, -1);
 }


NavPoint NavPoint::makeLatLonAlt(double lat, double lon, double alt, double t) {
	return NavPoint(Position::makeLatLonAlt(lat,lon,alt), t);
}

NavPoint NavPoint::makeXYZ(double x, double y, double z, double t) {
	return NavPoint(Position::makeXYZ(x, y, z), t);
}

bool NavPoint::isInvalid() const {
	return p.isInvalid() || t != t;
}


/**
 * Can this point be merged with the given point p?  Merged points only apply to TCP points.
 * @param point the other point
 * @return true, if the points can be merged.
 */
bool NavPoint::mergeable(const NavPoint& point) const {
	//f.pln(" $$ mergeable this = "+this.toStringFull()+" p = "+p.toStringFull());
	bool r1 = (this->tcp_trk == NavPoint::NONE || point.tcp_trk == NavPoint::NONE)
								|| (this->tcp_trk == NavPoint::BOT && point.tcp_trk == NavPoint::EOT)
								|| (this->tcp_trk == NavPoint::EOT && point.tcp_trk == NavPoint::BOT);
	bool r2 = r1 && ((this->tcp_gs == NavPoint::NONEg || point.tcp_gs == NavPoint::NONEg)
			|| (this->tcp_gs == NavPoint::BGS && point.tcp_gs == NavPoint::EGS)
			|| (this->tcp_gs == NavPoint::EGS && point.tcp_gs == NavPoint::BGS));
	bool r3 = r2 && ((this->tcp_vs == NavPoint::NONEv || point.tcp_vs == NavPoint::NONEv)
			|| (this->tcp_vs == NavPoint::BVS && point.tcp_vs == NavPoint::EVS)
			|| (this->tcp_vs == NavPoint::EVS && point.tcp_vs == NavPoint::BVS));
	return r3 && this->t == point.t && this->p == point.p && (this->velocityInit_v.isInvalid() || point.velocityInit_v.isInvalid() ||  this->velocityInit_v == point.velocityInit_v);
}


/**
 * Creates a new point that is the merger of the this point and the given
 * point.  Assumes that mergeable() on the two points would return true, if
 * not, then approximately the original point is returned. <p>
 *
 * Merging rules:
 * <ul>
 * <li> mergeTCPInfo() is commutative
 * <li> If both points are linear points, then the points are merged.
 * <li> If one point is a linear point (aka, not a TCP), then the TCP info (velocity and acceleration)
 * from the TCP point is used.
 * <li> If both points are TCP (aka acceleration points) of different types (Trk, Gs, Vs), then the points
 * should merge without an issue, note: they are required to have the same "velocity in"
 * <li> If both points are TCP (aka acceleration points) of the same type (Trk, Gs, or Vs), then the
 * resulting point should be a combined point (e.g., EOTBOT), and the TCP information should be the information
 * from the beginning point.
 * <li> If both points are TCP "begin" points, then a message is provided if they have different source positions
 * </ul>
 *
 * @param point the other point
 * @return a new point that is the merger of the two points.
 */
NavPoint NavPoint::mergeTCPInfo(const NavPoint& point) const {
	// position & time -- keep either (should be the same)

	WayType my_ty = (this->ty == NavPoint::AltPreserve || point.ty == NavPoint::AltPreserve) ? NavPoint::AltPreserve :
			((this->ty == NavPoint::Orig || point.ty == NavPoint::Orig) ? NavPoint::Orig : NavPoint::Virtual);
	std::string my_label = this->label_s+point.label_s;

	Trk_TCPType my_tcp_trk = ((this->isBOT() && point.isEOT()) || (point.isBOT() && this->isEOT())) ? NavPoint::EOTBOT :
		((this->isBOT() || point.isBOT()) ? NavPoint::BOT :
			((this->isEOT() || point.isEOT()) ? NavPoint::EOT : NavPoint::NONE));
	Gs_TCPType my_tcp_gs = ((this->isBGS() && point.isEGS()) || (point.isBGS() && this->isEGS())) ? NavPoint::EGSBGS :
		((this->isBGS() || point.isBGS()) ? NavPoint::BGS :
			((this->isEGS() || point.isEGS()) ? NavPoint::EGS : NavPoint::NONEg));
	Vs_TCPType my_tcp_vs = ((this->isBVS() && point.isEVS()) || (point.isBVS() && this->isEVS())) ? NavPoint::EVSBVS :
		((this->isBVS() || point.isBVS()) ? NavPoint::BVS :
			((this->isEVS() || point.isEVS()) ? NavPoint::EVS : NavPoint::NONEv));

    		double my_radius = 0.0; // (this.tcp_trk != Trk_TCPType.NONE || point.tcp_trk != Trk_TCPType.NONE) ? 0 : this.sgnRadius;
    		if (this->tcp_trk == BOT || this->tcp_trk == EOTBOT) my_radius = this->sgnRadius;
    		else my_radius = point.sgnRadius;


        	double my_accel_gs = (this->accel_gs == BGS || this->accel_gs == EGSBGS) ? this->accel_gs : point.accel_gs;
			double my_accel_vs = (this->accel_vs == BVS || this->accel_vs == EVSBVS) ? this->accel_vs : point.accel_vs;

			Velocity my_velocityIn;
			if (this->isBeginTCP()) {
				my_velocityIn = this->velocityInit_v;
			} else {
				my_velocityIn = point.velocityInit_v;
			}

			Position my_sourcePosition;
			double my_sourceTime;
			int my_linearIndex;
			if (this->isBeginTCP()) {  // checking if point.sourceTime is NaN
				my_sourcePosition = this->sourcePosition_p;
				my_sourceTime = this->sourceTime_d;
				my_linearIndex = this->linearIndex_i;
			} else {
				my_sourcePosition = point.sourcePosition_p;
				my_sourceTime = point.sourceTime_d;
				my_linearIndex = point.linearIndex_i;
			}


	return NavPoint(this->p, this->t, my_ty, my_label, my_tcp_trk, my_tcp_gs, my_tcp_vs,
			my_radius, my_accel_gs, my_accel_vs, my_velocityIn, my_sourcePosition, my_sourceTime, my_linearIndex);
}

//	/**
//	 * Can this point be merged with the given point p?
//	 * @param point the other point
//	 * @return true, if the points can be merged.
//	 */
//   bool NavPoint::mergeable(const NavPoint& point) const {
//		//f.pln(" $$ mergeable this = "+this.toStringFull()+" p = "+p.toStringFull());
//		bool r1 = (this->tcp_trk == NavPoint::NONE || point.tcp_trk == NavPoint::NONE)
//				|| (this->tcp_trk == NavPoint::BOT && point.tcp_trk == NavPoint::EOT)
//				|| (this->tcp_trk == NavPoint::EOT && point.tcp_trk == NavPoint::BOT);
//		bool r2 = r1 && ((this->tcp_gs == NavPoint::NONEg || point.tcp_gs == NavPoint::NONEg)
//				|| (this->tcp_gs == NavPoint::BGS && point.tcp_gs == NavPoint::EGS)
//				|| (this->tcp_gs == NavPoint::EGS && point.tcp_gs == NavPoint::BGS));
//		bool r3 = r2 && ((this->tcp_vs == NavPoint::NONEv || point.tcp_vs == NavPoint::NONEv)
//				|| (this->tcp_vs == NavPoint::BVS && point.tcp_vs == NavPoint::EVS)
//				|| (this->tcp_vs == NavPoint::EVS && point.tcp_vs == NavPoint::BVS));
//		//fpln(" mergeable: r1 = "+Fmb(r1)+" r2 = "+Fmb(r2)+" r3 = "+Fmb(r3));
//		//fpln(" mergeable: "+Fmb(this->p == point.p)+" "+Fmb(this->velocityIn_v == point.velocityIn_v));
//		return r3 && this->t == point.t && this->p == point.p
//			   && (this->velocityIn_v.isInvalid() || point.velocityIn_v.isInvalid() || this->velocityIn_v == point.velocityIn_v);
//	}
//
//	/**
//	 * Creates a new point that is the merger of the this point and the given
//	 * point.  Assumes that mergeable() on the two points would return true, if
//	 * not the orignal point is returned.
//	 *
//	 * @param point the other point
//	 * @return a new point that is the merger of the two points.
//	 */
//NavPoint NavPoint::mergeTCPInfo(const NavPoint& point) const {
//		NavPoint np = *this;
//		if (this->tcp_trk == NavPoint::BOT && point.tcp_trk == NavPoint::EOT) { // add end
//			np = np.makeTrkTCP(NavPoint::EOTBOT);
//		} else if (this->tcp_trk == NavPoint::EOT && point.tcp_trk == NavPoint::BOT) { // start
//			np = np.makeTrkTCP(NavPoint::EOTBOT).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition_p, point.sourceTime_d);
//		} else if (point.tcp_trk != NavPoint::NONE) { // general case
//			np = np.makeTrkTCP(point.tcp_trk).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition_p, point.sourceTime_d);
//		}
//
//		if (this->tcp_gs == NavPoint::BGS && point.tcp_gs == NavPoint::EGS) { // add end
//			np = np.makeGsTCP(NavPoint::EGSBGS);
//		} else if (this->tcp_gs == NavPoint::EGS && point.tcp_gs == NavPoint::BGS) { // start
//			np = np.makeGsTCP(NavPoint::EGSBGS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition_p,point.sourceTime_d);
//		} else if (point.tcp_gs != NavPoint::NONEg) { // general case
//			np = np.makeGsTCP(point.tcp_gs).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition_p,point.sourceTime_d);
//		}
//
//		if (this->tcp_vs == NavPoint::BVS && point.tcp_vs == NavPoint::EVS) { // add end
//			np = np.makeVsTCP(NavPoint::EVSBVS);
//		} else if (this->tcp_vs == NavPoint::EVS && point.tcp_vs == NavPoint::BVS) {  // start
//			np = np.makeVsTCP(NavPoint::EVSBVS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition_p,point.sourceTime_d);
//		} else if (point.tcp_vs != NavPoint::NONEv) { // general case
//			np = np.makeVsTCP(point.tcp_vs).makeVsAccel(point.accel_vs).makeSource(point.sourcePosition_p,point.sourceTime_d);
//		}
//
//		if (this->velocityIn_v.isInvalid()) {
//			np = np.makeVelocityIn(point.velocityIn_v);
//		}
//
//		if ((this->tcp_trk == BOT && point.tcp_vs == NavPoint::BVS) ||  // add warning
//			(this->tcp_vs == NavPoint::BVS && point.tcp_trk ==  BOT)) {
//			fpln(" $$$$!!!!!!!!!!!!! Warning: this point:"+np.toString()+" may not properly revert via removeTCPS");
//		}
//
//		np = np.appendLabel(point.label_s);
//       return np;
//	}


bool NavPoint::almostEquals(const NavPoint& v) const {
	return Constants::almost_equals_time(t,v.t)
	&& p.almostEquals(v.p);
}

bool NavPoint::almostEqualsPosition(const NavPoint& v) const {
	return p.almostEquals(v.p);
}


bool NavPoint::operator == (const NavPoint& v) const {  // strict equality
	return p == v.p && t==v.t && ty == v.ty; //  && mut == v.mut;
}

bool NavPoint::equals(const NavPoint& v) const {  // strict equality
	return *this == v;
}

bool NavPoint::operator != (const NavPoint& v) const {  // strict disequality
	return p!=v.p || t!=v.t || ty != v.ty; //  || mut != v.mut;
}

Vect2 NavPoint::vect2() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in vect2()");
	}
	return p.vect2();
}

Point NavPoint::point() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in vect3()");
	}
	return p.point();
}

const LatLonAlt& NavPoint::lla() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lla()");
	}
	return p.lla();
}

const Position& NavPoint::position() const {
	return p;
}

double NavPoint::x() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in x()");
	}
	return p.x();
}

double NavPoint::y() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in y()");
	}
	return p.y();
}

double NavPoint::z() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in z()");
	}
	return p.z();
}

double NavPoint::lat() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lat()");
	}
	return p.lat();
}

double NavPoint::lon() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lon()");
	}
	return p.lon();
}

double NavPoint::alt() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in alt()");
	}
	return p.alt();
}


double NavPoint::latitude() const {
	return p.latitude();
}

double NavPoint::longitude() const {
	return p.longitude();
}

double NavPoint::altitude() const {
	return p.altitude();
}


double NavPoint::xCoordinate() const {
	return p.xCoordinate();
}

double NavPoint::yCoordinate() const {
	return p.yCoordinate();
}

double NavPoint::zCoordinate() const {
	return p.zCoordinate();
}

double NavPoint::time() const {
	return t;
}

NavPoint::Trk_TCPType NavPoint::getTrkTCP() const {
	return tcp_trk;
}

NavPoint::Gs_TCPType NavPoint::getGsTCP() const {
	return tcp_gs;
}

NavPoint::Vs_TCPType NavPoint::getVsTCP() const {
	return tcp_vs;
}


const std::string& NavPoint::label() const {
	std::stringstream sb;
	//    if (label == "") {
		//		if (isLatLon()) {
	//			sb << "L" << Fm2is(latitude()) << ":" << Fm2is(longitude()) << ":"
	//					<< Fm3i(altitude() / 100.0);
	//			tmp_label = sb.str();
	//			return tmp_label;
	//		} else {
	//			sb << Fm2is(xCoordinate()) << ":" << Fm2is(yCoordinate()) << ":"
	//					<< Fm3i(altitude() / 100.0);
	//			tmp_label = sb.str();
	//			return tmp_label;
	//		}
	//	}
	return label_s;
}


std::string NavPoint::fullLabel() const {
	return label()+metaDataLabel(4);
}

bool NavPoint::isNameSet() const {
	return label_s != "";
}

std::string NavPoint::metaDataLabel(int precision) const {
	std::string tlabel = "";
	if (isVirtual()) {
		tlabel = tlabel + "VIRT:";
	}
	if (isTrkTCP()) {
		tlabel = tlabel + toStringTrkTCP(tcp_trk) +":";
		if (isBOT()) {
			tlabel = tlabel + "ATRK:"+FmPrecision(sgnRadius,precision)+":";
		}
	}
	if (isGsTCP()) {
		tlabel = tlabel + toStringGsTCP(tcp_gs) +":";
		if (isBGS()) {
			tlabel = tlabel + "AGS:"+FmPrecision(accel_gs,precision)+":";
		}
	}
	if (isVsTCP()) {
		tlabel = tlabel + toStringVsTCP(tcp_vs) +":";
		if (isBVS()) {
			tlabel = tlabel + "AVS:"+FmPrecision(accel_vs,precision)+":";
		}
	}
	if (!velocityInit_v.isInvalid()) {
		std::string v = velocityInit_v.toStringNP(precision);
		replace(v, ',', '_');
		replace(v, ' ', '_');
		tlabel = tlabel + "VEL:"+v+":";
	}
	if (sourceTime_d < 0) { // MOT or other added points
		tlabel = tlabel + "ADDED:";
	} else if (isTCP()){ // generated TCP points
		std::string p = sourcePosition_p.toStringNP(precision);
		replace(p, ',', '_');
		replace(p, ' ', '_');
		tlabel = tlabel + "SRC:"+p+":";
		tlabel = tlabel + "STM:"+FmPrecision(sourceTime_d,precision)+":";
	} else if (sourceTime_d != t) { // linear timeshifted point
		tlabel = tlabel + "STM:"+FmPrecision(sourceTime_d,precision)+":";
	}
	// add starting colon, if there is any data;
	if (tlabel.length() > 0) {
		tlabel = ":"+tlabel;
	}
	return tlabel;
}


const NavPoint NavPoint::parseMetaDataLabel(const std::string& tlabel) const {
	NavPoint point = *this;
	int lowIndex = tlabel.length(); // then current end of the user label
	int j;
	int i = tlabel.find(":ACC:");
	if (i >= 0) {
		return INVALID();
	}
	i = tlabel.find(":VIRT:");
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		point = point.makeVirtual();
	}

	i = tlabel.find(":ADDED:");
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		point = point.makeSource(Position::INVALID(),-1.0);
	} else {
		Position sp = p;
		double st = t;
		i = tlabel.find(":SRC:");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+5);
			std::string s = tlabel.substr(i+5, j);
			replace(s, '_', ' ');
			sp = Position::parse(s);
			if (sp.isInvalid()) {
				if (isLatLon()) {
					sp = Position::parseLL(s);
				} else {
					sp = Position::parseXYZ(s);
				}
			}
		}
		i = tlabel.find(":STM:");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+5);
			st = Util::parse_double(tlabel.substr(i+5,j));
		}
		point = point.makeSource(sp,st);
	}
	i = tlabel.find(":VEL:"); // velocity in
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		j = tlabel.find(":",i+5);
		std::string s = tlabel.substr(i+5, j);
		replace(s, '_', ' ');
		Velocity v = Velocity::parse(s);
		point = point.makeVelocityInit(v);
	}
	for (int iter = 0; iter <= 4; iter++) {
		Trk_TCPType tt = toTrkTCP(iter);
		i = tlabel.find(":"+toStringTrkTCP(tt)+":");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.makeTrkTCP(tt);
		}
	}
	for (int iter = 0; iter <= 4; iter++) {
		Gs_TCPType tt = toGsTCP(iter);
		i = tlabel.find(":"+toStringGsTCP(tt)+":");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.makeGsTCP(tt);
		}
	}
	for (int iter = 0; iter <= 4; iter++) {
		Vs_TCPType tt = toVsTCP(iter);
		i = tlabel.find(":"+toStringVsTCP(tt)+":");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.makeVsTCP(tt);
		}
	}
	i = tlabel.find(":ATRK:"); // acceleration
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		j = tlabel.find(":",i+6);
		double a = Util::parse_double(tlabel.substr(i+6,j));
		point = point.makeTrkAccel(a);
	}
	i = tlabel.find(":AGS:"); // acceleration
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		j = tlabel.find(":",i+5);
		double a = Util::parse_double(tlabel.substr(i+5,j));
		point = point.makeGsAccel(a);
	}
	i = tlabel.find(":AVS:"); // acceleration
	if (i >= 0) {
		lowIndex = std::min(i, lowIndex);
		j = tlabel.find(":",i+5);
		double a = Util::parse_double(tlabel.substr(i+5,j));
		point = point.makeVsAccel(a);
	}
	std::string name = tlabel;
	if (lowIndex >= 0 && lowIndex < (int) tlabel.length()) {
		name = tlabel.substr(0,lowIndex);
		point = point.makeLabel(name);
	}
	return point;
}

/**
 * Re-parse this point as a TCP, if its label describes it as such.
 * If this point is already a TCP or has no appropriate label, return unchanged.
 * @return
 */
const NavPoint NavPoint::parseMetaDataLabel() const {
	//	  if (isTCP()) return *this;
	return parseMetaDataLabel(label_s);
}


const std::string NavPoint::tcpTypeString() const {
	std::string s = "";
	if (tcp_trk != NavPoint::NONE) s += tcp_trk;
	if (tcp_gs != NavPoint::NONEg) {
		if (s!="") s+= ",";
		s += tcp_gs;
	}
	if (tcp_vs != NavPoint::NONEv) {
		if (s!="") s+= ",";
		s += tcp_vs;
	}
	if (s=="") s += "NONE";
	return s;
}


bool NavPoint::isLatLon() const {
	return p.isLatLon();
}


bool NavPoint::isVirtual() const {
	return ty == Virtual;
}

//  bool NavPoint::isAdded() const {
//    return ty == Added;
//  }


bool NavPoint::isAdded() const {
	return sourceTime_d < 0.0;
}

bool NavPoint::isOriginal() const {
	return ty == Orig;
}

//  bool NavPoint::isModified() const {
//    return ty == Modified;
//  }

bool NavPoint::isAltPreserve() const {
	return ty == AltPreserve;
}

//	double NavPoint::goalGsIn() const {
//		if (velocityIn_v.isInvalid()) return -1.0;
//		return velocityIn_v.gs();
//	}
//
//	std::string& NavPoint::strTCP() const{
//		return tcp.toString();
//	}


const std::string& NavPoint::toStringType(NavPoint::WayType ty)  {
	if (ty == NavPoint::Orig) return NavPoint::ORIG_STR;
	//if (ty == NavPoint::Added) return NavPoint::ADDED_STR;
	//if (ty == NavPoint::Modified) return NavPoint::MODIFIED_STR;
	if (ty == NavPoint::Virtual) return NavPoint::VIRTUAL_STR;
	if (ty == NavPoint::AltPreserve) return NavPoint::ALTPRESERVE_STR;
	return NavPoint::ELSE_STR;
}

const std::string& NavPoint::strType() const {
	return toStringType(ty);
}

const std::string& NavPoint::toStringTrkTCP(NavPoint::Trk_TCPType ty) {
	if (ty == NavPoint::BOT) return NavPoint::BOT_STR;
	if (ty == NavPoint::EOT) return NavPoint::EOT_STR;
	if (ty == NavPoint::EOTBOT) return NavPoint::EOTBOT_STR;
	return NavPoint::NONE_STR;
}

const std::string& NavPoint::toStringGsTCP(NavPoint::Gs_TCPType ty) {
	if (ty == NavPoint::BGS) return NavPoint::BGS_STR;
	if (ty == NavPoint::EGS) return NavPoint::EGS_STR;
	if (ty == NavPoint::EGSBGS) return NavPoint::EGSBGS_STR;
	return NavPoint::NONE_STR;
}

const std::string& NavPoint::toStringVsTCP(NavPoint::Vs_TCPType ty) {
	if (ty == NavPoint::BVS) return NavPoint::BVS_STR;
	if (ty == NavPoint::EVS) return NavPoint::EVS_STR;
	if (ty == NavPoint::EVSBVS) return NavPoint::EVSBVS_STR;
	return NavPoint::NONE_STR;
}

NavPoint::WayType NavPoint::WayTypeValueOf(const std::string& s) {
	if (larcfm::equals(s, NavPoint::VIRTUAL_STR)) return NavPoint::Virtual;
	if (larcfm::equals(s, NavPoint::ALTPRESERVE_STR)) return NavPoint::AltPreserve;
	if (larcfm::equals(s, NavPoint::ORIG_STR)) return NavPoint::Orig;
	return NavPoint::UNKNOWN_WT;
}


NavPoint::Trk_TCPType NavPoint::Trk_TCPTypeValueOf(const std::string& s) {
	if (larcfm::equals(s, NavPoint::BOT_STR)) return NavPoint::BOT;
	if (larcfm::equals(s, NavPoint::EOT_STR)) return NavPoint::EOT;
	if (larcfm::equals(s, NavPoint::EOTBOT_STR)) return NavPoint::EOTBOT;
	if (larcfm::equals(s, NavPoint::NONE_STR)) return NavPoint::NONE;
	return NavPoint::UNKNOWN_TRK;
}

NavPoint::Gs_TCPType NavPoint::Gs_TCPTypeValueOf(const std::string& s) {
	if (larcfm::equals(s, NavPoint::BGS_STR)) return NavPoint::BGS;
	if (larcfm::equals(s, NavPoint::EGS_STR)) return NavPoint::EGS;
	if (larcfm::equals(s, NavPoint::EGSBGS_STR)) return NavPoint::EGSBGS;
	if (larcfm::equals(s, NavPoint::NONE_STR)) return NavPoint::NONEg;
	return NavPoint::UNKNOWN_GS;
}

NavPoint::Vs_TCPType NavPoint::Vs_TCPTypeValueOf(const std::string& s) {
	if (larcfm::equals(s, NavPoint::BVS_STR)) return NavPoint::BVS;
	if (larcfm::equals(s, NavPoint::EVS_STR)) return NavPoint::EVS;
	if (larcfm::equals(s, NavPoint::EVSBVS_STR)) return NavPoint::EVSBVS;
	if (larcfm::equals(s, NavPoint::NONE_STR)) return NavPoint::NONEv;
	return NavPoint::UNKNOWN_VS;
}

NavPoint::Trk_TCPType NavPoint::toTrkTCP(int ty) {
	if (ty == 1) return NavPoint::BOT;
	if (ty == 2) return NavPoint::EOT;
	if (ty == 3) return NavPoint::EOTBOT;
	return NavPoint::NONE;
}

NavPoint::Gs_TCPType NavPoint::toGsTCP(int ty) {
	if (ty == 1) return NavPoint::BGS;
	if (ty == 2) return NavPoint::EGS;
	if (ty == 3) return NavPoint::EGSBGS;
	return NavPoint::NONEg;
}

NavPoint::Vs_TCPType NavPoint::toVsTCP(int ty) {
	if (ty == 1) return NavPoint::BVS;
	if (ty == 2) return NavPoint::EVS;
	if (ty == 3) return NavPoint::EVSBVS;
	return NavPoint::NONEv;
}


//double NavPoint::getRadius() const {
//    return sRadius;
//}

double NavPoint::turnRadius() const {
	//fpln("NavPoint::turnRadius "+Fmb(isTurn())+" "+Fm2(accel_d)+" "+velocityIn_v.toString());
	return std::abs(sgnRadius);
//	if (tcp_trk == NavPoint::NONE) {
//		return radius;
//	} else if (isTrkTCP() && accel_trk != 0.0) {
//		return std::abs(velocityIn_v.gs()/accel_trk);
//	}
//	return 0.0;
}

double NavPoint::signedRadius() const {
	return sgnRadius;
}



Position NavPoint::turnCenter() const {
	double R = signedRadius();
//	if (isTrkTCP()) {
//		if (R > 0.0) {
//			return p.linear(velocityIn_v.mkAddTrk(Util::sign(accel_trk)*Pi/2).Hat(),R).mkZ(p.z());
//		}
//	} else
	if (R != 0.0) {
		//return p.linear(velocityInit_v.mkAddTrk(Util::sign(R)*Pi/2).Hat(),std::abs(R)).mkZ(p.z());
		Velocity vHat = velocityInit_v.mkAddTrk(Util::sign(R)*Pi/2).Hat2D();
		return p.linear(vHat, std::abs(R)).mkZ(p.z());
	}
	return Position::INVALID();
}


double NavPoint::sourceTime() const {
	return sourceTime_d;
}

Position NavPoint::sourcePosition() const {
	return sourcePosition_p;
}

int NavPoint::linearIndex() const {
	return linearIndex_i;
}

double NavPoint::hasSource() const {
	return sourceTime_d >= 0;
}


bool NavPoint::isTCP() const {
	return tcp_trk != NONE || tcp_gs != NONEg || tcp_vs != NONEv;
}

bool NavPoint::isTrkTCP() const {
	return tcp_trk != NONE;
}

bool NavPoint::isBOT() const {
	return tcp_trk == BOT || tcp_trk == EOTBOT;
}

//  bool NavPoint::isTurnMid() const {
//	return tcp == TMID;
//  }

bool NavPoint::isEOT() const {
	return tcp_trk == EOT || tcp_trk == EOTBOT;
}

bool NavPoint::isGsTCP() const {
	return tcp_gs != NONEg;
}

bool NavPoint::isBGS() const {
	return tcp_gs == BGS || tcp_gs == EGSBGS;
}

bool NavPoint::isEGS() const {
	return tcp_gs == EGS || tcp_gs == EGSBGS;
}

bool NavPoint::isVsTCP() const {
	return tcp_vs != NONEv;
}

bool NavPoint::isBVS() const {
	return tcp_vs == BVS || tcp_vs == EVSBVS;
}

bool NavPoint::isEVS() const {
	return tcp_vs == EVS || tcp_vs == EVSBVS;
}

bool NavPoint::isBeginTCP() const {
	return tcp_trk == BOT || tcp_gs == BGS || tcp_vs == BVS ||
			tcp_trk == EOTBOT || tcp_gs == EGSBGS || tcp_vs == EVSBVS;
}

bool NavPoint::isEndTCP() const {
	return tcp_trk == EOT || tcp_gs == EGS || tcp_vs == EVS ||
			tcp_trk == EOTBOT || tcp_gs == EGSBGS || tcp_vs == EVSBVS;
}


const NavPoint NavPoint::copy(const Position& p) const {
	WayType ty = this->ty;
	//    if (ty == Orig) {
		//      ty = Modified;
	//    }
	return NavPoint(p, this->t, ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::copy(WayType ty) const {
	return NavPoint(this->p, this->t, ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::mkLat(double lat) const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeLat()");
	}
	return copy(p.mkLat(lat));
}

const NavPoint NavPoint::mkX(double x) const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeX()");
	}
	return copy(p.mkX(x));
}

const NavPoint NavPoint::mkLon(double lon) const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeLon()");
	}
	return copy(p.mkLon(lon));
}

const NavPoint NavPoint::mkY(double y) const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeY()");
	}
	return copy(p.mkY(y));
}

const NavPoint NavPoint::mkAlt(double alt) const {
	return copy(p.mkAlt(alt));
}

const NavPoint NavPoint::mkZ(double z) const {
	return copy(p.mkZ(z));
}


// this->sgnRadius this->accel_gs, this->accel_vs, this->velocityIn_v, this->sourcePosition_p, this->sourceTime_d

const NavPoint NavPoint::makeTime(double time) const {
	WayType ty = this->ty;
	//    if (ty == Orig) {
	//      ty = Modified;
	//    }
	return NavPoint(this->p, time, ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeTrkTCP(Trk_TCPType tcp) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, tcp, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeGsTCP(Gs_TCPType tcp) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, tcp, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeVsTCP(Vs_TCPType tcp) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, tcp,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeSource(const Position& sp, double st) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  sp, st,  this->linearIndex_i);
}


const NavPoint NavPoint::makeSourcePosition(const Position& sp) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  sp, this->sourceTime_d,  this->linearIndex_i);
}

  
const NavPoint NavPoint::makeSourceTime(double st) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, st,  this->linearIndex_i);
}

const NavPoint NavPoint::makeLinearIndex(int ix) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d, ix);
}

const NavPoint NavPoint::makeRadius(double r) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			r, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}


//	/** Make a new NavPoint with current point's information as the "source" */
//	const NavPoint NavPoint::makeSourceClear() const {
//		return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp,
//				this->p, this->t, this->accel_d, this->velocityIn_v, this->minorV_i);
//	}

const NavPoint NavPoint::makeTrkAccel(double omega) const {
	double radius = this->velocityInit_v.gs()/omega;
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			radius, this->accel_gs, this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeGsAccel(double ga) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, ga, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeVsAccel(double va) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, va, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeVelocityInit(const Velocity& vi) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, vi, this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

//  const NavPoint NavPoint::makeGoalGsIn(double gs) const {
//		Velocity v = this->velocityIn_v;
//		if (v.isInvalid()) {
//			v = Velocity::ZEROV();
//		}
//		if (gs < 0.0) {
//			v = Velocity::INVALIDV();
//		} else {
//			v = v.mkGs(gs);
//		}
//
//    return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp,
//    		this->sourcePosition, this->sourceTime, this->accel_d, v, this->minorV_i);
//  }

const NavPoint NavPoint::makeVirtual() const {
	return copy(Virtual);
}

const NavPoint NavPoint::makeAdded() const {
	//    if (isAdded()) return *this;
	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  Position::INVALID(), -1.0,  -1);
}

const NavPoint NavPoint::makeOriginal() const {
	//if (isOriginal()) return *this;
	return copy(Orig);
}

//  const NavPoint NavPoint::makeModified() const {
//    //if (isModified()) return *this;
//    return copy(Modified);
//  }


const NavPoint NavPoint::makeAltPreserve() const {
	if (isAltPreserve()) return *this;
	return copy(AltPreserve);
}

//  const NavPoint NavPoint::makeVirtualAdded() const {
//	  //if ( ! isVirtual()) return *this;
//	  return copy(Orig);
//  }


const NavPoint NavPoint::makeLabel(const std::string& label) const {
	return NavPoint(this->p, this->t, this->ty,  label, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::appendLabel(const std::string& label) const {
	return NavPoint(this->p, this->t, this->ty,  this->label_s+label, this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d,  this->linearIndex_i);
}

const NavPoint NavPoint::makeBOT(const Position& p, double t,
				 const Velocity& v_velocityIn, double sRadius, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, BOT, this->tcp_gs, this->tcp_vs,
			sRadius, this->accel_gs, this->accel_vs, v_velocityIn, this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEOT(const Position& p, double t, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, EOT, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, v_velocityIn, this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEOTBOT(const Position& p , double t, const Velocity& v_velocityIn, double sRadius, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, EOTBOT, this->tcp_gs, this->tcp_vs,
			sRadius, this->accel_gs, this->accel_vs, v_velocityIn, this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeBGS(const Position& p, double t,  double d_gsAccel, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, this->tcp_trk, BGS,  this->tcp_vs,
			this->sgnRadius, d_gsAccel, this->accel_vs, v_velocityIn,  this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEGS(const Position& p, double t, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, this->tcp_trk, EGS,  this->tcp_vs,
			this->sgnRadius, this->accel_gs , this->accel_vs, v_velocityIn,  this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEGSBGS(const Position& p , double t, double a, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, this->tcp_trk, EGSBGS, this->tcp_vs,
			this->sgnRadius, a, this->accel_vs, v_velocityIn,  this->sourcePosition_p, this->sourceTime_d, ix);
}


  const NavPoint NavPoint::makeBVS(const Position& p, double t, double d_vsAccel, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s,  this->tcp_trk, this->tcp_gs, BVS,
			this->sgnRadius, this->accel_gs, d_vsAccel, v_velocityIn, this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEVS(const Position& p, double t, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s,  this->tcp_trk, this->tcp_gs, EVS,
			this->sgnRadius, this->accel_gs, this->accel_vs, v_velocityIn, this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeEVSBVS(const Position& p , double t, double a, const Velocity& v_velocityIn, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, EVSBVS,
			this->sgnRadius,  this->accel_gs, a, v_velocityIn,  this->sourcePosition_p, this->sourceTime_d, ix);
}

  const NavPoint NavPoint::makeMidpoint(const Position& p, double t, int ix) const {
	return NavPoint(p, t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs,  this->tcp_vs,
			this->sgnRadius, this->accel_gs , this->accel_vs, this->velocityInit_v, this->sourcePosition_p, this->sourceTime_d, ix);
}


// const NavPoint NavPoint::makeTCPTurnBegin() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, BOT, this->tcp_gs, this->tcp_vs,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

// const NavPoint NavPoint::makeTCPTurnEnd() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, EOT, this->tcp_gs, this->tcp_vs,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

// const NavPoint NavPoint::makeTCPGSCBegin() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, BGS, this->tcp_vs,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

// const NavPoint NavPoint::makeTCPGSCEnd() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, EGS, this->tcp_vs,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

// //  const NavPoint NavPoint::makeTCPTurnMid() const {
// 	//	    return NavPoint(this->p, this->t, this->ty,  this->label_s, TMID,
// //	  		      this->sgnRadius this->accel_gs, this->accel_vs, this->velocityIn_v, this->sourcePosition_p, this->sourceTime_d);
// // }



// const NavPoint NavPoint::makeTCPVSCBegin() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, BVS,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

// const NavPoint NavPoint::makeTCPVSCEnd() const {
// 	return NavPoint(this->p, this->t, this->ty,  this->label_s, this->tcp_trk, this->tcp_gs, EVS,
// 			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d);
// }

const NavPoint NavPoint::makeStandardRetainSource() const {
	return NavPoint(this->p, this->t, Orig, "", NONE, NONEg, NONEv,
			0.0, 0.0, 0.0, Velocity::INVALIDV(), this->sourcePosition_p, this->sourceTime_d, this->linearIndex_i);
}

/** Makes a new NavPoint that is devoid of any "turn" or "ground speed" tags. */
const NavPoint NavPoint::makeNewPoint() const {
	return NavPoint(this->p, this->t, Orig, "", NONE, NONEg, NONEv,
			0.0, 0.0, 0.0, Velocity::INVALIDV(), this->p, this->t, -1);
}

//
//const NavPoint NavPoint::makeTCPClear() const {
//	return NavPoint(this->p, this->t, this->ty,  this->label_s, NONE, NONEg, NONEv,
//			this->sgnRadius this->accel_gs, this->accel_vs, this->velocityIn_v, this->sourcePosition_p, this->sourceTime_d);
//}


const NavPoint NavPoint::makeMovedFrom(const NavPoint& o) const {
	return NavPoint(this->p, this->t, o.ty, o.label_s, o.tcp_trk, o.tcp_gs, o.tcp_vs,
			o.sgnRadius, o.accel_gs, o.accel_vs, o.velocityInit_v, o.sourcePosition_p, o.sourceTime_d, o.linearIndex_i);

}

const NavPoint NavPoint::makePosition(const Position& p) const {
	return NavPoint(p, this->t, this->ty,  this->label_s,  this->tcp_trk, this->tcp_gs, this->tcp_vs,
			this->sgnRadius, this->accel_gs, this->accel_vs, this->velocityInit_v,  this->sourcePosition_p, this->sourceTime_d, this->linearIndex_i);
}

//  const NavPoint NavPoint::revertToSource(const Position& p) const {
//	    return NavPoint(this->sourcePosition, this->sourceTime, this->ty,  this->label_s);
//  }



  
  Velocity NavPoint::initialVelocity(const NavPoint& s1, const NavPoint& s2) {
	if (NavPoint_DEBUG && s1.isLatLon() != s2.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s2.time() - s1.time();
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_initial(s1.p.lla(), s2.p.lla(), dt);
		} else {
			return Velocity::make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
		}
	} else {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_initial(s2.p.lla(), s1.p.lla(), -dt);
		} else {
			return Velocity::make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
		}
	}
}

  
  
Velocity NavPoint::initialVelocity(const NavPoint& s) const {
  return initialVelocity(*this, s);
}

Velocity NavPoint::finalVelocity(const NavPoint& s) const {
	if (NavPoint_DEBUG && p.isLatLon() != s.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s.time() - t;
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s.isLatLon()) {
			return GreatCircle::velocity_final(p.lla(), s.p.lla(), dt);
		} else {
			return Velocity::make((s.p.point().Sub(p.point())).Scal(1.0/dt));
		}
	} else {
		if (s.isLatLon()) {
			return GreatCircle::velocity_final(s.p.lla(), p.lla(), -dt);
		} else {
			return Velocity::make((p.point().Sub(s.p.point())).Scal(1.0/-dt));
		}
	}
}


double NavPoint::verticalSpeed(const NavPoint& s) const {
	return (s.alt()-this->alt())/(s.time()-this->time());
}


Velocity NavPoint::averageVelocity(const NavPoint& s) const {
	if (NavPoint_DEBUG && p.isLatLon() != s.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s.time() - t;
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s.isLatLon()) {
			return GreatCircle::velocity_average(p.lla(), s.p.lla(), dt);
		} else {
			return Velocity::make((s.p.point().Sub(p.point())).Scal(1.0/dt));
		}
	} else {
		if (s.isLatLon()) {
			return GreatCircle::velocity_average(s.p.lla(), p.lla(), -dt);
		} else {
			return Velocity::make((p.point().Sub(s.p.point())).Scal(1.0/-dt));
		}
	}
}


const NavPoint NavPoint::linear(const Velocity& v, const double time) const {
	Position newPos = p.linear(v,time);
	return NavPoint(newPos,t+time);
}


const NavPoint NavPoint::linearEst(const Velocity& vo, double tm) const {
	return NavPoint(p.linearEst(vo,tm),t+tm);
}


const NavPoint NavPoint::interpolate(const NavPoint& np, const double time) const {
	Velocity v = initialVelocity(np);
	Position newPos = p.linear(v,time-t);
	return NavPoint(newPos,time);
}


double NavPoint::groundSpeed(const NavPoint& np) const {
	return initialVelocity(np).gs();
}

Velocity NavPoint::velocityInit() const {
	//if (isTCP()) {
	return velocityInit_v;
	//}
	//return Velocity::INVALIDV();
}




double NavPoint::trkAccel() const {
	//	return accel_trk;
	if (std::abs(sgnRadius) > 0) {
		return velocityInit_v.gs()/sgnRadius;
	} else {
		return 0.0;
	}
}

double NavPoint::gsAccel() const {
	return accel_gs;
}
double NavPoint::vsAccel() const {
	return accel_vs;
}

//  double NavPoint::rateOfTurn() const{
//	  if (isTurn()) {
//		  return accel_d;
//	  }
//	  return 0.0;
//  }

double NavPoint::distanceH(const NavPoint& np2) const {
	return np2.position().distanceH(position());
}

/** Vertical distance */
double NavPoint::distanceV(const NavPoint& np2) const {
	return np2.position().distanceV(position());
}



std::string NavPoint::toStringShort() const {
	return toString(Constants::get_output_precision());
}

std::string NavPoint::toStringShort(int precision) const {
	return p.toStringNP(precision) + ", " + FmPrecision(t,precision);
}

std::string NavPoint::toString() const {
	return toString(Constants::get_output_precision());
}

std::string NavPoint::toString(int precision) const {
	return p.toStringNP(precision) + ", " + FmPrecision(t,precision)  +" " +tcpTypeString() + " " + label_s;
}


std::vector<std::string> NavPoint::toStringList(int precision, bool tcp) const {
	std::vector<std::string> ret;  // name is (0)
	std::vector<std::string> vec = p.toStringList(precision);
	ret.insert(ret.end(), vec.begin(),vec.end());
	ret.push_back(FmPrecision(t,precision)); // time (4)
	if (tcp) {
		ret.push_back(strType()); // type (string) (5)
		vec = velocityInit_v.toStringList(precision);
		ret.insert(ret.end(),vec.begin(),vec.end()); // vin (6-8) DO NOT CHANGE THIS -- POLYGONS EXPECT VEL TO BE HERE
		ret.push_back(toStringTrkTCP(tcp_trk)); // tcp trk (string) (9)
		ret.push_back(FmPrecision(Units::to("deg/s",trkAccel()),precision)); // trk accel (10)
		ret.push_back(toStringGsTCP(tcp_gs)); // tcp gs (string) (11)
		ret.push_back(FmPrecision(Units::to("m/s^2",accel_gs),precision)); // gs accel (12)
		ret.push_back(toStringVsTCP(tcp_vs)); // tcp vs (string) (13)
		ret.push_back(FmPrecision(Units::to("m/s^2",accel_vs),precision)); // vs accel (14)
		ret.push_back(FmPrecision(Units::to("nmi", sgnRadius), precision)); // radius (15)
		vec = sourcePosition_p.toStringList(precision);
		ret.insert(ret.end(),vec.begin(),vec.end()); // source position (16-18)
		ret.push_back(FmPrecision(sourceTime_d,precision)); // source time (19)
		if (label_s.size() > 0) {
			ret.push_back(label_s); // label (string) (20)
		} else {
			ret.push_back("-");
		}
	} else {
		std::string fl = fullLabel();
		if (fl.size() > 0) {
			ret.push_back(fl); // label (string) (5)
		} else {
			ret.push_back("-");
		}
	}
	return ret;
}


std::string NavPoint::toStringFull() const {
	//StringBuffer sb = new StringBuffer(100);
	std::stringstream sb;

	sb << "[(";
	if (isLatLon()) sb << "LL: ";
	sb << toStringShort(4);
	sb << "), ";
	sb << toStringType(ty);
	if (isTrkTCP()) {
		sb << ", " << toStringTrkTCP(tcp_trk);
		if (isBOT()) {
			sb << " accTrk = " << Fm4(Units::to("deg/s", trkAccel()));
		}
	}
	if (isGsTCP()) {
		sb << ", " << toStringGsTCP(tcp_gs);
		if (isBGS()) {
			sb << " accGs = " << Fm4(Units::to("m/s^2", accel_gs));
		}
	}
	if (isVsTCP()) {
		sb << ", " << toStringVsTCP(tcp_vs);
		if (isBVS()) {
			sb << " accVs = " << Fm4(Units::to("m/s^2", accel_vs));
		}
	}
	if (!velocityInit_v.isInvalid()) sb << " vin = " << velocityInit_v.toStringUnits();
	if (sgnRadius != 0.0) {
		sb << " sgnRadius = " << Fm4(Units::to("NM", sgnRadius));
	}
//	if (sourceTime_d >= 0) {
//		sb << " srcTime = " << Fm2(sourceTime_d);
//		if (!sourcePosition_p.isInvalid()) {
//			sb << " srcPos = " << sourcePosition_p.toStringUnits();
//		}
//	} else {
//		sb << " ADDED";
//	}
    sb << "<" << Fm0(linearIndex_i) << ">";
	sb << "]";
	sb << " " << label();
	return sb.str();
}

//  std::string NavPoint::toStringMutability(std::string prefix, std::string suffix) const {
//    //StringBuffer sb = new StringBuffer(100);
//    std::stringstream sb;
//
//     if (mut.pos != NavPoint::Mutable || mut.alt != NavPoint::Mutable || mut.time != NavPoint::Mutable) {
//      // print out mutability
//    	sb << prefix;
//      if (mut.pos == mut.alt && mut.alt == mut.time) {
//	sb << strPosition().substr(0,1);
//      } else {
//	sb << strPosition().substr(0,1)+
//	  strAltitude().substr(0,1)+
//	  strTime().substr(0,1);
//      }
//      sb << suffix;
//    }
//    return sb.str();
//  }

NavPoint NavPoint::parseLL(const std::string& s) {
	std::vector<string> fields = split(s, Constants::wsPatternParens);
	Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
	double time = Util::parse_double(fields[3]);
	Position pos = Position::makeLatLonAlt(v.x, v.y, v.z);
	if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
		std::string  extra = fields[MIN_OUTPUT_COLUMNS];
		return NavPoint(pos, time, extra).parseMetaDataLabel(extra);
	} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
		WayType wt = WayTypeValueOf(fields[4]);
		Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
		Trk_TCPType trkty = Trk_TCPTypeValueOf(fields[8]);
		//double trkacc = Units::from("deg/s", Util::parse_double(fields[9]));
		Gs_TCPType gsty = Gs_TCPTypeValueOf(fields[10]);
		double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
		Vs_TCPType vsty = Vs_TCPTypeValueOf(fields[12]);
		double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
		double radius = Units::from("nmi", Util::parse_double(fields[14]));
		LatLonAlt slla = LatLonAlt::parse(fields[15]+" "+fields[16]+" "+fields[17]);
		Position sp = Position(slla);
		double st = Util::parse_double(fields[18]);
		std::string lab = fields[19];
		int linearIndex = -1;   // need to fix this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return NavPoint(pos, time, wt, lab, trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, linearIndex);
	} else {
		return NavPoint(pos, time);
	}
}

NavPoint NavPoint::parseXYZ(const std::string& s) {
	std::vector<string> fields = split(s, Constants::wsPatternParens);
	Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
	double time = Util::parse_double(fields[3]);
	Position pos = Position::makeXYZ(v.x, v.y, v.z);
	if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
		std::string  extra = fields[MIN_OUTPUT_COLUMNS];
		return NavPoint(pos, time, extra).parseMetaDataLabel(extra);
	} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
		WayType wt = WayTypeValueOf(fields[4]);
		Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
		Trk_TCPType trkty = Trk_TCPTypeValueOf(fields[8]);
		//double trkacc = Units::from("deg/s", Util::parse_double(fields[9]));
		Gs_TCPType gsty = Gs_TCPTypeValueOf(fields[10]);
		double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
		Vs_TCPType vsty = Vs_TCPTypeValueOf(fields[12]);
		double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
		double radius = Units::from("nmi", Util::parse_double(fields[14]));
		Vect3 sv = Vect3::parse(fields[15]+" "+fields[16]+" "+fields[17]);
		Position sp = Position::makeXYZ(sv.x, sv.y, sv.z);
		double st = Util::parse_double(fields[18]);
		std::string lab = fields[19];
		int linearIndex = -1;   // need to fix this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return NavPoint(pos, time, wt, lab, trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, linearIndex);
	} else {
		return NavPoint(pos, time);
	}}


std::string NavPoint::toOutput() const {
	return toOutput(Constants::get_output_precision(),false);
}


std::string NavPoint::toOutput(int precision, bool tcp) const {
	return list2str(toStringList(precision,tcp),",");
}



}
