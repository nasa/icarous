/*
 * PolycarpAcceptablePolygon - determining if a 2D polygon is well-formed
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//import gov.nasa.larcfm.Util.Vect2;
//import java.util.ArrayList;
//import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.ae;
//import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.AE;
//import static gov.nasa.larcfm.ACCoRD.PolycarpEdgeProximity.near_edge;
//import static gov.nasa.larcfm.ACCoRD.PolycarpEdgeProximity.segments_2D_close;

#include "PolycarpAcceptablePolygon.h"
#include "PolycarpDoubleQuadratic.h"
#include "PolycarpEdgeProximity.h"
#include "Vect2.h"
#include <vector>

namespace larcfm {

	bool PolycarpAcceptablePolygon::near_poly_edge(const std::vector<Vect2>& p,const Vect2& s,double BUFF,int i) {
	    int next_ind = i < (int) p.size()-1 ? i+1 : 0;
	    return PolycarpEdgeProximity::near_edge(p[i],p[next_ind],s,BUFF);
	}

	PolycarpAcceptablePolygon::EdgeCross PolycarpAcceptablePolygon::upshot_crosses_edge(const std::vector<Vect2>& p,const Vect2& s,int i) {
	    int next_ind = i < (int) p.size()-1 ? i+1 : 0;
	    double tester = (p[next_ind].x-p[i].x)*(p[next_ind].x-p[i].x) * (p[i].y-s.y) + (s.x-p[i].x)*(p[next_ind].y-p[i].y)*(p[next_ind].x-p[i].x);
	    if (p[i].x>s.x && p[next_ind].x>s.x) return PolycarpAcceptablePolygon::EdgeCross(false,false);
	    else if (p[i].x<s.x && p[next_ind].x<s.x) return PolycarpAcceptablePolygon::EdgeCross(false,false);
	    else if (PolycarpDoubleQuadratic::ae(p[i].x,p[next_ind].x) && (p[i].y>=s.y || p[next_ind].y>=s.y)) return PolycarpAcceptablePolygon::EdgeCross(true,true);
	    else if (PolycarpDoubleQuadratic::ae(p[i].x,p[next_ind].x)) return PolycarpAcceptablePolygon::EdgeCross(false,false);
	    else if (tester>=0) return PolycarpAcceptablePolygon::EdgeCross(true,false);
	    return PolycarpAcceptablePolygon::EdgeCross(false,false);
	}

	PolycarpAcceptablePolygon::CrossAns PolycarpAcceptablePolygon::compute_intercept(const std::vector<Vect2>& p,const Vect2& s,int i) {
		 int next_ind = i < (int) p.size()-1 ? i+1 : 0;
	    if ((p[i].x>s.x && p[next_ind].x>s.x) ||
	       (p[i].x<s.x && p[next_ind].x<s.x) ||
	       PolycarpDoubleQuadratic::ae(p[i].x,p[next_ind].x)) {
	        return PolycarpAcceptablePolygon::CrossAns(-1,1,1);
	    }
	    double newnum = (p[i].y-s.y)*(p[next_ind].x-p[i].x) + (s.x-p[i].x)*(p[next_ind].y-p[i].y);
	    double newdenom = p[next_ind].x-p[i].x;
	    PolycarpAcceptablePolygon::CrossAns ans(-1,1,1);
	    if (newnum*newdenom<0) {
	    	ans = PolycarpAcceptablePolygon::CrossAns(-1,1,1);
	    } else {
	    	ans = PolycarpAcceptablePolygon::CrossAns(i,newnum,newdenom);
	    }
	    return ans;
	}


	PolycarpAcceptablePolygon::CrossAns PolycarpAcceptablePolygon::min_cross_dist_index(const std::vector<Vect2>& p,const Vect2& s) {
	  PolycarpAcceptablePolygon::CrossAns curr = compute_intercept(p,s,0);
	    for (int j = 1; j < (int) p.size(); j++) {
	    	CrossAns cii = compute_intercept(p,s,j);
	        if (curr.index<0) {
	            curr = cii;
	            continue;
	        }
	        if (cii.index<0) {
	        	continue;
	        }
	        if (curr.denom*curr.denom*cii.num*cii.denom<curr.num*curr.denom*cii.denom*cii.denom) {
	            curr = cii;
	        }
	    }
	    return curr;
	}

	bool PolycarpAcceptablePolygon::corner_lt_3deg(const Vect2& v,const Vect2& w) {
	    if (v.norm()<0.1 || w.norm()<0.1) return true;
	    if (v.dot(w)<-0.9986295347545738*(v.norm()*w.norm())) // #(cosine of angle)
	    	return true;
	    return false;
	}

	    

	bool PolycarpAcceptablePolygon::acceptable_polygon_2D(const std::vector<Vect2>& p,double BUFF) {
	    if (p.size()<=2) return false;
	    for (int i = 0; i < (int) p.size(); i++) {
	        for (int j = i; j < (int) p.size(); j++) {
	            int mi = i < (int) p.size()-1 ? i+1 : 0;
	            int mj = j < (int) p.size()-1 ? j+1 : 0;
	            Vect2 pj = p[j];
	            Vect2 pi = p[i];
	            Vect2 pmi = p[mi];
	            Vect2 pmj = p[mj];
	            if (i==j) continue;
	            if (pi.within_epsilon(pj,PolycarpDoubleQuadratic::AE)) return false;
	            if (j==mi && (near_poly_edge(p,pmj,BUFF,i) || near_poly_edge(p,pi,BUFF,j))) return false;
	            if (j==mi && corner_lt_3deg(pj.Sub(pi),pmj.Sub(pj))) return false;
	            if (j==mi) continue;
	            if (i==mj && (near_poly_edge(p,pmi,BUFF,j) || near_poly_edge(p,pj,BUFF,i))) return false;
	            if (i==mj && corner_lt_3deg(pi.Sub(pj),pmi.Sub(pi))) return false;
	            if (i==mj) continue;
	            if (PolycarpEdgeProximity::segments_2D_close(pi,pmi,pj,pmj,BUFF)) return false;
	        }
	    }
	    return true;
	}

	int PolycarpAcceptablePolygon::counterclockwise_corner_index(const std::vector<Vect2>& p,int eps) {
	    int windex = 0;
	    for (int i = 0; i < (int) p.size(); i++) {
	        if (PolycarpDoubleQuadratic::ae(p[windex].x,p[i].x) && p[windex].y>=p[i].y) windex = i;
	        if (PolycarpDoubleQuadratic::ae(p[windex].x,p[i].x)) continue;
	        if (eps*p[windex].x>eps*p[i].x) windex = i;
	    }
	    return windex;
	}

	double PolycarpAcceptablePolygon::min_y_val(const std::vector<Vect2>& p) {
	    double curr = p[0].y;
	    for (int i = 0; i < (int) p.size(); i++) {
	        if (p[i].y<=curr) {
	            curr = p[i].y;
	        }
	    }
	    return curr;
	}

	Vect2 PolycarpAcceptablePolygon::test_point_below(const std::vector<Vect2>& p,double BUFF) {
	    int minxindex = counterclockwise_corner_index(p,1);
	    int maxxindex = counterclockwise_corner_index(p,-1);
	    double minx = p[minxindex].x;
	    double maxx = p[maxxindex].x;
	    return Vect2((minx+maxx)/2,min_y_val(p)-std::abs(maxx-minx)-2*BUFF);
	}

	bool PolycarpAcceptablePolygon::counterclockwise_edges(const std::vector<Vect2>& p) {
	    int l = counterclockwise_corner_index(p,1);
	    int r = counterclockwise_corner_index(p,-1);
	    int lprev = l>0 ? l-1 : p.size()-1;
	    int rprev = r>0 ? r-1 : p.size()-1;
	    int lnext_ind = l < (int) p.size()-1 ? l+1 : 0;
	    int rnext_ind = r < (int) p.size()-1 ? r+1 : 0;
	    Vect2 LPP = p[l].Sub(p[lprev]);
	    Vect2 LPN = p[lnext_ind].Sub(p[l]);
	    Vect2 RPP = p[r].Sub(p[rprev]);
	    Vect2 RPN = p[rnext_ind].Sub(p[r]);
	    bool Lcc = (LPP.det(LPN)>0);
	    bool Rcc = (RPP.det(RPN)>0);
	    return Lcc && Rcc;
	}

    bool PolycarpAcceptablePolygon::segment_near_any_edge(const std::vector<Vect2>& p,double BUFF,
                  const Vect2& segstart, const Vect2& segend) {
        for (int i = 0; i < (int) p.size(); i++) {
          int mi = i < (int) p.size()-1 ? i+1 : 0;
          if (PolycarpEdgeProximity::segments_2D_close(p[i],p[mi],segstart,segend,BUFF)) return true;
        }
        return false;
    }


}
