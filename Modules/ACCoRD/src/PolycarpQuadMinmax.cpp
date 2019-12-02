/*
 * PolycarpQuadMinmax - determining if a univariate quadratic passes below a threshold on an interval
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "PolycarpQuadMinmax.h"

namespace larcfm {

	double PolycarpQuadMinmax::quadratic(double a,double b,double c,double x) {
	    return a*x*x+b*x+c;
	}

	double PolycarpQuadMinmax::quad_min_int(double a,double b,double c,double xl,double xu) {
	    if ((a<=0 && quadratic(a,b,c,xl)>=quadratic(a,b,c,xu))) return xu;
	    else if (a<=0) return xl;
	    else if (2*a*xl<=-b && -b<=2*a*xu) return -b/(2*a);
	    else if (-b<2*a*xl) return xl;
	    else return xu;
	}

	bool PolycarpQuadMinmax::quad_min_le_D_int(double ap,double b,double c,double xl,double xu,double D) {
	    if (xl>xu) return false;
	    else if (2*ap*xl<=-b && -b<=2*ap*xu) return (b*b-4*ap*(c-D)>0);
	    else if (quadratic(ap,b,c-D,xl)<0 || quadratic(ap,b,c-D,xu)<0) return true;
	    else return false;
	}
	
}
