/*
 * Util.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov), Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Utility functions.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
#include "Units.h"
#include "Constants.h"
#include <cmath>
#include <limits>
#include <string.h>
#include "format.h"
#include "string_util.h"
#include <stdexcept>

//#include <dirent.h>



namespace larcfm {

using std::string;
using std::vector;

static const double twopi = 2*Pi;


bool Util::almost_less(double a, double b) {
	if (almost_equals(a, b)) {
		return false;
	}
	return a < b;
}

bool Util::almost_less(double a, double b, INT64FM maxUlps) {
	if (almost_equals(a, b, maxUlps)) {
		return false;
	}
	return a < b;
}

bool Util::almost_greater(double a, double b) {
	if (almost_equals(a, b)) {
		return false;
	}
	return a > b;
}

bool Util::almost_greater(double a, double b, INT64FM maxUlps) {
	if (almost_equals(a, b, maxUlps)) {
		return false;
	}
	return a > b;
}


bool Util::almost_geq(double a, double b, INT64FM maxUlps) {
	return a >= b || almost_equals(a, b, maxUlps);
}

bool Util::almost_geq(double a, double b) {
	return almost_geq(a, b, PRECISION_DEFAULT);
}

bool Util::almost_leq(double a, double b, INT64FM maxUlps) {
	return a <= b || almost_equals(a, b, maxUlps);
}

bool Util::almost_leq(double a, double b) {
	return almost_leq(a, b, PRECISION_DEFAULT);
}

// This assumes that a INT64FM is 64 bits, same as a double
bool Util::almost_equals(double a, double b, INT64FM maxUlps) {
	if (a == b) { // if two numbers are equal, then the are almost_equal
		return true;
	}

	// special case of comparing to zero.
	if (a == 0.0 || b == 0.0) {
		double comp = 1.0e-13;  // should correspond to PRECISION_DEFAULT;
		if (maxUlps == PRECISION5) comp = 1.0e-5;
		if (maxUlps == PRECISION7) comp = 1.0e-7;
		if (maxUlps == PRECISION9) comp = 1.0e-9;
		if (maxUlps == PRECISION13) comp = 1.0e-13;
		if (std::abs(a) < comp && std::abs(b) < comp) {
			return true;
		}
	}

	//if (ISNAN(a) || ISNAN(b)) { // this operation is sloooooow
	//	return false;
	//  }
	if (!(a < b || b < a)) { // idiom to filter out NaN's
		return false;
	}
	if (ISINF(a) || ISINF(b)) {
		return false;
	}

	//long aInt = Double.doubleToLongBits(a);
	INT64FM aInt;
	memcpy(&aInt,&a,sizeof(a)); // same as:  INT64FM aInt = *(INT64FM*)&a;

	// Make aInt lexicographically ordered as a twos-complement long
	if (aInt < 0)
		aInt = HIGH_BIT - aInt;

	// Make bInt lexicographically ordered as a twos-complement long
	//long bInt = Double.doubleToLongBits(b);
	INT64FM bInt;
	memcpy(&bInt,&b,sizeof(b)); // same as: INT64FM bInt = *(INT64FM*)&b;

	if (bInt < 0)
		bInt = HIGH_BIT - bInt;

	INT64FM intDiff = llabs(aInt - bInt); // This is valid because IEEE-754
	// doubles are required to be
	// lexically ordered

	if (intDiff <= maxUlps) {
		return true;
	}

	return false;
}


// This assumes that a INT64FM is 64 bits, same as a double
bool Util::almost_equals(double a, double b) {
	return almost_equals(a,b,PRECISION_DEFAULT);
}


bool Util::within_epsilon(double a, double b, double epsilon) {
	return std::abs(a-b) < epsilon;
}

bool Util::within_epsilon(double a, double epsilon) {
	return std::abs(a) < epsilon;
}

INT64FM Util::llabs(const INT64FM x) {
	return x >= 0 ? x : -x;
}



double  Util::discretizeDir(double voz, double nvoz, double discreteUnits) {
	int sgn = -1;
	if (nvoz >= 0) sgn = 1;
	//double rtn =  sgn*ceil(std::abs(nvoz/discreteUnits))*discreteUnits;
	double rtn;
	if (sgn*nvoz >= sgn*voz)
		rtn =  sgn*ceil(std::abs(nvoz/discreteUnits))*discreteUnits;
	else
		rtn =  sgn*floor(std::abs(nvoz/discreteUnits))*discreteUnits;
	//f.pln(" #### vsDiscretize: voz = "+Units.str("fpm",voz)+" nvoz = "+Units.str("fpm",nvoz)+" rtn = "+Units.str("fpm",rtn));
	return rtn;
}



double Util::sq(const double x) {
	return x*x;
}

double Util::sqrt_safe(const double x) {
	return sqrt(std::max(x,0.0));
}

double Util::atan2_safe(const double y, const double x) {
	if (y == 0 && x == 0)
		return 0;
	return atan2(y,x);
}

double Util::asin_safe(double x) {
	return asin(std::max(-1.0,std::min(x,1.0)));
}

double Util::acos_safe(double x) {
	return acos(std::max(-1.0,std::min(x,1.0)));
}

double Util::discr(const double a, const double b, const double c){
	return sq(b) - 4*a*c;
}

double Util::root(const double a, const double b, const double c, const int eps) {
	if (a == 0.0 && b == 0.0)
		return NaN;
	else if (a == 0.0)
		return -c/b;
	else {
		double sqb = sq(b);
		double ac  = 4*a*c;
		if (almost_equals(sqb,ac) || sqb > ac)
			return (-b + eps*sqrt_safe(sqb-ac))/(2*a);
		return NaN;
	}
}

double Util::root2b(const double a, const double b, const double c, const int eps) {
	if (a == 0.0 && b == 0.0)
		return NaN;
	else if (a == 0.0)
		return -c/(2*b);
	else {
		double sqb = sq(b);
		double ac  = a*c;
		if (almost_equals(sqb,ac) || sqb > ac)
			return (-b + eps*sqrt_safe(sqb-ac))/a;
		return NaN;
	}
}

int Util::sign(const double x) {
	if (x >= 0.0)
		return 1;
	return -1;
}

/**
 * Computes the modulo of val and mod. The returned value is in the range [0,mod)
 */
double Util::modulo(double val, double mod) {
	double n = floor(val / mod);
	double r = val - n * mod;
	return Util::almost_equals(r,mod) ? 0.0 : r;
}

// To range [0,2*pi)
double Util::to_2pi(double rad) {
	return modulo(rad,twopi);
}

// To range [0,360)
double Util::to_360(double deg) {
	return modulo(deg,360);
}

// To range (-pi,pi]
double Util::to_pi(double rad) {
	double r = to_2pi(rad);
	if (r > Pi)
		return r-twopi;
	else {
		return r;
	}
}

// to range [-pi/2, +pi/2]; continuous, thus to_pi2_cont(PI/2+eps) = PI/2-eps
double Util::to_pi2_cont(double rad) {
	double r = to_pi(rad);
	if (r < -Pi / 2) {
		return -Pi - r;
	} else if (r < Pi / 2) {
		return r;
	} else {
		return Pi - r;
	}
}


// To range (-180,180]
double Util::to_180(double deg) {
	double d = to_360(deg);
	if (d > 180.0)
		return d-360.0;
	else {
		return d;
	}
}


bool Util::less_or_equal(std::string s1, std::string s2) {
	if (s1.compare(s2) >= 0) return true;
	return false;
}


bool Util::clockwise(double alpha, double beta) {
	double a = to_2pi(alpha);
	double b = to_2pi(beta);
	if (std::abs(a-b) <= M_PI) {
		return b >= a;
	}
	return a > b;
}

int Util::turnDir(double initTrack, double goalTrack) {
	if (Util::clockwise(initTrack,goalTrack)) return 1;
	else return -1;
}


double Util::turnDelta(double alpha, double beta) {
	double a = to_2pi(alpha);
	double b = to_2pi(beta);
	double delta = std::abs(a-b);
	if (delta <= M_PI) {
		return delta;
	}
	return 2.0*M_PI - delta;
}

double Util::signedTurnDelta(double alpha, double beta) {
	return turnDir(alpha,beta)*turnDelta(alpha,beta);
}


double Util::turnDelta(double alpha, double beta, bool turnRight) {
	if (Constants::almost_equals_radian(alpha,beta)) return 0;                 // do not want 2 PI returned
	//if (std::abs(alpha-beta) < 0.00001) return 0;                              // do not want 2 PI returned
	bool clkWise = Util::clockwise(alpha,beta);
	double rtn = Util::turnDelta(alpha,beta);
	if (turnRight != clkWise)  { // go the long way around
		rtn = 2.0 * M_PI - rtn;
	}
	return rtn;
}


bool Util::is_double(const string& str) {
	std::istringstream stream;
	stream.str(str);
	double d;
	stream >> d;
	if (stream.fail()) {
		return false;
	}
	return true;
}

double Util::parse_double(const string& str) {
	std::istringstream stream;
	stream.str(str);
	double d;
	stream >> d;
	if (stream.fail()) {
		return 0.0;
	}
	return d;
}


/** @param degMinSec  Lat/Lon string of the form "46:55:00"  or "-111:57:00"
      @return   numbers of degrees decimal
 */
double Util::decimalDegrees(const std::string& degMinSec) {
	vector<std::string> dms = split(degMinSec,":");
	double degrees = parse_double(dms[0]);
	int sgn = Util::sign(degrees);
	double minutes = parse_double(dms[1]);
	double seconds = parse_double(dms[2]);
	return sgn*(std::abs(degrees) + minutes/60+ seconds/3600);
}


/** Reads in a clock string and converts it to seconds.
 * Accepts hh:mm:ss, mm:ss, and ss.
 */
double Util::parse_time(const string& s) {
	double tm = -1;
	try {
		string patternStr = "[:]";
		vector<std::string> fields2 = split(s, patternStr);
		if (fields2.size() >= 3) {
			tm = parse_double(fields2[2]) + 60 * parse_double(fields2[1]) + 3600 * parse_double(fields2[0]); // hrs:min:sec
		} else if (fields2.size() == 2) {
			tm = parse_double(fields2[1]) + 60 * parse_double(fields2[0]); // min:sec
		} else if (fields2.size() == 1){
			tm = parse_double(fields2[0]); //getColumn(_sec, head[TM_CLK]);
		}
		return tm;
	} catch (std::runtime_error e) {
		return -1.0;
	}
}


/**
 * @param t time in seconds
 * @return String of hours:mins:secs
 */
string Util::hoursMinutesSeconds(double t) {
	int hours = (int) t/3600;
	int rem = (int) t - hours*3600;
	int mins = rem / 60;
	rem = rem - mins*60;
	int secs = rem;
	return ""+Fm0(hours)+":"+Fm0(mins)+":"+Fm0(secs);
}

///**
// * The behavior of the x%y operator is different between Java and C++ if either x or y is negative.  Use this to always return a value between 0 and y.
// * @param x value
// * @param y range
// * @return x mod y, having the same sign as y (Java behavior)
// */
//int Util::mod(int x, int y) {
//	int r = std::abs(x) % std::abs(y);
//	return r*sign(y);
//}



//*******************************************
// deprecated functions:


INT64FM llabs(const INT64FM x) {
	return Util::llabs(x);
}

double sq(const double x) {
	return Util::sq(x);
}

double sqrt_safe(const double x) {
	return sqrt(std::max(x,0.0));
}

double atan2_safe(const double y, const double x) {
	return Util::atan2_safe(y,x);
}

double asin_safe(double x) {
	return Util::asin_safe(x);
}

double acos_safe(double x) {
	return Util::acos_safe(x);
}

double discr(const double a, const double b, const double c){
	return Util::discr(a,b,c);
}

double root(const double a, const double b, const double c, const int eps) {
	return Util::root(a,b,c,eps);
}

double root2b(const double a, const double b, const double c, const int eps) {
	return Util::root2b(a,b,c,eps);
}

int sign(const double x) {
	return Util::sign(x);
}


// To range [0,2*pi)
double to_2pi(double rad) {
	return Util::to_2pi(rad);
}


// To range [0,360)
double to_360(double deg) {
	return Util::to_360(deg);
}

// To range (-pi,pi]
double to_pi(double rad) {
	return Util::to_pi(rad);
}

// to range [-pi/2, +pi/2]; continuous, thus to_pi2_cont(PI/2+eps) = PI/2-eps
double to_pi2_cont(double rad) {
	return Util::to_pi2_cont(rad);
}


// To range (-180,180]
double to_180(double deg) {
	return Util::to_180(deg);
}

//#ifndef _MSC_VER
//// will (probably) not work in Visual C++
//// modified from code on net (http://www.linuxquestions.org/quecgstions/programming-9/c-list-files-in-directory-379323/)
//
//int filesInDir (string path, vector<string>& fv) {
//        DIR *directory;
//        struct dirent *dir_entry;
//        directory = opendir(path.c_str());
//        if(directory == NULL) {
//                return -1; // error
//        }
//        dir_entry = readdir(directory);
//        while (dir_entry != NULL) {
//                string s = string(dir_entry->d_name);
//                int x = s.find(".txt", 0); //doesn't work in the if test, for some reason
//                if (x >= 0) {
//                        fv.push_back(path+"/"+s);
//                }
//                dir_entry = readdir(directory);
//        }
//        closedir(directory);
//        return 1;  // ok
//}
//#endif
//
//bool fileExists(const char *filename) {
//        std::ifstream tmp(filename);
//        return tmp.good();
//}


}




