/*
 * PolycarpQuadMinmax - determining if a univariate quadratic passes below a threshold on an interval
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPQUADMINMAX_H_
#define POLYCARPQUADMINMAX_H_
namespace larcfm {
    class PolycarpQuadMinmax {
    public:
        static double quadratic(double a,double b,double c,double x);
        static double quad_min_int(double a,double b,double c,double xl,double xu);
        static bool quad_min_le_D_int(double ap,double b,double c,double xl,double xu,double D);
    };
}
#endif
