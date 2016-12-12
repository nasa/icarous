/*
 * Constants.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * General Constants
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Constants.h"
//#include "UnitSymbols.h"

namespace larcfm {

const std::string Constants::version = larcfm::_FormalATM_version;							// eventually move definition here
const std::string Constants::build_date = __DATE__;
const std::string Constants::build_time = __TIME__;

const double Constants::GPS_LIMIT_HORIZONTAL = larcfm::_FormalATM_GPS_LIMIT_HORIZONTAL; 	// eventually Units::from(_ft, 25.0);
const double Constants::GPS_LIMIT_VERTICAL = larcfm::_FormalATM_GPS_LIMIT_VERTICAL; 		// eventually Units::from(_ft, 25.0);
const double Constants::TIME_LIMIT_EPSILON = larcfm::_FormalATM_TIME_LIMIT_EPSILON; 		// eventually Units::from(_s, 1.0);
const double Constants::NO_TIME_LIMIT_EPSILON = _FormalATM_NO_TIME_LIMIT_EPSILON;
const double Constants::NO_TIME = _FormalATM_NO_TIME;

double Constants::HORIZONTAL_ACCURACY = 1E-7;  // Constants::GPS_LIMIT_HORIZONTAL;
double Constants::VERTICAL_ACCURACY   = 1E-7; //Constants::GPS_LIMIT_VERTICAL;
double Constants::HORIZONTAL_ACCURACY_RAD = Units::to("NM", Constants::HORIZONTAL_ACCURACY) * M_PI / (180.0 * 60.0);
double Constants::TIME_ACCURACY       = 1E-7; // Constants::TIME_LIMIT_EPSILON;

int Constants::OUTPUT_PRECISION = 6;

const std::string Constants::separatorPattern = ",;";
const std::string Constants::wsPatternBaseNoRegex = " \t,;";
const std::string Constants::wsPatternBase = "[\t ,;]+"; //"[\\t ,;]+";
const std::string Constants::wsPatternParens = "() \t,;";
const std::string Constants::wsPatternBraces = "[] \t,;";


void Constants::set_time_accuracy(double acc) {
  if (acc > 0.0) {
    TIME_ACCURACY = acc;
  }
}

void Constants::set_horizontal_accuracy(double acc) {
  if (acc > 0.0) {
    HORIZONTAL_ACCURACY = acc;
    HORIZONTAL_ACCURACY_RAD = Units::to("NM", acc) * M_PI / (180.0 * 60.0);
    //HORIZONTAL_ACCURACY_RAD = GreatCircle.angle_from_distance(acc);
  }
}

void Constants::set_vertical_accuracy(double acc) {
  if ( acc > 0.0) {
    VERTICAL_ACCURACY = acc;
  }
}

double Constants::get_horizontal_accuracy() {
  return HORIZONTAL_ACCURACY;
}

double Constants::get_vertical_accuracy() {
  return VERTICAL_ACCURACY;
}

double Constants::get_latlon_accuracy() {
  return HORIZONTAL_ACCURACY_RAD;
}

double Constants::get_time_accuracy() {
  return TIME_ACCURACY;
}

int Constants::get_output_precision() {
	return OUTPUT_PRECISION;
}

void Constants::set_output_precision(int i) {
	if (i >= 0 && i <= 16) {
		OUTPUT_PRECISION = i;
	}
}

bool Constants::almost_equals_time(double t1, double t2) {
  return Util::within_epsilon(t1,t2,TIME_ACCURACY);
}

bool Constants::almost_equals_xy(double x1, double y1, double x2, double y2) {
  return Util::within_epsilon(sq(x1-x2)+sq(y1-y2),sq(HORIZONTAL_ACCURACY));
}

bool Constants::almost_equals_distance(double d) {
  return Util::within_epsilon(d, HORIZONTAL_ACCURACY);
}

bool Constants::almost_equals_radian(double d1, double d2) {
  return Util::within_epsilon(d1, d2, HORIZONTAL_ACCURACY_RAD);
}

bool Constants::almost_equals_radian(double d) {
  return Util::within_epsilon(d, HORIZONTAL_ACCURACY_RAD);
}

bool Constants::almost_equals_alt(double a1, double a2) {
  return Util::within_epsilon(a1, a2, VERTICAL_ACCURACY);
}

	//*********************************************
	// deprecated functions:

void set_time_accuracy(double acc) {
	Constants::set_time_accuracy(acc);
}

void set_horizontal_accuracy(double acc) {
	Constants::set_horizontal_accuracy(acc);
}

void set_vertical_accuracy(double acc) {
	Constants::set_vertical_accuracy(acc);
}

double get_horizontal_accuracy() {
	return Constants::get_horizontal_accuracy();
}

double get_vertical_accuracy() {
	return Constants::get_vertical_accuracy();
}

double get_time_accuracy() {
	return Constants::get_time_accuracy();
}
	
bool almost_equals_time(double t1, double t2) {
	  return Constants::almost_equals_time(t1,t2);
}

bool almost_equals_xy(double x1, double y1, double x2, double y2) {
	  return Constants::almost_equals_xy(x1,y1,x2,y2);
}

bool almost_equals_distance(double d) {
	 return Constants::almost_equals_distance(d);
}

bool almost_equals_radian(double d1, double d2) {
	  return Constants::almost_equals_radian(d1,d2);
}

bool almost_equals_radian(double d) {
	  return Constants::almost_equals_radian(d);
}
	
bool almost_equals_alt(double a1, double a2) {
	  return Constants::almost_equals_alt(a1,a2);
}
}

