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
#include "PolycarpContain.h"
#include "Vect2.h"
#include "PolycarpAcceptablePolygon.h"
#include <vector>

namespace larcfm {

	bool PolycarpContain::near_any_edge(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
		for (int i = 0; i < (int) p.size(); i++) {
			if (PolycarpAcceptablePolygon::near_poly_edge(p,s,BUFF,i)) {
				return true;
			}
		}
		return false;
	}


	PolycarpContain::NumEdgesCross PolycarpContain::number_upshot_crosses(const std::vector<Vect2>& p,const Vect2& s) {
		int num = 0;
		bool invalid = false;
		for (int i = 0; i < (int) p.size(); i++) {
		  PolycarpAcceptablePolygon::EdgeCross uce = PolycarpAcceptablePolygon::upshot_crosses_edge(p,s,i);
			if (uce.ans) {
				num=num+1;
			}
			if (uce.invalid) {
				invalid = true;
			}
		}
		return NumEdgesCross(num,invalid);
	}

	int PolycarpContain::quadrant(const Vect2& s) {
		if (s.x>=0 && s.y>=0) return 1;
		if (s.x<=0 && s.y>=0) return 2;
		if (s.x<=0) return 3;
		return 4;
	}

	int PolycarpContain::winding_number(const std::vector<Vect2>& p,const Vect2& s) {
		//#print("calling winding_number with p = "+str(p))
		int total = 0;
		int q = quadrant(p[0].Sub(s));
		for (int i = 0; i < (int) p.size(); i++) {
			int k = q;
			Vect2 thisv = p[i].Sub(s);
			Vect2 nextv = i < (int) p.size()-1 ? p[i+1].Sub(s) : p[0].Sub(s);
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
		return total/4;
	}

	std::vector<Vect2> PolycarpContain::fix_polygon(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
		std::vector<Vect2> newp = p;
		for (int i = 0; i < (int) p.size(); i++) {
			if (p[i].y>=s.y-BUFF && std::abs(p[i].x-s.x)<BUFF) {
				newp[i] = p[i].Sub(Vect2(2*BUFF,0));
			}
		}
		return newp;
	}

	bool PolycarpContain::definitely_inside_prelim(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
	  PolycarpAcceptablePolygon::CrossAns mcdi = PolycarpAcceptablePolygon::min_cross_dist_index(p,s);
		NumEdgesCross nuc = number_upshot_crosses(p,s);
		if (mcdi.index<0) {
			return false;
		}
		if (nuc.invalid) {
			return false;
		}
		if (p[mcdi.index].x<s.x) {
			return false;
		}
		int next_ind = mcdi.index < (int) p.size()-1 ? mcdi.index+1 : 0;
		if (p[next_ind].x>s.x) {
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

	bool PolycarpContain::definitely_inside(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
		std::vector<Vect2> fixp = fix_polygon(p,s,BUFF);
		bool insidefixp = definitely_inside_prelim(fixp,s,BUFF);
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

	bool PolycarpContain::definitely_outside_prelim(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
	  PolycarpAcceptablePolygon::CrossAns mcdi = PolycarpAcceptablePolygon::min_cross_dist_index(p,s);
	  NumEdgesCross nuc = number_upshot_crosses(p,s);
		if (near_any_edge(p,s,BUFF)) return false;
		if (nuc.invalid) return false;
		if (mcdi.index<0) return true;
		if (p[mcdi.index].x>s.x) return false;
		int next_ind = mcdi.index<(int)p.size()-1 ? mcdi.index+1 : 0;
		if (p[next_ind].x<s.x) return false;
		if ((nuc.num % 2)!=0) return false;
		return true;
	}

	bool PolycarpContain::definitely_outside(const std::vector<Vect2>& p,const Vect2& s,double BUFF) {
		std::vector<Vect2> fixp = fix_polygon(p,s,BUFF);
		bool outsidefixp = definitely_outside_prelim(fixp,s,BUFF);
		if (near_any_edge(p,s,BUFF)) return false;
		if (near_any_edge(fixp,s,BUFF)) return false;
		if (winding_number(p,s)!=0) return false;
		if (!outsidefixp) return false;
		return true;
	}

	bool PolycarpContain::nice_polygon_2D(const std::vector<Vect2>& p,double BUFF) {
		return PolycarpAcceptablePolygon::acceptable_polygon_2D(p,BUFF)
				&& PolycarpAcceptablePolygon::counterclockwise_edges(p)
				&& definitely_outside(p,PolycarpAcceptablePolygon::test_point_below(p,BUFF),BUFF);
	}


}
