/*
 * PolycarpAcceptablePolygon - determining if a 2D polygon is well-formed
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPACCEPTABLEPOLYGON_H_
#define POLYCARPACCEPTABLEPOLYGON_H_

#include <vector>
#include "Vect2.h"

namespace larcfm {
    class PolycarpAcceptablePolygon {
    public:
        
        class EdgeCross {
        public:
            bool ans;
            bool invalid;
            
            EdgeCross(bool b1, bool b2) {
                ans = b1;
                invalid = b2;
            }
        };
        
        class CrossAns {
        public:
            int index;
            double num;
            double denom;
            
            CrossAns(int a, double b, double c) {
                index = a;
                num = b;
                denom = c;
            }
        };
        
        static bool near_poly_edge(const std::vector<Vect2>& p,const Vect2& s,double BUFF,int i);
        static EdgeCross upshot_crosses_edge(const std::vector<Vect2>& p,const Vect2& s,int i);
        static CrossAns compute_intercept(const std::vector<Vect2>& p,const Vect2& s,int i);
        static CrossAns min_cross_dist_index(const std::vector<Vect2>& p,const Vect2& s);
        static bool corner_lt_3deg(const Vect2& v,const Vect2& w);
        static bool acceptable_polygon_2D(const std::vector<Vect2>& p,double BUFF);
        static int counterclockwise_corner_index(const std::vector<Vect2>& p,int eps);
        static double min_y_val(const std::vector<Vect2>& p);
        static Vect2 test_point_below(const std::vector<Vect2>& p,double BUFF);
        static bool counterclockwise_edges(const std::vector<Vect2>& p);
        static bool segment_near_any_edge(const std::vector<Vect2>& p,double BUFF,const Vect2& segstart, const Vect2& segend);
    };
}
#endif

