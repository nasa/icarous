/*
 * PolycarpDoubleQuadratic - Determining if a quadratic in two variables ever passes below a threshold
 *                         - for inputs in the unit box
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPDOUBLEQUADRATIC_H_
#define POLYCARPDOUBLEQUADRATIC_H_
namespace larcfm {
    /**
 * PolycarpDoubleQuadratic - Determining if a quadratic in two variables ever passes below a threshold
 * 						   - for inputs in the unit box
 */
class PolycarpDoubleQuadratic {
    public:
        static const double AE;
        static bool ae(double a, double b);
//        static double sq(double a);
//        static double discr(double a, double b, double c);
//        static double root(double a, double b, double c, int eps);
        static double quad(double a,double b,double c,double d,double ee,double f,double x,double y);
        static double rev_disc(double a, double b,double c);
        static double quad_min_y_to_x(double a,double c,double d,double y);
        static double quad_min_y(double a,double b,double c,double d,double ee);
        static double quad_min_x(double a,double b,double c,double d,double ee);
        static bool quad_min_unit_box_init(double a,double b,double c,double d,double ee, double f,double D);
        static bool quad_min_unit_box(double a,double b,double c,double d,double ee,double f,double D);
    };
}
#endif
