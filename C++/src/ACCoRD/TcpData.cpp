/*
 * TcpData
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "TcpData.h"
#include "NavPoint.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Constants.h"
#include "KinematicsPosition.h"
#include "format.h"
#include "Util.h" // NaN definition
#include "Velocity.h"
#include "string_util.h"
#include <stdexcept>
#include <algorithm>

namespace larcfm {
	using std::string;
	using std::cout;
	using std::endl;
	using std::runtime_error;


	
	//TODO remove this method once the conversion is complete
	TcpData::WayType TcpData::getType() const {
		return ty;
	}

	std::string TcpData::getTypeString() const {
		return toStringWayType(ty);
	}

	std::string TcpData::toStringWayType(WayType t) {
		static const std::string ORIG_STR = "Orig";
		static const std::string VIRTUAL_STR = "Virtual";
		static const std::string ALTPRESERVE_STR = "AltPreserve";
		static const std::string ELSE_STR = "ELSE";

		if (t == TcpData::Orig) return ORIG_STR;
		if (t == TcpData::Virtual) return VIRTUAL_STR;
		if (t == TcpData::AltPreserve) return ALTPRESERVE_STR;
		return ELSE_STR;
	}

	TcpData::WayType TcpData::valueOfWayType(const std::string& t) {
		if (t == toStringWayType(TcpData::Orig)) return TcpData::Orig;
		if (t == toStringWayType(TcpData::Virtual)) return TcpData::Virtual;
		if (t == toStringWayType(TcpData::AltPreserve)) return TcpData::AltPreserve;
		return TcpData::UNKNOWN_WT;
	}

	bool TcpData::isVirtual() const {
		return ty == TcpData::Virtual;
	}

	bool TcpData::isOriginal() const {
		return ty == TcpData::Orig;
	}

	bool TcpData::isAltPreserve() const {
		return ty == TcpData::AltPreserve;
	}
	
	void TcpData::setType(const std::string& t) {
		WayType x;
		x = valueOfWayType(t);
		if (x == TcpData::UNKNOWN_WT) {
			x = ty;
		}
		ty = x;
	}
	
	TcpData TcpData::copy() const {
		return TcpData(*this);
	}

	TcpData TcpData::setVirtual() {
		ty = TcpData::Virtual;
		return *this;
	}


	TcpData TcpData::setOriginal() {
		ty = TcpData::Orig;
		return *this;
	}

	TcpData TcpData::setAltPreserve() {
		ty = TcpData::AltPreserve;
		return *this;
	}

	void TcpData::reset() {
		ty = TcpData::Orig;
		tcpTrk = TcpData::NONE;
		tcpGs = TcpData::NONEg;
		tcpVs = TcpData::NONEv;
		radiusSigned = 0.0;
		centerTurn = Position::INVALID();
		accelGs = 0.0;
		accelVs = 0.0;
		//velocityInit_v = Velocity::INVALIDV();
		//setSource(source);     // retain source from original TcpData object
		//linearIndex = -1;      // retain the linear index from originial TcpData object
		information = "";
	}

	std::string TcpData::getTrkTypeString() const {
		return toStringTrkType(tcpTrk);
	}
	std::string TcpData::toStringTrkType(TrkTcpType t) {
		static const std::string NONE_STR = "NONE";
		static const std::string BOT_STR = "BOT";
		static const std::string EOT_STR = "EOT";
		static const std::string EOTBOT_STR = "EOTBOT";
		static const std::string ELSE_STR = "ELSE";

		if (t == TcpData::NONE) return NONE_STR;
		if (t == TcpData::BOT) return BOT_STR;
		if (t == TcpData::EOT) return EOT_STR;
		if (t == TcpData::EOTBOT) return EOTBOT_STR;
		return ELSE_STR;
	}

	TcpData::TrkTcpType TcpData::valueOfTrkType(const std::string& t) {
		if (t == toStringTrkType(TcpData::NONE)) return TcpData::NONE;
		if (t == toStringTrkType(TcpData::BOT)) return TcpData::BOT;
		if (t == toStringTrkType(TcpData::EOT)) return TcpData::EOT;
		if (t == toStringTrkType(TcpData::EOTBOT)) return TcpData::EOTBOT;
		return TcpData::UNKNOWN_TRK;
	}



	std::string TcpData::getGsTypeString() const {
		return toStringGsType(tcpGs);
	}
	std::string TcpData::toStringGsType(GsTcpType t) {
		static const std::string NONE_STR = "NONE";
		static const std::string BGS_STR = "BGS";
		static const std::string EGS_STR = "EGS";
		static const std::string EGSBGS_STR = "EGSBGS";
		static const std::string ELSE_STR = "ELSE";

		if (t == TcpData::NONEg) return NONE_STR;
		if (t == TcpData::BGS) return BGS_STR;
		if (t == TcpData::EGS) return EGS_STR;
		if (t == TcpData::EGSBGS) return EGSBGS_STR;
		return ELSE_STR;
	}

	TcpData::GsTcpType TcpData::valueOfGsType(const std::string& t) {
		if (t == toStringGsType(TcpData::NONEg)) return TcpData::NONEg;
		if (t == toStringGsType(TcpData::BGS)) return TcpData::BGS;
		if (t == toStringGsType(TcpData::EGS)) return TcpData::EGS;
		if (t == toStringGsType(TcpData::EGSBGS)) return TcpData::EGSBGS;
		return TcpData::UNKNOWN_GS;
	}


	std::string TcpData::getVsTypeString() const {
		return toStringVsType(tcpVs);
	}
	std::string TcpData::toStringVsType(VsTcpType t) {
		static const std::string NONE_STR = "NONE";
		static const std::string BVS_STR = "BVS";
		static const std::string EVS_STR = "EVS";
		static const std::string EVSBVS_STR = "EVSBVS";
		static const std::string ELSE_STR = "ELSE";

		if (t == TcpData::NONEv) return NONE_STR;
		if (t == TcpData::BVS) return BVS_STR;
		if (t == TcpData::EVS) return EVS_STR;
		if (t == TcpData::EVSBVS) return EVSBVS_STR;
		return ELSE_STR;
	}

	TcpData::VsTcpType TcpData::valueOfVsType(const std::string& t) {
		if (t == toStringVsType(TcpData::NONEv)) return TcpData::NONEv;
		if (t == toStringVsType(TcpData::BVS)) return TcpData::BVS;
		if (t == toStringVsType(TcpData::EVS)) return TcpData::EVS;
		if (t == toStringVsType(TcpData::EVSBVS)) return TcpData::EVSBVS;
		return TcpData::UNKNOWN_VS;
	}
	
	double TcpData::getRadiusSigned() const {
		return radiusSigned;
	}

	double TcpData::signedRadius() const {
		return radiusSigned;
	}

	double TcpData::turnRadius() const {
		//fpln("NavPoint::turnRadius "+Fmb(isTurn())+" "+Fm2(accel_d)+" "+velocityIn_v.toString());
		return std::abs(radiusSigned);
	}

	Position TcpData::turnCenter() const {
		return centerTurn;
	}

	TcpData TcpData::setTurnCenter(const Position& center) {
		centerTurn = center;
		return *this;
	}

	TcpData TcpData::setRadiusSigned(double r) {
		radiusSigned = r;
		return *this;
	}

//	TcpData TcpData::setTrkAccel(double omega) {
//		double radius = velocityInit_v.gs()/omega;
//		radiusSigned = radius;
//		return *this;
//	}
//
//
//	double TcpData::trkAccel() const {
//		//return accel_trk;
//		double rtn = 0.0;
//		if (std::abs(radiusSigned) > 0) {
//		    rtn = velocityInit_v.gs()/radiusSigned;
//		}
//		//fpln(" $$$ trkAccel: radiusSigned = "+Units::str("NM",radiusSigned)+" rtn = "+Units::str("deg/s",rtn));
//		return rtn;
//	}
	
	double TcpData::getGsAccel() const {
		return accelGs;
	}

	double TcpData::gsAccel() const {
		return accelGs;
	}

	TcpData TcpData::setGsAccel(double ga) {
		accelGs = ga;
		return *this;
	}

	double TcpData::getVsAccel() const {
		return accelVs;
	}

	double TcpData::vsAccel() const {
		return accelVs;
	}

	TcpData TcpData::setVsAccel(double va) {
		accelVs = va;
		return *this;
	}

//	Velocity TcpData::getVelocityInit() const {
//		return velocityInit_v;
//	}
//
//	Velocity TcpData::velocityInit() const {
//		return velocityInit_v;
//	}

//	TcpData TcpData::setVelocityInit(const Velocity& v) {
//		velocityInit_v = v;
//		return *this;
//	}

	Position TcpData::getSourcePosition() const {
		return sourcePosition_p;
	}

//	Position TcpData::sourcePosition() const {
//		return sourcePosition_p;
//	}

	NavPoint TcpData::sourceNavPoint() const {
		return NavPoint(sourcePosition_p,sourceTime_d);
	}


	bool TcpData::hasSource() const {
    	return sourceTime_d >= 0;
    }


	TcpData TcpData::setSource(const Position& sp, double st) {
		sourcePosition_p = sp;
		sourceTime_d = st;
		return *this;
	}

	TcpData TcpData::setSource(const NavPoint& np) {
		sourcePosition_p = np.position();
		sourceTime_d = np.time();
		return *this;
	}

	TcpData TcpData::makeSource(const NavPoint& np) {
		TcpData tcp = TcpData();
		tcp.sourcePosition_p = np.position();
		tcp.sourceTime_d = np.time();
		return tcp;
	}


	TcpData TcpData::setSourcePosition(const Position& sp) {
		sourcePosition_p = sp;
		return *this;
	}

	double TcpData::getSourceTime() const {
		return sourceTime_d;
	}

//	double TcpData::sourceTime() const {
//		return sourceTime_d;
//	}

	TcpData TcpData::setSourceTime(double st) {
		sourceTime_d = st;
		return *this;
	}

	int TcpData::getLinearIndex() const {
		return linearIndex;
	}

	TcpData TcpData::setLinearIndex(int ix) {
		linearIndex = ix;
		return *this;
	}

	std::string TcpData::getInformation() const {
		return information;
	}


	void TcpData::setInformation(const std::string& information) {
		this->information = information;
	}
	
	TcpData TcpData::makeFull(const std::string& ty, const std::string& tcp_trk, const std::string& tcp_gs, const std::string& tcp_vs,
			double radiusSigned, const Position& center, double accel_gs, double accel_vs,
			const Velocity& velocityInit, const Position& sourcep, double sourcet, int linearIndex) {
		
		TcpData d;
		d.ty = valueOfWayType(trimCopy(ty));
		d.tcpTrk = valueOfTrkType(toUpperCase(trimCopy(tcp_trk)));
		d.tcpGs = valueOfGsType(toUpperCase(trimCopy(tcp_gs)));
		d.tcpVs = valueOfVsType(toUpperCase(trimCopy(tcp_vs)));
		d.setRadiusSigned(radiusSigned);
		d.setTurnCenter(center);
		d.setGsAccel(accel_gs);
		d.setVsAccel(accel_vs);
		//d.setVelocityInit(velocityInit);
		d.setSource(sourcep, sourcet);
		d.setLinearIndex(linearIndex);
		//d.setInformation(information);
		
		return d;
	}
	
	TcpData TcpData::makeInvalid() {
		TcpData d;
		d.setInformation("INVALID");
		
		return d;
	}
	
	TcpData TcpData::makeOrig() {
		TcpData d = TcpData();
		return d;
	}

	
	TcpData TcpData::setBOT(double signedRadius, const Position& center, int linearIndex) {
		//velocityInit_v = velocityIn;
		radiusSigned = signedRadius;
		centerTurn = center;
		this->linearIndex = linearIndex;
		tcpTrk = TcpData::BOT;
		return *this;
	}
	
//	TcpData TcpData::setBotAccel(double trkAccel, int linearIndex) {
//		double radius = velocityIn.gs()/trkAccel;
//		return setBOT(velocityIn, radius, linearIndex);
//	}




	TcpData TcpData::setEOT(int linearIndex) {
		//velocityInit_v = velocityIn;
		this->linearIndex = linearIndex;
		tcpTrk = TcpData::EOT;
		return *this;
	}

	TcpData TcpData::setEOTBOT(double signedRadius, const Position& center, int linearIndex) {
		//velocityInit_v = velocityIn;
		radiusSigned = signedRadius;
		centerTurn = center;
		this->linearIndex = linearIndex;
		tcpTrk = TcpData::EOTBOT;
		return *this;
	}

	TcpData TcpData::setBGS(double a, int linearIndex) {
		//velocityInit_v = velocityIn;
		accelGs = a;
		this->linearIndex = linearIndex;
		tcpGs = TcpData::BGS;
		return *this;
	}

	void TcpData::clearGs() {
		tcpGs = TcpData::NONEg;
	}

	TcpData TcpData::setEGS(int linearIndex) {
		//velocityInit_v = velocityIn;
		this->linearIndex = linearIndex;
		tcpGs = TcpData::EGS;
		return *this;
	}

	TcpData TcpData::setEGSBGS(double a, int linearIndex) {
		//velocityInit_v = velocityIn;
		accelGs = a;
		this->linearIndex = linearIndex;
		tcpGs = TcpData::EGSBGS;
		return *this;
	}

	TcpData TcpData::setBVS(double a, int linearIndex) {
		//velocityInit_v = velocityIn;
		accelVs = a;
		this->linearIndex = linearIndex;
		tcpVs = TcpData::BVS;
		return *this;
	}

	void TcpData::clearVs() {
		tcpVs = TcpData::NONEv;
	}

	TcpData TcpData::setEVS(int linearIndex) {
		//velocityInit_v = velocityIn;
		this->linearIndex = linearIndex;
		tcpVs = TcpData::EVS;
		return *this;
	}

	TcpData TcpData::setEVSBVS(double a, int linearIndex) {
		//velocityInit_v = velocityIn;
		accelVs = a;
		this->linearIndex = linearIndex;
		tcpVs = TcpData::EVSBVS;
		return *this;
	}

	void TcpData::clearTrk() {
		tcpTrk = TcpData::NONE;
	}

	TcpData TcpData::clearBOT() {
		if (tcpTrk == TcpData::BOT) {
			tcpTrk = TcpData::NONE;
		} else if (tcpTrk == TcpData::EOTBOT) {
			tcpTrk = TcpData::EOT;
		}
		return *this;
	}

	TcpData TcpData::clearEOT() {
		if (tcpTrk == TcpData::EOT) {
			tcpTrk = TcpData::NONE;
		} else if (tcpTrk == TcpData::EOTBOT) {
			tcpTrk = TcpData::BOT;
		}
		return *this;
	}


	TcpData TcpData::clearBGS() {
		if (tcpGs == TcpData::BGS) {
			tcpGs = TcpData::NONEg;
		} else if (tcpGs == TcpData::EGSBGS) {
			tcpGs = TcpData::EGS;
		}
		return *this;
	}

	TcpData TcpData::clearEGS() {
		if (tcpGs == TcpData::EGS) {
			tcpGs = TcpData::NONEg;
		} else if (tcpGs == TcpData::EGSBGS) {
			tcpGs = TcpData::BGS;
		}
		return *this;
	}

	TcpData TcpData::clearBVS() {
		if (tcpVs == TcpData::BVS) {
			tcpVs = TcpData::NONEv;
		} else if (tcpVs == TcpData::EVSBVS) {
			tcpVs = TcpData::EVS;
		}
		return *this;
	}

	TcpData TcpData::clearEVS() {
		if (tcpVs == TcpData::EVS) {
			tcpVs = TcpData::NONEv;
		} else if (tcpVs == TcpData::EVSBVS) {
			tcpVs = TcpData::BVS;
		}
		return *this;
	}


//	/** Makes a new NavPoint that is an intermediate "mid" added point */
//	TcpData makeMidpoint(Position p, double t, int linearIndex) {
//		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,   this.tcp_gs, this.tcp_vs, 
//				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit,  this.sourcePosition, this.sourceTime, linearIndex);
//		return *this;
//	}
	
	
	
	
	
	bool TcpData::isInvalid() const {
		return information == "INVALID";
	}
	
	bool TcpData::isTCP() const {
		return tcpTrk != TcpData::NONE || tcpGs != TcpData::NONEg || tcpVs != TcpData::NONEv;
	}

	bool TcpData::isTrkTCP() const {
		return tcpTrk != TcpData::NONE;
	}

	bool TcpData::isBOT() const {
		return tcpTrk == TcpData::BOT || tcpTrk == TcpData::EOTBOT;
	}

	bool TcpData::isEOT() const {
		return tcpTrk == TcpData::EOT || tcpTrk == TcpData::EOTBOT;
	}

	bool TcpData::isGsTCP() const {
		return tcpGs != TcpData::NONEg;
	}

	bool TcpData::isBGS() const {
		return tcpGs == TcpData::BGS || tcpGs == TcpData::EGSBGS;
	}

	bool TcpData::isEGS() const {
		return tcpGs == TcpData::EGS || tcpGs == TcpData::EGSBGS;
	}

	bool TcpData::isVsTCP() const {
		return tcpVs != TcpData::NONEv;
	}

	bool TcpData::isBVS() const {
		return tcpVs == TcpData::BVS || tcpVs == TcpData::EVSBVS;
	}

	bool TcpData::isEVS() const {
		return tcpVs == TcpData::EVS || tcpVs == TcpData::EVSBVS;
	}

	bool TcpData::isBeginTCP() const {
		return tcpTrk == TcpData::BOT || tcpGs == TcpData::BGS || tcpVs == TcpData::BVS ||
				tcpTrk == TcpData::EOTBOT || tcpGs == TcpData::EGSBGS || tcpVs == TcpData::EVSBVS;
	}

	bool TcpData::isEndTCP() const {
		return tcpTrk == TcpData::EOT || tcpGs == TcpData::EGS || tcpVs == TcpData::EVS ||
				tcpTrk == TcpData::EOTBOT || tcpGs == TcpData::EGSBGS || tcpVs == TcpData::EVSBVS;
	}
	
	std::string TcpData::fullLabel(const NavPoint& p, const TcpData& d) {
		return p.label()+d.metaDataLabel(p.time(),4);
	}

	std::pair<TcpData,std::string> TcpData::parseMetaDataLabel(const NavPoint& default_source, const std::string& tlabel) {
		//fpln(" $$$$$ parseMetaDataLabel: tlabel = "+tlabel);
		int i = tlabel.find(":ACC:");
		if (i >= 0) {
			return std::pair<TcpData,std::string>(TcpData::makeInvalid(),"INVALID"); //NavPoint::INVALID;
		}
		TcpData point(*this);
		int lowIndex = tlabel.length(); // then current end of the user label
		int j;
		i = tlabel.find(":VIRT:");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.setVirtual();
		}

		i = tlabel.find(":ADDED:");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.setSource(Position::INVALID(),-1.0);
		} else {
			Position sp = default_source.position();
			double st = default_source.time();
			i = tlabel.find(":SRC:");
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				j = tlabel.find(":",i+5);
				// first try general parse, then coordinate-specific if that fails
				std::string t = tlabel.substr(i+5, j);
				std::replace(t.begin(), t.end(), '_', ' ');
				sp = Position::parse(t);
				if (sp.isInvalid()) {
					if (default_source.isLatLon()) {
						sp = Position::parseLL(t);
					} else {
						sp = Position::parseXYZ(t);
					}
				}
			}
			i = tlabel.find(":STM:");
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				j = tlabel.find(":",i+5);
				st = Util::parse_double(tlabel.substr(i+5,j));
			}
			//fpln(" $$$$$ parseMetaDataLabel: i = "+i+" st = "+st);
			point = point.setSource(sp,st);			
		}

		Velocity v = Velocity::ZEROV();
		i = tlabel.find(":VEL:"); // velocity in
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+5);
			std::string t = tlabel.substr(i+5, j);
			std::replace(t.begin(), t.end(), '_', ' ');
			v = Velocity::parse(t);
			//point = point.setVelocityInit(v);
		}
		i = tlabel.find(":ATRK:"); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+6);
			double a = Util::parse_double(tlabel.substr(i+6,j));
			//point = point.setTrkAccel(a);
			double r = 0.0;
			if ( a != 0.0) { // avoid divide by zero
				r = v.gs()/a;
			}
			point.setRadiusSigned(r);
		}
		i = tlabel.find(":AGS:"); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+5);
			double a = Util::parse_double(tlabel.substr(i+5,j));
			point = point.setGsAccel(a);
		}
		i = tlabel.find(":AVS:"); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = tlabel.find(":",i+5);
			double a = Util::parse_double(tlabel.substr(i+5,j));
			point = point.setVsAccel(a);
		}
		for (int iter = 0; iter <=4; iter++) {
			TrkTcpType t = toTrkTCP(iter);
			i = tlabel.find(":"+toStringTrkType(t)+":");
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				point.tcpTrk = t;
			}
		}
		for (int iter = 0; iter <=4; iter++) {
			GsTcpType t = toGsTCP(iter);
			i = tlabel.find(":"+toStringGsType(t)+":");
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				point.tcpGs = t;
			}
		}
		for (int iter = 0; iter <=4; iter++) {
			VsTcpType t = toVsTCP(iter);
			i = tlabel.find(":"+toStringVsType(t)+":");
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				point.tcpVs = t;
			}
		}
		std::string name = "";   //tlabel;
 		if (lowIndex > 0) { //lowIndex >= 0 && lowIndex < (int) tlabel.length()) {
			name = tlabel.substr(0,lowIndex);
			//point = point.makeLabel(name);
		}
		return std::pair<TcpData,std::string>(point, name);
	}


	std::string TcpData::tcpTypeString() const {
		std::string s = "";
		if (tcpTrk != TcpData::NONE) s += "tcpTrk";
		if (tcpGs != TcpData::NONEg) {
			if ( s != "") s+= ",";
			s += "tcpGs";
		}
		if (tcpVs != TcpData::NONEv) {
			if ( s !="" ) s+= ",";
			s += "tcpVs";
		}
		if (s=="") s = "NONE";
		return s;
	}

	std::string TcpData::metaDataLabel(double t, int precision) const {
		std::string tlabel = "";
		if (isVirtual()) {
			tlabel = tlabel + "VIRT:";
		} 
		if (isTrkTCP()) {
			tlabel = tlabel + toStringTrkType(tcpTrk) +":";
			if (isBOT()) {
				tlabel = tlabel + "ATRK:"+FmPrecision(radiusSigned,precision)+":";
			}
		}
		if (isGsTCP()) {
			tlabel = tlabel + toStringGsType(tcpGs) +":";
			if (isBGS()) {
				tlabel = tlabel + "AGS:"+FmPrecision(accelGs,precision)+":";
			}
		}
		if (isVsTCP()) {
			tlabel = tlabel + toStringVsType(tcpVs) +":";
			if (isBVS()) {
				tlabel = tlabel + "AVS:"+FmPrecision(accelVs,precision)+":";
			}
		}
//		if ( /*velocityInit != null &&*/ !velocityInit_v.isInvalid()) {
//			std::string v = velocityInit_v.toStringNP(precision);
//			replace(v, ',', '_');
//			replace(v, ' ', '_');
//
//			//std::string v = velocityInit_v.toStringNP(precision).replaceAll("[, ]+", "_");
//			tlabel = tlabel + "VEL:"+v+":";
//		}
		if (sourceTime_d < 0) { // MOT or other added points
			tlabel = tlabel + "ADDED:";
		} else if (isTCP()){ // generated TCP points
			std::string ttt = sourcePosition_p.toStringNP(precision);
			replace(ttt, ',', '_');
			replace(ttt, ' ', '_');
			tlabel = tlabel + "SRC:"+ttt+":";
			//tlabel = tlabel + "SRC:"+sourcePosition::toStringNP(precision).replaceAll("[, ]+", "_")+":";
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

	std::pair<NavPoint,TcpData> TcpData::parseLL(const std::string& s) {
		std::vector<string> fields = split(s, Constants::wsPatternParens);
		Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util::parse_double(fields[3]);
		Position pos = Position::makeLatLonAlt(v.x, v.y, v.z);
		if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
			std::string extra = fields[MIN_OUTPUT_COLUMNS];

			NavPoint np(pos, time, extra);
			std::pair<TcpData,std::string> p = TcpData().parseMetaDataLabel(np,extra);
			if ( p.second != "") {
				np.makeLabel(p.second);
			}
			return std::pair<NavPoint,TcpData>(np,p.first);
		} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
			std::string wt = fields[4];
			Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
			std::string trkty = fields[8];
			//double trkacc = Units::from("NM", Util::parse_double(fields[9]));
			std::string gsty = fields[10];
			double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
			std::string vsty = fields[12];
			double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
			double radius = Util::parse_double(fields[14]);
			LatLonAlt slla = LatLonAlt::parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp(slla);
			double st = Util::parse_double(fields[18]);
			std::string lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
			Position turnCenter = KinematicsPosition::centerFromRadius(pos, radius, vv.trk());
			return std::pair<NavPoint, TcpData>(NavPoint(pos, time, lab), TcpData::makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc, vv, sp, st, -1));
		} else {
			return std::pair<NavPoint, TcpData>(NavPoint(pos, time), TcpData());
		}
	}

	std::pair<NavPoint,TcpData> TcpData::parseXYZ(const std::string& s) {
		std::vector<string> fields = split(s, Constants::wsPatternParens);
		//std::pair<PointMutability,String> extra = parseExtra(fields);
		Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util::parse_double(fields[3]);
		Position pos = Position::makeXYZ(v.x, v.y, v.z);
		if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
			std::string extra = fields[MIN_OUTPUT_COLUMNS];
			NavPoint np(pos, time, extra);
			std::pair<TcpData,std::string> p = TcpData().parseMetaDataLabel(np,extra);
			if ( p.second != "") {
				np.makeLabel(p.second);
			}
			return std::pair<NavPoint,TcpData>(np,p.first);
			//return (new NavPoint(pos, time, extra)).parseMetaDataLabel(extra);
		} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
			std::string wt = fields[4];
			Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
			std::string trkty = fields[8];
			//double accel_trk = Units::from("deg/s", Util::parse_double(fields[9]));
			std::string gsty = fields[10];
			double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
			std::string vsty = fields[12];
			double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
			double radius = Util::parse_double(fields[14]);
			Vect3 sv = Vect3::parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp = Position::makeXYZ(sv.x, sv.y, sv.z);
			double st = Util::parse_double(fields[18]);
			std::string lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
			Position turnCenter = KinematicsPosition::centerFromRadius(pos, radius, vv.trk());
			return std::pair<NavPoint, TcpData>(NavPoint(pos, time, lab), TcpData::makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc, vv, sp, st, -1));
		} else {
			return std::pair<NavPoint,TcpData>(NavPoint(pos, time), TcpData());
		}
	}

//	NavPoint TcpData::parseLL(const std::string& s) {
//		std::vector<string> fields = split(s, Constants::wsPatternParens);
//		Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
//		double time = Util::parse_double(fields[3]);
//		Position pos = Position::makeLatLonAlt(v.x, v.y, v.z);
//		if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
//			std::string  extra = fields[MIN_OUTPUT_COLUMNS];
//			return NavPoint(pos, time, extra).parseMetaDataLabel(extra);
//		} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
//			WayType wt = WayTypeValueOf(fields[4]);
//			Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
//			Trk_TCPType trkty = Trk_TCPTypeValueOf(fields[8]);
//			//double trkacc = Units::from("deg/s", Util::parse_double(fields[9]));
//			Gs_TCPType gsty = Gs_TCPTypeValueOf(fields[10]);
//			double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
//			Vs_TCPType vsty = Vs_TCPTypeValueOf(fields[12]);
//			double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
//			double radius = Units::from("nmi", Util::parse_double(fields[14]));
//			LatLonAlt slla = LatLonAlt::parse(fields[15]+" "+fields[16]+" "+fields[17]);
//			Position sp = Position(slla);
//			double st = Util::parse_double(fields[18]);
//			std::string lab = fields[19];
//			int linearIndex = -1;   // need to fix this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//			return NavPoint(pos, time, wt, lab, trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, linearIndex);
//		} else {
//			return NavPoint(pos, time);
//		}
//	}
//
//	NavPoint NavPoint::parseXYZ(const std::string& s) {
//		std::vector<string> fields = split(s, Constants::wsPatternParens);
//		Vect3 v = Vect3::parse(fields[0]+" "+fields[1]+" "+fields[2]);
//		double time = Util::parse_double(fields[3]);
//		Position pos = Position::makeXYZ(v.x, v.y, v.z);
//		if (fields.size() == MIN_OUTPUT_COLUMNS+1) {
//			std::string  extra = fields[MIN_OUTPUT_COLUMNS];
//			return NavPoint(pos, time, extra).parseMetaDataLabel(extra);
//		} else if (fields.size() == TCP_OUTPUT_COLUMNS+1) {
//			WayType wt = WayTypeValueOf(fields[4]);
//			Velocity vv = Velocity::parse(fields[5]+" "+fields[6]+" "+fields[7]);
//			Trk_TCPType trkty = Trk_TCPTypeValueOf(fields[8]);
//			//double trkacc = Units::from("deg/s", Util::parse_double(fields[9]));
//			Gs_TCPType gsty = Gs_TCPTypeValueOf(fields[10]);
//			double gsacc = Units::from("m/s^2", Util::parse_double(fields[11]));
//			Vs_TCPType vsty = Vs_TCPTypeValueOf(fields[12]);
//			double vsacc = Units::from("m/s^2", Util::parse_double(fields[13]));
//			double radius = Units::from("nmi", Util::parse_double(fields[14]));
//			Vect3 sv = Vect3::parse(fields[15]+" "+fields[16]+" "+fields[17]);
//			Position sp = Position::makeXYZ(sv.x, sv.y, sv.z);
//			double st = Util::parse_double(fields[18]);
//			std::string lab = fields[19];
//			int linearIndex = -1;   // need to fix this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//			return NavPoint(pos, time, wt, lab, trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, linearIndex);
//		} else {
//			return NavPoint(pos, time);
//		}}


	/**
	 * Can this point be merged with the given point p?  Merged points only apply to TCP points.
	 * @param point the other point
	 * @return true, if the points can be merged.
	 */
	bool TcpData::mergeable(const TcpData& point) const {
		// these are bad!!!
		bool r1 = (isBOT() && point.isBOT()) || (isEOT() && point.isEOT());
		bool r2 = (isBGS() && point.isBGS()) || (isEGS() && point.isEGS());
		bool r3 = (isBVS() && point.isBVS()) || (isEVS() && point.isEVS());
		return !r1 && !r2 && !r3;
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
	TcpData TcpData::mergeTCPInfo(const TcpData& point) const {
		TcpData d;
		// position & time -- keep either (should be the same)

		WayType my_ty = (ty == TcpData::AltPreserve || point.ty == TcpData::AltPreserve) ? TcpData::AltPreserve :
				((ty == TcpData::Orig || point.ty == TcpData::Orig) ? TcpData::Orig : TcpData::Virtual);
		d.ty = my_ty;
		//std::string my_label = label_s+point.label_s;

		d.tcpTrk = ((isBOT() && point.isEOT()) || (point.isBOT() && isEOT())) ? TcpData::EOTBOT :
				((isBOT() || point.isBOT()) ? TcpData::BOT :
						((isEOT() || point.isEOT()) ? TcpData::EOT : TcpData::NONE));
		d.tcpGs = ((isBGS() && point.isEGS()) || (point.isBGS() && isEGS())) ? TcpData::EGSBGS :
				((isBGS() || point.isBGS()) ? TcpData::BGS :
						((isEGS() || point.isEGS()) ? TcpData::EGS : TcpData::NONEg));
		d.tcpVs = ((isBVS() && point.isEVS()) || (point.isBVS() && isEVS())) ? TcpData::EVSBVS :
				((isBVS() || point.isBVS()) ? TcpData::BVS :
						((isEVS() || point.isEVS()) ? TcpData::EVS : TcpData::NONEv));

		double my_radius = 0.0; // (this.tcp_trk != Trk_TCPType.NONE || point.tcp_trk != Trk_TCPType.NONE) ? 0 : this.sgnRadius;
		if (tcpTrk == BOT || tcpTrk == EOTBOT) my_radius = radiusSigned;
		else my_radius = point.radiusSigned;

		d.setRadiusSigned(my_radius);

		double my_accel_gs = (tcpGs == BGS || tcpGs == EGSBGS) ? accelGs : point.accelGs;
		double my_accel_vs = (tcpVs == BVS || tcpVs == EVSBVS) ? accelVs : point.accelVs;

		d.setGsAccel(my_accel_gs);
		d.setVsAccel(my_accel_vs);

//		Velocity my_velocityIn;
//		if (isBeginTCP()) {
//			my_velocityIn = velocityInit_v;
//		} else {
//			my_velocityIn = point.velocityInit_v;
//		}
		//d.setVelocityInit(my_velocityIn);

		Position my_sourcePosition;
		double my_sourceTime;
		int my_linearIndex;
		if (isBeginTCP()) {
			my_sourcePosition = sourcePosition_p;
			my_sourceTime = sourceTime_d;
			my_linearIndex = linearIndex;
		} else {
			my_sourcePosition = point.sourcePosition_p;
			my_sourceTime = point.sourceTime_d;
			my_linearIndex = point.linearIndex;
		}

		//fpln("sourceTime "+Fm4(sourceTime_d)+"  "+Fm4(sourceTime_d)+"  "+Fm4(point.sourceTime_d));
		d.setSource(my_sourcePosition, my_sourceTime);
		d.setLinearIndex(my_linearIndex);

		return d;
	}


	TcpData::TrkTcpType TcpData::toTrkTCP(int ty) {
		if (ty == 1) return TcpData::BOT;
		if (ty == 2) return TcpData::EOT;
		if (ty == 3) return TcpData::EOTBOT;
		return TcpData::NONE;
	}

	TcpData::GsTcpType TcpData::toGsTCP(int ty) {
		if (ty == 1) return TcpData::BGS;
		if (ty == 2) return TcpData::EGS;
		if (ty == 3) return TcpData::EGSBGS;
		return TcpData::NONEg;
	}

	TcpData::VsTcpType TcpData::toVsTCP(int ty) {
		if (ty == 1) return TcpData::BVS;
		if (ty == 2) return TcpData::EVS;
		if (ty == 3) return TcpData::EVSBVS;
		return TcpData::NONEv;
	}

	std::string TcpData::toString() const {
		return "TcpData [ty=" + toStringWayType(ty) + " <" + tcpTypeString() + ">, radiusSigned="
				+ Fm4(radiusSigned) + " turnCenter ="+ centerTurn.toString() +
				+ ", accelGs=" + Fm4(accelGs) + ", accelVs=" + Fm4(accelVs) // + ", velocityInit=" + velocityInit_v.toString()
				//+ ", sourcePosition=" + sourcePosition_p.toString() + ", sourceTime=" + Fm4(sourceTime_d)
				+ ", linearIndex=" + Fmi(linearIndex) + ", information=" + information + "]";
	}


}
