/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef GENERALPLAN_H_
#define GENERALPLAN_H_

#include "Plan.h"
#include "PolyPath.h"
#include "GeneralState.h"
#include "ErrorReporter.h"
#include <string>


namespace larcfm {

/**
 * A "general state" object that holds Euclidean or Lat/Lon information about position and velocity for an object.
 * Currently this can either be a (point-mass) aircraft or a (possible morphing) polygon.
 * This is intended to be traffic information that will be converted to a General3DState 
 * object to be sent to a GeneralDetector object.  
 */
class GeneralPlan : public ErrorReporter {
private:
	Plan fp;
	PolyPath pp;
	bool plan;
	bool poly;
	bool containment;

public:
	static const GeneralPlan INVALID;

	GeneralPlan();

	GeneralPlan(const Plan& p);

	GeneralPlan(const PolyPath& p);


	/**
	 * Allows for containment polygons
	 * @param p
	 * @param cont true = containment poly, false = avoidance poly
	 * @param time
	 */
	GeneralPlan(const PolyPath& p, bool cont);

	GeneralPlan(const GeneralPlan& g);

	GeneralState state(double time) ;

	double getFirstTime() const;

	double getLastTime() const;

	int size() const;

	std::string getID() const;

	bool isLatLon() const;

	bool hasPlan() const;

	bool hasPolyPath() const;

	Plan getPlan() const;

	PolyPath getPolyPath() const;

	bool isContainment() const;

	bool isInvalid() const;

	GeneralState point(int i) ;

	int getSegment(double t) const;

	bool validate() const;

	std::string toString() ;

	//std::string toOutput(int precision) ;


	// ErrorReporter Interface Methods

	bool hasError() const {
		if (plan) {
			return fp.hasError();
		} else if (poly) {
			return pp.hasError();
		}
		return false;
	}
	bool hasMessage() const {
		if (plan) {
			return fp.hasMessage();
		} else if (poly) {
			return pp.hasMessage();
		}
		return false;
	}
	std::string getMessage() {
		if (plan) {
			return fp.getMessage();
		} else if (poly) {
			return pp.getMessage();
		}
		return "";
	}
	std::string getMessageNoClear() const {
		if (plan) {
			return fp.getMessageNoClear();
		} else if (poly) {
			return pp.getMessageNoClear();
		}
		return "";
	}


};


}// namespace

#endif
