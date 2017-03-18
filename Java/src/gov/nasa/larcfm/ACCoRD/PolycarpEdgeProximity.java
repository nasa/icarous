/* 
 * PolycarpEdgeProximity - Determining if a point is near a line segment or if two line segments are near each other
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 * 
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.ae;
import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.quad_min_unit_box;
import static gov.nasa.larcfm.ACCoRD.PolycarpQuadMinmax.quad_min_le_D_int;

public class PolycarpEdgeProximity {

	public static boolean near_edge(Vect2 segstart, Vect2 segend, Vect2 s, double BUFF) {
	    if (Math.abs(s.x-segstart.x)>2*BUFF && Math.abs(s.x-segend.x)>2*BUFF && Util.sign(s.x-segend.x)==Util.sign(s.x-segstart.x)) {
	    	return false;
	    		}
	    else if (Math.abs(s.y-segstart.y)>2*BUFF && Math.abs(s.y-segend.y)>2*BUFF && Util.sign(s.y-segend.y)==Util.sign(s.y-segstart.y)) {
	    	return false;
		}
		double ap = (segend.Sub(segstart)).sqv();
		double b = 2*((segstart.Sub(s)).dot(segend.Sub(segstart)));
		double c = (segstart.Sub(s)).sqv();
		if (segstart.Sub(s).sqv()<Util.sq(BUFF) || (segend.Sub(s).sqv()<Util.sq(BUFF))) {
			return true;
		}
		if (ap>0 && quad_min_le_D_int(ap,b,c,0,1,Util.sq(BUFF))){
			return true;
		}
		return false;
	}

	public static boolean segments_2D_close(Vect2 segstart1,Vect2 segend1,Vect2 segstart2,Vect2 segend2,double BUFF) {
		double segStartXDiff    = segstart1.x - segstart2.x;
		double segStartEndXDiff = segstart1.x - segend2.x;
		double segStartYDiff    = segstart1.y - segstart2.y;
		double segStartEndYDiff = segstart1.y - segend2.y;
		double segEndXDiff      = segend1.x - segend2.x;
		double segEndStartXDiff = segend1.x - segstart2.x;
		double segEndYDiff      = segend1.y - segend2.y;
		double segEndStartYDiff = segend1.y - segstart2.y;
		boolean segXApart  = (Math.abs(segStartXDiff)> 2*BUFF && Math.abs(segStartEndXDiff) > 2*BUFF &&
			      Math.abs(segEndXDiff)> 2*BUFF && Math.abs(segEndStartXDiff) > 2*BUFF &&
			      Util.sign(segStartXDiff) == Util.sign(segStartEndXDiff) &&
			      Util.sign(segEndXDiff) == Util.sign(segEndStartXDiff) && Util.sign(segStartXDiff) == Util.sign(segEndXDiff));
		boolean segYApart  = (Math.abs(segStartYDiff) > 2*BUFF && Math.abs(segStartEndYDiff) > 2*BUFF &&
			      Math.abs(segEndYDiff) > 2*BUFF && Math.abs(segEndStartYDiff) > 2*BUFF &&
			          Util.sign(segStartYDiff) == Util.sign(segStartEndYDiff) && Util.sign(segEndYDiff) == Util.sign(segEndStartYDiff) &&
			          Util.sign(segStartYDiff) == Util.sign(segEndYDiff));
		if (segXApart || segYApart) return false;
		else if (near_edge(segstart2,segend2,segstart1,BUFF)) return true; 
		else if (near_edge(segstart2,segend2,segend1,BUFF)) return true;
		else if (near_edge(segstart1,segend1,segstart2,BUFF)) return true;
		else if (near_edge(segstart1,segend1,segend2,BUFF)) return true;
		else if (ae((segend1.Sub(segstart1)).sqv(),0) || ae((segend2.Sub(segstart2)).sqv(),0)) return false;
		Vect2 s=segstart1.Sub(segstart2);
		Vect2 v=segend1.Sub(segstart1);
		Vect2 w=segend2.Sub(segstart2);
		double a=v.sqv();
		double b=w.sqv();
		double c=-2*(v.dot(w));
		double d=2*(s.dot(v));
		double ee=-2*(s.dot(w));
		double f=s.sqv();
		return quad_min_unit_box(a,b,c,d,ee,f,Util.sq(BUFF));
	}

	public static Vect2 closest_point(Vect2 segstart, Vect2 segend, Vect2 s, double BUFFER) {
		if (segend.Sub(segstart).norm() <= BUFFER) return segstart;
		if (s.Sub(segstart).dot(segend.Sub(segstart)) <= 0) return segstart;
		if (s.Sub(segend).dot(segstart.Sub(segend)) <= 0) return segend;
		Vect2 normdir = segend.Sub(segstart).Scal(1/segend.Sub(segstart).norm());
		double tt = segstart.Sub(s).det(normdir);
		return s.Add(normdir.PerpR().Scal(tt));
	}
}
