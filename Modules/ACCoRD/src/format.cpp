/*
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "format.h"
#include "Vect2.h"
#include "Vect3.h"
//#include "Vect4.h"
#include "Util.h"
#include "Triple.h"
#include "Units.h"
//#include "UnitSymbols.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

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
  return FmPrecision(v, precision, Constants::get_trailing_zeros());
}

string FmPrecision(double v, int precision, bool includeTrailingZeros) {
	if (ISNAN(v)) {
		return "NaN";
	}
	if (ISINF(v)) {
		string str = "infty";
		if (v < 0) {
			return "-"+str;
		}
		return str;
	}
	std::ostringstream ss;
	ss.precision(precision);
	ss << std::fixed;
	if (precision == 0) {
	  ss << std::noshowpoint;
	} else {
	  ss << std::showpoint;
	}
	ss << fm_nz(v,precision+1);
	std::string str = ss.str();
	if (precision > 0 && !includeTrailingZeros) {
	  std::size_t first=str.find('.');
	  if (first!=std::string::npos) {
	    std::size_t last = str.find_last_not_of('0');
	    if (first == last) {
	      // number has the form xxx.00000
	      str.erase(last+2);
	    } else {
	      str.erase(last+1);
	    }
	  }
	}
	return str;
}

string FmLead(int i, int minLength) {
	std::ostringstream oss;
	oss.fill('0');
	oss.width(minLength);
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

string FmPair(const std::pair<int,int>& v) {
	return "("+Fm0(v.first)+","+Fm0(v.second)+")";
}

string FmPair(const std::pair<double,double>& v) {
	return "("+Fm6(v.first)+","+Fm6(v.second)+")";
}

string FmTriple(const Triple<int,int,int>& v) {
	return "("+Fm0(v.first)+","+Fm0(v.second)+","+Fm0(v.third)+")";
}

string FmTriple(const Triple<double,double,double>& v) {
	return "("+Fm6(v.first)+","+Fm6(v.second)+","+Fm6(v.third)+")";
}

string Fobj(const std::vector<int>& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += Fm0(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::vector<double>& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += Fm4(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::vector<std::string>& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += v[i];
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}


string Fobj(const std::vector<std::pair<int,int> >& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += FmPair(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::vector<std::pair<double,double> >& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += FmPair(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::vector<Triple<int,int,int> >& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += FmTriple(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::vector<Triple<double,double,double> >& v) {
	string s = "[";
	for (unsigned int i = 0; i < v.size(); i++) {
		s += FmTriple(v[i]);
		if (i < v.size()-1) s += ", ";
	}
	return s+"]";
}

string Fobj(const std::map<std::string,std::string>& v) {
	string s = "[";
	for (std::map<std::string,std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += it->first +"->"+it->second+", ";
	}
	return s+"]";
}

string Fobj(const std::map<int,int>& v) {
	string s = "[";
	for (std::map<int,int>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += Fm0(it->first) +"->"+Fm0(it->second)+", ";
	}
	return s+"]";
}

string Fobj(const std::map<int,std::string>& v) {
	string s = "[";
	for (std::map<int,std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += Fm0(it->first) +"->"+it->second+", ";
	}
	return s+"]";
}


string Fobj(const std::map<std::string,int>& v) {
	string s = "[";
	for (std::map<std::string,int>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += it->first +"->"+Fm0(it->second)+", ";
	}
	return s+"]";
}

string Fobj(const std::map<double,double>& v) {
	string s = "[";
	for (std::map<double,double>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += Fm4(it->first) +"->"+Fm4(it->second)+", ";
	}
	return s+"]";
}

string Fobj(const std::map<double,std::string>& v) {
	string s = "[";
	for (std::map<double,std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += Fm4(it->first) +"->"+it->second+", ";
	}
	return s+"]";
}


string Fobj(const std::map<std::string,double>& v) {
	string s = "[";
	for (std::map<std::string,double>::const_iterator it = v.begin(); it != v.end(); ++it) {
		s += it->first +"->"+Fm4(it->second)+", ";
	}
	return s+"]";
}


std::string list2str(const std::vector<std::string>& l, const std::string& delimiter) {
	string s = "";
	if (l.size() > 0) {
		s+=l[0];
		for (unsigned int i = 1; i < l.size(); ++i) {
			s += delimiter + l[i];
		}
	}
	//fpln(" $$$$ format.list2str: s = "+s);
	return s;
}

/**
 * Print int array
 */
string Farray(int const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; ++i) {
		s += Fm0(v[i]);
		if (i < sz-1) s += ", ";
	}
	return s;
}

/**
 * Print double array
 */
string Farray(double const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; i++) {
		s += Fm4(v[i]);
		if (i < sz-1) s += ", ";
	}
	return s;
}

/**
 * Print string array
 */
string Farray(std::string const v[], int sz) {
	string s = "";
	for (int i = 0; i < sz; i++) {
		s += v[i];
		if (i < sz-1) s += ", ";
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
	string pad = "";
	for (int i = 0; i < n-(int)s.size(); i++) {
	  pad = pad + " ";
	}

	return pad+s;
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
	cout.flush();
}

void fp(const string& str) {
	cout << str;
}

void fpln(ostream* os, const string& str) {
	std::string newline = "\n";
	os->write(str.c_str(),str.size());
	os->write(newline.c_str(),newline.size());
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

string double2PVS(double val) {
	return double2PVS(val,Constants::get_output_precision());
}

string double2PVS(double val, int precision) {
	return FmPrecision(val,precision)+"::ereal";
}


}
