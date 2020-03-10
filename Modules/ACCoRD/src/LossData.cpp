/*
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

/*
 * LossData.cpp
 * [CAM] (time_in,time_out) is the time interval of loss of separation. Every point in the open interval represents
 * a time where the aircraft are in violation. Whether or not the bounds of the interval are violation points or not
 * depends on the detector, e.g., for CD3D the points time_in and time_out are not necessarily violations points,
 * for WCV_tvar the points time_in and time_out violation times. Furthermore, time_in and time_out are always
 * between the lookahead time interval [B,T], where 0 <= B < T. It is always the case that if time_in < time_out
 * is true, then there is a conflict.
 */

#include "LossData.h"
#include "format.h"

namespace larcfm {

LossData::LossData(double tin, double tout) : time_in(tin), time_out(tout) {
}

LossData::LossData() : time_in(PINFINITY), time_out(NINFINITY) {
}

const LossData& EMPTY() {
	static LossData tmp;
	return tmp;
}

/**
 * Returns true if loss
 */
bool LossData::conflict() const {
	return time_in < time_out && !Util::almost_equals(time_in,time_out); //[CAM] Added to avoid numerical instability
}

/**
 * Returns true if loss last more than thr in seconds
 */
bool LossData::conflict(double thr) const {
	return conflict() && (time_out - time_in >= thr);
}

/**
 * Returns time to first loss in seconds.
 */
double LossData::getTimeIn() const {
	return conflict() ? time_in : PINFINITY;
}

/**
 * Returns time to last loss in seconds.
 */
double LossData::getTimeOut() const {
	return conflict() ? time_out : NINFINITY;
}

/**
 * Returns time interval to loss in seconds
 */
Interval LossData::getTimeInterval() const {
	return Interval(time_in,time_out);
}

std::string LossData::toString() const {
	std::string str = "[time_in: " + Fm2(time_in) + ", time_out:" + Fm2(time_out) +"]";
	return str;
}

} /* namespace larcfm */
