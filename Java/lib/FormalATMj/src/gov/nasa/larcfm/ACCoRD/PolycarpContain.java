/* 
 * PolycarpContain - containment for 2D polygons
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 * 
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;

import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.EdgeCross;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.CrossAns;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.near_poly_edge;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.upshot_crosses_edge;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.min_cross_dist_index;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.acceptable_polygon_2D;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.counterclockwise_edges;
import static gov.nasa.larcfm.ACCoRD.PolycarpAcceptablePolygon.test_point_below;

public class PolycarpContain {

	public static class NumEdgesCross {
		int num = 0;
		boolean invalid = true;

		public NumEdgesCross(int n, boolean i) {
			num = n;
			invalid = i;
		}
	}

	public static boolean near_any_edge(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		for (int i = 0; i < p.size(); i++) {
			if (near_poly_edge(p,s,BUFF,i)) {
				return true;
			}
		}
		return false;
	}


	public static NumEdgesCross number_upshot_crosses(ArrayList<Vect2> p,Vect2 s) {
		int num = 0;
		boolean invalid = false;
		for (int i = 0; i < p.size(); i++) {
			EdgeCross uce = upshot_crosses_edge(p,s,i);
			if (uce.ans) {
				num=num+1;
			}
			if (uce.invalid) {
				invalid = true;
			}
		}
		return new NumEdgesCross(num,invalid);
	}

	public static int quadrant(Vect2 s) {
		if (s.x>=0 && s.y>=0) return 1;
		if (s.x<=0 && s.y>=0) return 2;
		if (s.x<=0) return 3;
		return 4;
	}

	public static int winding_number(ArrayList<Vect2> p,Vect2 s) {
		int total = 0;
		int q = quadrant(p.get(0).Sub(s));
		for (int i = 0; i < p.size(); i++) {
			int k = q;
			Vect2 thisv = p.get(i).Sub(s);
			Vect2 nextv = i<p.size()-1 ? p.get(i+1).Sub(s) : p.get(0).Sub(s);
			q = quadrant(nextv);
			if (k==q) {
				continue;
			} else if (q-1==(k % 4)) {
				total = total+1;
			} else if (k-1==(q % 4)) {
				total = total-1;
			} else if ((nextv.Sub(thisv)).det(thisv)<=0) {
				total=total+2;
			} else {
				total=total-2;
			}
		}
		return (int)Math.floor(total/4.0+0.5);
	}

	public static ArrayList<Vect2> fix_polygon(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		ArrayList<Vect2> newp = (ArrayList<Vect2>)p.clone();
		for (int i = 0; i < p.size(); i++) {
			if (p.get(i).y>=s.y-BUFF && Math.abs(p.get(i).x-s.x)<BUFF) {
				newp.set(i, p.get(i).Sub(new Vect2(2*BUFF,0)));
			}
		}
		return newp;
	}

	public static boolean definitely_inside_prelim(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		CrossAns mcdi = min_cross_dist_index(p,s);
		NumEdgesCross nuc = number_upshot_crosses(p,s);
		if (mcdi.index<0) {
			return false;
		}
		if (nuc.invalid) {
			return false;
		}
		if (p.get(mcdi.index).x<s.x) {
			return false;
		}
		int next_ind = mcdi.index<p.size()-1 ? mcdi.index+1 : 0;
		if (p.get(next_ind).x>s.x) {
			return false;
		}
		if ((nuc.num % 2)==0) {
			return false;
		}
		if (near_any_edge(p,s,BUFF)) {
			return false;
		}
		return true;
	}

	public static boolean definitely_inside(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		ArrayList<Vect2> fixp = fix_polygon(p,s,BUFF);
		boolean insidefixp = definitely_inside_prelim(fixp,s,BUFF);
		if (near_any_edge(p,s,BUFF)) {
			return false;
		}
		if (near_any_edge(fixp,s,BUFF)) {
			return false;
		}
		if (winding_number(p,s) != 1) {
			return false;
		}
		if (!insidefixp) {
			return false;
		}
		return true;
	}

	public static boolean definitely_outside_prelim(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		CrossAns mcdi = min_cross_dist_index(p,s);
		NumEdgesCross nuc = number_upshot_crosses(p,s);
		if (near_any_edge(p,s,BUFF)) return false;
		if (nuc.invalid) return false;
		if (mcdi.index<0) return true;
		if (p.get(mcdi.index).x>s.x) return false;
		int next_ind = mcdi.index<p.size()-1 ? mcdi.index+1 : 0;
		if (p.get(next_ind).x<s.x) return false;
		if ((nuc.num % 2)!=0) return false;
		return true;
	}

	public static boolean definitely_outside(ArrayList<Vect2> p,Vect2 s,double BUFF) {
		ArrayList<Vect2> fixp = fix_polygon(p,s,BUFF);
		boolean outsidefixp = definitely_outside_prelim(fixp,s,BUFF);		
		if (near_any_edge(p,s,BUFF)) {
			return false;
		}
		if (near_any_edge(fixp,s,BUFF)) {
			return false;
		}
		if (winding_number(p,s) != 0) {
			return false;
		}
		if (!outsidefixp) {
			return false;
		}
		return true;
	}

	public static boolean nice_polygon_2D(ArrayList<Vect2> p,double BUFF) {
		return acceptable_polygon_2D(p,BUFF) 
				&& counterclockwise_edges(p) 
				&& definitely_outside(p,test_point_below(p,BUFF),BUFF);
	}


}
