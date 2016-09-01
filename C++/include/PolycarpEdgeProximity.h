/*
 * PolycarpEdgeProximity - Determining if a point is near a line segment or if two line segments are near each other
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYCARPEDGEPROXIMITY_H_
#define POLYCARPEDGEPROXIMITY_H_

#include "Vect2.h"

namespace larcfm {
    class PolycarpEdgeProximity {
    public:
        static bool near_edge(const Vect2& segstart, const Vect2& segend, const Vect2& s, double BUFF);
        static bool segments_2D_close(const Vect2& segstart1, const Vect2& segend1, const Vect2& segstart2, const Vect2& segend2, double BUFF);
        static Vect2 closest_point(const Vect2& segstart, const Vect2& segend, const Vect2& s, double BUFFER);
    };
}
#endif


