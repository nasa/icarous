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
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;

import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.ae;
import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.AE;
import static gov.nasa.larcfm.ACCoRD.PolycarpEdgeProximity.near_edge;
import static gov.nasa.larcfm.ACCoRD.PolycarpEdgeProximity.segments_2D_close;

public class PolycarpAcceptablePolygon {

	public static boolean near_poly_edge(ArrayList<Vect2> p,Vect2 s,double BUFF,int i) {
	    int next_ind = i < p.size()-1 ? i+1 : 0;
	    return near_edge(p.get(i),p.get(next_ind),s,BUFF);
	}

//	def printpoly(p):
//	    ans = "["
//	    for i in range(len(p)):
//	        ans = ans+str(p.get(i))+","
//	    ans = ans + "]"
//	    return ans

	public static class EdgeCross {
	    boolean ans = true;
	    boolean invalid = true;
	    
	    public EdgeCross(boolean b1, boolean b2) {
	    	ans = b1;
	    	invalid = b2;
	    }
	}

	public static EdgeCross upshot_crosses_edge(ArrayList<Vect2> p,Vect2 s,int i) {
	    int next_ind = i < p.size()-1 ? i+1 : 0;
	    double tester = (p.get(next_ind).x-p.get(i).x)*(p.get(next_ind).x-p.get(i).x) * (p.get(i).y-s.y) + (s.x-p.get(i).x)*(p.get(next_ind).y-p.get(i).y)*(p.get(next_ind).x-p.get(i).x);
	    if (p.get(i).x>s.x && p.get(next_ind).x>s.x) return new EdgeCross(false,false);
	    else if (p.get(i).x<s.x && p.get(next_ind).x<s.x) return new EdgeCross(false,false);
	    else if (ae(p.get(i).x,p.get(next_ind).x) && (p.get(i).y>=s.y || p.get(next_ind).y>=s.y)) return new EdgeCross(true,true);
	    else if (ae(p.get(i).x,p.get(next_ind).x)) return new EdgeCross(false,false);
	    else if (tester>=0) return new EdgeCross(true,false);
	    return new EdgeCross(false,false);
	}


	public static class CrossAns {
	    int index = 0;
	    double num = 0;
	    double denom = 1;
	    
	    public CrossAns(int a, double b, double c) {
	    	index = a;
	    	num = b;
	    	denom = c;
	    }
	}
	
	public static CrossAns compute_intercept(ArrayList<Vect2> p,Vect2 s,int i) {
		 int next_ind = i < p.size()-1 ? i+1 : 0;
	    if ((p.get(i).x>s.x && p.get(next_ind).x>s.x) ||
	       (p.get(i).x<s.x && p.get(next_ind).x<s.x) ||
	       ae(p.get(i).x,p.get(next_ind).x)) {
	        return new CrossAns(-1,1,1);
	    }
	    double newnum = (p.get(i).y-s.y)*(p.get(next_ind).x-p.get(i).x) + (s.x-p.get(i).x)*(p.get(next_ind).y-p.get(i).y);
	    double newdenom = p.get(next_ind).x-p.get(i).x;
	    CrossAns ans;
	    if (newnum*newdenom<0) {
	    	ans = new CrossAns(-1,1,1);
	    } else {
	    	ans = new CrossAns(i,newnum,newdenom);
	    }
	    return ans;
	}


	public static CrossAns min_cross_dist_index(ArrayList<Vect2> p,Vect2 s) {
		CrossAns curr = compute_intercept(p,s,0);
	    for (int j = 1; j < p.size(); j++) {
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

	public static boolean corner_lt_3deg(Vect2 v,Vect2 w) {
	    if (v.norm()<0.1 || w.norm()<0.1) return true;
	    if (v.dot(w)<-0.9986295347545738*(v.norm()*w.norm())) // #(cosine of angle)
	    	return true;
	    return false;
	}

	    

	public static boolean acceptable_polygon_2D(ArrayList<Vect2> p,double BUFF) {
	    if (p.size()<=2) return false;
	    for (int i = 0; i < p.size(); i++) {
	        for (int j = i; j < p.size(); j++) {
	            int mi = i<p.size()-1 ? i+1 : 0;
	            int mj = j<p.size()-1 ? j+1 : 0;
	            Vect2 pj = p.get(j);
	            Vect2 pi = p.get(i);
	            Vect2 pmi = p.get(mi);
	            Vect2 pmj = p.get(mj);
	            if (i==j) continue;
	            if (pi.within_epsilon(pj,AE)) return false;
	            if (j==mi && (near_poly_edge(p,pmj,BUFF,i) || near_poly_edge(p,pi,BUFF,j))) return false;
	            if (j==mi && corner_lt_3deg(pj.Sub(pi),pmj.Sub(pj))) return false;
	            if (j==mi) continue;
	            if (i==mj && (near_poly_edge(p,pmi,BUFF,j) || near_poly_edge(p,pj,BUFF,i))) return false;
	            if (i==mj && corner_lt_3deg(pi.Sub(pj),pmi.Sub(pi))) return false;
	            if (i==mj) continue;
	            if (segments_2D_close(pi,pmi,pj,pmj,BUFF)) return false;
	        }
	    }
	    return true;
	}

	public static int counterclockwise_corner_index(ArrayList<Vect2> p,int eps) {
	    int windex = 0;
	    for (int i = 1; i < p.size(); i++) { // python goes to <= p.size() ?
	        if (ae(p.get(windex).x,p.get(i).x) && p.get(windex).y>=p.get(i).y) windex = i;
	        if (ae(p.get(windex).x,p.get(i).x)) continue;
	        if (eps*p.get(windex).x>eps*p.get(i).x) windex = i;
	    }
	    return windex;
	}

	public static double min_y_val(ArrayList<Vect2> p) {
	    double curr = p.get(0).y;
	    for (int i = 1; i < p.size(); i++) {
	        if (p.get(i).y<=curr) {
	            curr = p.get(i).y;
	        }
	    }
	    return curr;
	}

	public static Vect2 test_point_below(ArrayList<Vect2> p,double BUFF) {
	    int minxindex = counterclockwise_corner_index(p,1);
	    int maxxindex = counterclockwise_corner_index(p,-1);
	    double minx = p.get(minxindex).x;
	    double maxx = p.get(maxxindex).x;
	    return new Vect2((minx+maxx)/2,min_y_val(p)-Math.abs(maxx-minx)-2*BUFF);
	}

	public static boolean counterclockwise_edges(ArrayList<Vect2> p) {
	    int l = counterclockwise_corner_index(p,1);
	    int r = counterclockwise_corner_index(p,-1);
//f.pln("l="+l);	    
//f.pln("r="+r);	    
	    int lprev = l>0 ? l-1 : p.size()-1;
	    int rprev = r>0 ? r-1 : p.size()-1;
	    int lnext_ind = l < p.size()-1 ? l+1 : 0;
	    int rnext_ind = r < p.size()-1 ? r+1 : 0;
	    Vect2 LPP = p.get(l).Sub(p.get(lprev));
	    Vect2 LPN = p.get(lnext_ind).Sub(p.get(l));
	    Vect2 RPP = p.get(r).Sub(p.get(rprev));
	    Vect2 RPN = p.get(rnext_ind).Sub(p.get(r));
	    boolean Lcc = (LPP.det(LPN)>0);
//f.pln("Lcc="+Lcc);	    
	    boolean Rcc = (RPP.det(RPN)>0);
//f.pln("Rcc="+Rcc);	    
	    return Lcc && Rcc;
	}
	
	public static boolean segment_near_any_edge(ArrayList<Vect2> p,double BUFF,Vect2 segstart,Vect2 segend) {
	    for (int i = 0; i < p.size(); i++) {
	    	int mi = i<p.size()-1 ? i+1 : 0;
	    	if (PolycarpEdgeProximity.segments_2D_close(segstart, segend, p.get(i), p.get(mi), BUFF)) {
	    		return true;
	    	}
	    }
	    return false;
	}

//	static Vect2[] pzt = {new Vect2(0.0,0.0),new Vect2(1.0,0.0),new Vect2(1.0,1.0),new Vect2(0.0,1.0)};

//	if not acceptable_polygon_2D(pzt,0.001):
//	    print("poly not working")

}
