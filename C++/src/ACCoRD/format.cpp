/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "format.h"
#include "Vect2.h"
#include "Vect3.h"
//#include "Vect4.h"
#include "Util.h"
#include "Units.h"
//#include "UnitSymbols.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <sstream>

namespace larcfm {

using std::string;
using std::endl;
using std::cout;
using std::cerr;
using std::ostream;

string Fmi(int v) {
	std::ostringstream s;
	s << std::fixed << std::noshowpoint << v;
	return s.str();
}

string Fmui(unsigned int v) {
	std::ostringstream s;
	s << std::fixed << std::noshowpoint << v;
	return s.str();
}


string Fmul(unsigned long v) {
	std::ostringstream s;
	s << std::fixed << std::noshowpoint << v;
	return s.str();
}

// remove negative zeros
inline double fm_nz(double v, int precision) {
	if (v < 0.0 && ceil(v*pow(10.0,precision)-0.5) == 0.0)
		return 0.0;
	return v+0.0; // This removes actual negative zeros
}

string Fm2z(double v) {
	std::ostringstream s;
	if (v < 0) {
		s << "-";
		v *= -1.0;
	} else {
		s << "+";
	}
	s.width(2);
	s.precision(0);
	s.fill('0');
	s << std::fixed;
	s << v;
	return s.str();
}

string Fm3z(double v) {
	std::ostringstream s;
	s.width(3);
	s.precision(0);
	s.fill('0');
	s << std::fixed << v;
	return s.str();
}

string Fm0(double v) {
	return FmPrecision(v,0);
}

string Fm1(double v) {
	return FmPrecision(v,1);
}

string Fm2(double v) {
	return FmPrecision(v,2);
}

string Fm3(double v) {
	return FmPrecision(v,3);
}

string Fm4(double v) {
	return FmPrecision(v,4);
}

string Fm6(double v) {
	return FmPrecision(v,6);
}

string Fm8(double v) {
	return FmPrecision(v,8);
}

string Fm12(double v) {
	return FmPrecision(v,12);
}

string Fm16(double v) {
	return FmPrecision(v,16);
}

string FmPrecision(double v) {
	return FmPrecision(v, Constants::get_output_precision());
}

string FmPrecision(double v, int precision) {
	if (ISNAN(v)) {
		return "NaN";
	}
	if (ISINF(v)) {
		string s = "infty";
		if (v < 0) {
			return "-"+s;
		}
		return s;
	}
	std::ostringstream s;
	s.precision(precision);
	s << std::fixed;
	if (precision == 0) {
		s << std::noshowpoint;
	} else {
		s << std::showpoint;
	}
	s << fm_nz(v,precision+1);
	return s.str();
}

string FmLead(int i, int precision) {
	std::ostringstream oss;
	oss.fill('0');
	oss.width(precision);
	oss << std::fixed << std::noshowpoint << i;
	return oss.str();
}

string Fmb(bool b) {
	if (b) return "true";
	return "false";
}

string bool2str(bool b) {
	if (b) return "true";
	else return "false";
}

string Fobj(const std::vector<int>& v) {
	string s = "";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += Fm0(v[i])+" ";
	}
	return s;
}

string Fobj(const std::vector<double>& v) {
	string s = "";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += Fm4(v[i])+" ";
	}
	return s;
}

string Fobj(const std::vector<std::string>& v) {
	string s = "";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += v[i]+" ";
	}
	return s;
}

std::string list2str(const std::vector<std::string>& l, const std::string& delimiter) {
	string s = "";
	if (l.size() > 0) {
		s+=l[0];
		for (unsigned int i = 1; i < l.size(); i++) {
			s += delimiter + l[i];
		}
	}
	return s;
}

/**
 * Print int array
 */
string Farray(int const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; i++) {
		s += Fm0(v[i])+" ";
	}
	return s;
}

/**
 * Print double array
 */
string Farray(double const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; i++) {
		s += Fm4(v[i])+" ";
	}
	return s;
}

/**
 * Print string array
 */
string Farray(std::string const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; i++) {
		s += v[i]+" ";
	}
	return s;
}


string FmVec(Vect2 v) {
	return "("+Fm16(v.x)+","+Fm16(v.y)+")";
}

string FmVec(Vect3 v) {
	return "("+Fm16(v.x)+","+Fm16(v.y)+","+Fm16(v.z)+")";
}

string padLeft(string s, int n) {
	//return string.format("%1$#" + n + "s", s);
	string ss = s;
	ss.resize(n,' ');
	return ss;
}


string padRight(string s, int n) {
	//return string.format("%1$-" + n + "s", s);
	while ((int)s.length() < n) {
		s = s + " ";
	}
	return s;
}


void fpln(const string& str) {
	cout << str << endl;
}

void fp(const string& str) {
	cout << str;
}

void fpln(ostream* os, const string& str) {
	os->write(str.c_str(),str.size());
	os->write("\n",1);
}

void fp(ostream* os, const string& str) {
	os->write(str.c_str(),str.size());
}

void fdln(const string& str) {
	cerr << "$$ERROR$$ " << str << endl;
}

string fsStr(const Vect2& s) {
	return "(" + Units::str("nmi", s.x) + " ," + Units::str("nmi", s.y) + ")";
}

string fsStr(const Vect3& s) {
	return "(" + Units::str("nmi", s.x) + " ," + Units::str("nmi", s.y) + " ,"+ Units::str("ft", s.z) + ")";
}

string fsStr8NP(const Vect3& s) {
	return Fm8(Units::to("NM", s.x)) + " " + Fm8(Units::to("NM", s.y)) + " " 	+ Fm8(Units::to("ft", s.z));
}

string fsStr15NP(const Vect3& s) {
	return Fm16(Units::to("NM", s.x)) + " " + Fm16(Units::to("NM", s.y)) + " " 	+ Fm16(Units::to("ft", s.z));
}

string fvStr(const Vect2& s) {
	return "(" + Units::str("knot", s.x) + " ," + Units::str("knot", s.y) + ")";
}

string fvStr(const Vect3& s) {
	return "(" + Units::str("knot", s.x) + " ," + Units::str("knot", s.y) + " ,"  + Units::str("fpm", s.z) + ")";
}

string fvStr2(const Vect2& v) {
	return "("+Units::str("deg",v.compassAngle())+", "+Units::str("knot",v.norm())+")";
}

string fvStr2(const Vect3& v) {
	return "("+Units::str("deg",v.vect2().compassAngle())+", "+Units::str("knot",v.norm())+", "+Units::str("fpm",v.z)+")";
}

string double2PVS(double val, int prec) {
	return FmPrecision(val,prec)+"::ereal";
}


}
