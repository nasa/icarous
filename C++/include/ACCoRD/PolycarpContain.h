/*
 * PolycarpContain - containment for 2D polygons
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPCONTAIN_H_
#define POLYCARPCONTAIN_H_

#include "Vect2.h"
#include <vector>

namespace larcfm {
    class PolycarpContain {
    public:
        class NumEdgesCross {
        public:
            int num;
            bool invalid;
            
            NumEdgesCross(int n, bool i) {
                num = n;
                invalid = i;
            }
        };
        
        static bool near_any_edge(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static NumEdgesCross number_upshot_crosses(const std::vector<Vect2>& p,const Vect2& s);
        static int quadrant(const Vect2& s);
        static int winding_number(const std::vector<Vect2>& p,const Vect2& s);
        static std::vector<Vect2> fix_polygon(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static bool definitely_inside_prelim(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static bool definitely_inside(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static bool definitely_outside_prelim(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static bool definitely_outside(const std::vector<Vect2>& p,const Vect2& s,double BUFF);
        static bool nice_polygon_2D(const std::vector<Vect2>& p,double BUFF);
    };
}
#endif
