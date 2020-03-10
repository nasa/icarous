/*
 * TcpData
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2016-2019 United States Government as represented by
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

	bool TcpData::motFlagInInfo = false;
	std::string TcpData::MOTflag = ".<MOT>.";

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
		information = "";
	}

	std::string TcpData::getTrkTypeString() const {
		return toStringTrkType(tcpTrk);
	}
	std::string TcpData::toStringTrkType(TrkTcpType t) {
		static const std::string NONE_STR = "NONE";
		static const std::string BOT_STR = "BOT";
		static const std::string MOT_STR = "MOT";
		static const std::string EOT_STR = "EOT";
		static const std::string EOTBOT_STR = "EOTBOT";
		static const std::string ELSE_STR = "ELSE";

		if (t == TcpData::NONE) return NONE_STR;
		if (t == TcpData::BOT) return BOT_STR;
		if (t == TcpData::MOT) return MOT_STR;
		if (t == TcpData::EOT) return EOT_STR;
		if (t == TcpData::EOTBOT) return EOTBOT_STR;
		return ELSE_STR;
	}

	TcpData::TrkTcpType TcpData::valueOfTrkType(const std::string& t) {
		if (t == toStringTrkType(TcpData::NONE)) return TcpData::NONE;
		if (t == toStringTrkType(TcpData::BOT)) return TcpData::BOT;
		if (t == toStringTrkType(TcpData::MOT)) return TcpData::MOT;
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
	
	// Note: a return value of 0 indicates no specified radius
	double TcpData::getRadiusSigned() const {
		return radiusSigned;
	}


	int  TcpData::turnDir() const {
		//fpln("NavPoint::turnRadius "+Fmb(isTurn())+" "+Fm2(accel_d)+" "+velocityIn_v.toString());
		return Util::sign(radiusSigned);
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

	double TcpData::getGsAccel() const {
		return accelGs;
	}

	TcpData TcpData::setGsAccel(double ga) {
		accelGs = ga;
		return *this;
	}

	double TcpData::getVsAccel() const {
		return accelVs;
	}

	TcpData TcpData::setVsAccel(double va) {
		accelVs = va;
		return *this;
	}


	std::string TcpData::getInformation() const {
		return information;
	}


	void TcpData::setInformation(const std::string& info) {
		information = info;
	}


	void TcpData::appendInformation(const std::string&  info) {
		if (info.length() == 0 || information.length() == 0) {
			information += info;
		} else {
			information += ":"+info;
		}
	}

	void TcpData::appendInformationNoDuplication(const std::string&  info) {
		if (contains(information,info) ||		// do nothing if this string is already in the label
				contains(info,"$virtual") || // do not add virtual labels
				!startsWith(info,"$")) {
			//TODO fix me!
		} else {
			appendInformation(info);
		}
	}

	
	TcpData TcpData::makeFull(const std::string& ty, const std::string& tcp_trk, const std::string& tcp_gs, const std::string& tcp_vs,
			double radiusSigned, const Position& center, double accel_gs, double accel_vs) {
		
		TcpData d;
		d.ty = valueOfWayType(trimCopy(ty));
		d.tcpTrk = valueOfTrkType(toUpperCase(trimCopy(tcp_trk)));
		d.tcpGs = valueOfGsType(toUpperCase(trimCopy(tcp_gs)));
		d.tcpVs = valueOfVsType(toUpperCase(trimCopy(tcp_vs)));
		d.setRadiusSigned(radiusSigned);
		d.setTurnCenter(center);
		d.setGsAccel(accel_gs);
		d.setVsAccel(accel_vs);
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

	
	TcpData TcpData::setBOT(double signedRadius, const Position& center) {
		radiusSigned = signedRadius;
		centerTurn = center;
		tcpTrk = TcpData::BOT;
		return *this;
	}
	

	TcpData TcpData::setEOT() {
		tcpTrk = TcpData::EOT;
		return *this;
	}


	TcpData TcpData::addBOT(double signedRadius, Position center) {
		radiusSigned = signedRadius;
		centerTurn = center;
		if (tcpTrk == TcpData::EOT) tcpTrk = TcpData::EOTBOT;
		else tcpTrk = TcpData::BOT;
		return *this;
	}

	TcpData TcpData::addEOT() {
		if (tcpTrk == TcpData::BOT) tcpTrk = TcpData::EOTBOT;
		else tcpTrk = TcpData::EOT;
		return *this;
	}


	TcpData TcpData::setMOT(bool b) {
		if (TcpData::motFlagInInfo) {
			setInformation(getInformation()+MOTflag);
		} else {
		    if (b) tcpTrk = TcpData::MOT;
		    else if (!isTrkTCP()) tcpTrk = TcpData::NONE;
			//isMOTflag = b;
		}
		return *this;
	}



	TcpData TcpData::setEOTBOT(double signedRadius, const Position& center) {
		radiusSigned = signedRadius;
		centerTurn = center;
		tcpTrk = TcpData::EOTBOT;
		return *this;
	}

	TcpData TcpData::setBGS(double a) {
		accelGs = a;
		tcpGs = TcpData::BGS;
		return *this;
	}

	TcpData TcpData::addEGS() {
		if (tcpGs == TcpData::BGS) tcpGs = TcpData::EGSBGS;
		else tcpGs = TcpData::EGS;
		return *this;
	}


	void TcpData::clearGs() {
		tcpGs = TcpData::NONEg;
	}

	TcpData TcpData::setEGS() {
		tcpGs = TcpData::EGS;
		return *this;
	}

	TcpData TcpData::setEGSBGS(double a) {
		accelGs = a;
		tcpGs = TcpData::EGSBGS;
		return *this;
	}

	TcpData TcpData::setBVS(double a) {
		accelVs = a;
		tcpVs = TcpData::BVS;
		return *this;
	}

	void TcpData::clearVs() {
		tcpVs = TcpData::NONEv;
	}

	TcpData TcpData::setEVS() {
		tcpVs = TcpData::EVS;
		return *this;
	}

	TcpData TcpData::addEVS() {
		if (tcpVs == TcpData::BVS) tcpVs = TcpData::EVSBVS;
		else tcpVs = TcpData::EVS;
		return *this;
	}


	TcpData TcpData::setEVSBVS(double a) {
		accelVs = a;
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

	bool TcpData::isInvalid() const {
		return information == "INVALID";
	}
	
	bool TcpData::isTCP() const {
		return isTrkTCP() || tcpGs != TcpData::NONEg || tcpVs != TcpData::NONEv;
	}

	bool TcpData::isTrkTCP() const {   // the MOT is not strictly a track TCP
		return tcpTrk != TcpData::NONE  && tcpTrk != TcpData::MOT;
	}

	bool TcpData::isBOT() const {
		return tcpTrk == TcpData::BOT || tcpTrk == TcpData::EOTBOT;
	}

	bool TcpData::isEOT() const {
		return tcpTrk == TcpData::EOT || tcpTrk == TcpData::EOTBOT;
	}

	bool TcpData::isMOT() const {
		if (TcpData::motFlagInInfo) return contains(information, TcpData::MOTflag);
		else {
			return tcpTrk == TcpData::MOT;
			//return isMOTflag;
		}
	}


	bool TcpData::isGsTCP() const {
		return tcpGs != TcpData::NONEg;
	}

	bool TcpData::isEGSBGS() const {
		return tcpGs == TcpData::EGSBGS;
	}


	bool TcpData::isBGS() const {
		return tcpGs == TcpData::BGS || tcpGs == TcpData::EGSBGS;
	}

	bool TcpData::isEGS() const {
		return tcpGs == TcpData::EGS || tcpGs == TcpData::EGSBGS;
	}


	bool TcpData::isEVSBVS() const {
		return tcpVs == TcpData::EVSBVS;
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
	
	bool TcpData::isDefined() const {
		if (isBOT()) {
			if (radiusSigned != radiusSigned) return false;
			if (centerTurn.isInvalid()) return false;
		}
		if (isBGS() && accelGs != accelGs) return false;
		if (isBVS() && accelVs != accelVs) return false;
		return true;
	}

	std::string TcpData::fullName(const NavPoint& p, const TcpData& d) {
		return p.name()+d.metaDataLabel(4);
	}

	std::pair<TcpData,std::string> TcpData::parseMetaDataLabel(const std::string& tlabel) {
		//fpln(" $$$$$ parseMetaDataLabel: tlabel = "+tlabel);
		int i = static_cast<int>(tlabel.find(":ACC:"));
		if (i >= 0) {
			return std::pair<TcpData,std::string>(TcpData::makeInvalid(),"INVALID"); //NavPoint::INVALID;
		}
		TcpData point(*this);
		int lowIndex = tlabel.length(); // then current end of the user label
		int j;
		i = static_cast<int>(tlabel.find(":VIRT:"));
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			point = point.setVirtual();
		}

		i = static_cast<int>(tlabel.find(":ADDED:"));
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
		}

		// set low index of for no longer used source fields
		i = static_cast<int>(tlabel.find(":SRC:"));
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
		}
		i = tlabel.find(":STM:");
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
		}


		Velocity v = Velocity::ZEROV();
		i = static_cast<int>(tlabel.find(":VEL:")); // velocity in
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = static_cast<int>(tlabel.find(":",i+5));
			std::string t = tlabel.substr(i+5, j);
			std::replace(t.begin(), t.end(), '_', ' ');
			v = Velocity::parse(t);
			//point = point.setVelocityInit(v);
		}
		i = static_cast<int>(tlabel.find(":ATRK:")); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = static_cast<int>(tlabel.find(":",i+6));
			double a = Util::parse_double(tlabel.substr(i+6,j));
			//point = point.setTrkAccel(a);
			double r = 0.0;
			if ( a != 0.0) { // avoid divide by zero
				r = v.gs()/a;
			}
			point.setRadiusSigned(r);
		}
		i = static_cast<int>(tlabel.find(":AGS:")); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = static_cast<int>(tlabel.find(":",i+5));
			double a = Util::parse_double(tlabel.substr(i+5,j));
			point = point.setGsAccel(a);
		}
		i = static_cast<int>(tlabel.find(":AVS:")); // acceleration
		if (i >= 0) {
			lowIndex = std::min(i, lowIndex);
			j = static_cast<int>(tlabel.find(":",i+5));
			double a = Util::parse_double(tlabel.substr(i+5,j));
			point = point.setVsAccel(a);
		}
		for (int iter = 0; iter <=4; iter++) {
			TrkTcpType t = toTrkTCP(iter);
			i = static_cast<int>(tlabel.find(":"+toStringTrkType(t)+":"));
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				point.tcpTrk = t;
			}
		}
		for (int iter = 0; iter <=4; iter++) {
			GsTcpType t = toGsTCP(iter);
			i = static_cast<int>(tlabel.find(":"+toStringGsType(t)+":"));
			if (i >= 0) {
				lowIndex = std::min(i, lowIndex);
				point.tcpGs = t;
			}
		}
		for (int iter = 0; iter <=4; iter++) {
			VsTcpType t = toVsTCP(iter);
			i = static_cast<int>(tlabel.find(":"+toStringVsType(t)+":"));
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
		std::string s1 = "NONE";
		std::string s2 = "NONE";
		std::string s3 = "NONE";
		if (tcpTrk == TcpData::BOT) s1 = "BOT";
		if (tcpTrk == TcpData::EOT) s1 = "EOT";
		if (tcpTrk == TcpData::EOTBOT) s1 = "EOTBOT";
		if (tcpGs == TcpData::BGS) s2 = "BGS";
		if (tcpGs == TcpData::EGS) s2 = "EGS";
		if (tcpGs == TcpData::EGSBGS) s2 = "EGSBGS";
		if (tcpVs == TcpData::BVS) s3 = "BVS";
		if (tcpVs == TcpData::EVS) s3 = "EVS";
		if (tcpVs == TcpData::EVSBVS) s3 = "EVSBVS";
	    if(!TcpData::motFlagInInfo & isMOT()) s1 ="MOT";
		return s1+" "+s2+" "+s3;
	}

	std::string TcpData::metaDataLabel(int precision) const {
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
			std::pair<TcpData,std::string> p = TcpData().parseMetaDataLabel(extra);
			if ( p.second != "") {
				np.makeName(p.second);
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
			std::string lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
			Position turnCenter = KinematicsPosition::centerFromRadius(pos, radius, vv.trk());
			return std::pair<NavPoint, TcpData>(NavPoint(pos, time, lab), TcpData::makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc));
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
			std::pair<TcpData,std::string> p = TcpData().parseMetaDataLabel(extra);
			if ( p.second != "") {
				np.makeName(p.second);
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
			std::string lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
			Position turnCenter = KinematicsPosition::centerFromRadius(pos, radius, vv.trk());
			return std::pair<NavPoint, TcpData>(NavPoint(pos, time, lab), TcpData::makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc));
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
	TcpData TcpData::mergeTCPData(const TcpData& point) const {
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
	    Position center = Position::INVALID();
		if (tcpTrk == BOT || tcpTrk == EOTBOT) {
			my_radius = radiusSigned;
			center = turnCenter();
		} else {
			my_radius = point.radiusSigned;
			center = point.turnCenter();
		}
		d.setMOT(isMOT() || point.isMOT());
		d.setRadiusSigned(my_radius);
		d.setTurnCenter(center);
		double my_accel_gs = (tcpGs == BGS || tcpGs == EGSBGS) ? accelGs : point.accelGs;
		double my_accel_vs = (tcpVs == BVS || tcpVs == EVSBVS) ? accelVs : point.accelVs;

		d.setGsAccel(my_accel_gs);
		d.setVsAccel(my_accel_vs);
		d.setInformation(information+point.information);
		d.appendInformationNoDuplication(point.information);
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
		string rtn= "TcpData [ty=" + toStringWayType(ty) + " <" + tcpTypeString() + ">";
		if (radiusSigned != 0.0) rtn = rtn + " , radiusSigned="	+ Fm4(radiusSigned);
		if (!centerTurn.isInvalid()) rtn = rtn + " turnCenter ="+ centerTurn.toString();
		rtn = rtn + ", accelGs=" + Fm4(accelGs) + ", accelVs=" + Fm4(accelVs)
				+ ", information=" + information; // + " isMOT = "+bool2str(isMOT());
//		if (showSource) rtn = rtn + ", sourcePosition=" + sourcePosition_p.toString();// + ", sourceTime=" + Fm4(sourceTime_d)

		rtn = rtn +  + "]";
		return rtn;
	}
	
	std::string TcpData::toStringTcpType() const {
		std::string rtn = "";
		if (isBOT()) {
	    	if (isEOT()) rtn = rtn +"EOTBOT";
	    	else rtn = rtn +"BOT";
	    } else if (isEOT()) rtn = rtn +"EOT";
	    if (isBGS()) {
	    	if (isEGS()) rtn = rtn +"EGSBGS";
	    	else rtn = rtn +"BGS";
	    } else if (isEGS()) rtn = rtn +"EGS";
	    if (isBVS()) {
	    	if (isEVS()) rtn = rtn +"EVSBVS";
	    	else rtn = rtn +"BVS";
	    } else if (isEVS()) rtn = rtn +"EVS";
	    
	    if (!TcpData::motFlagInInfo & isMOT()) rtn = rtn +"MOT";
	    
        return rtn;
	}

}
