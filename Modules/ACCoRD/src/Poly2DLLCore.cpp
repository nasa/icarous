/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Poly2DLLCore.h"
#include "LatLonAlt.h"
#include "Pair.h"
#include "Vect3.h"
#include "GreatCircle.h"
#include "format.h"

namespace larcfm {

//			  % The following functions need to be defined before using containment
//
//			  acceptable_spherical_polygon(N:posnat,p:spherical_polygon(N)): bool = TRUE
//
//			  nice_spherical_polygon(N:posnat,p:spherical_polygon(N)): bool = TRUE

//const double Poly2DLLCore::BUFF = 1E-12;


// This doesn't work yet with wrap-around but it soon will
/**
 * Return a (answer,isInvalid) pair
 * @return
 */
std::pair<bool,bool> Poly2DLLCore::upshot_crosses_edge(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, int i) {
	int next = 0;
//	fpln("       upshot crosses edge "+Fm0(i));
	if (i < (int) p.size()-1) next = i+1;
//	fpln("lon1 is "+Fm4(p[i].lon())+" and lon2 is "+Fm4(p[next].lon())+" and thislon is "+Fm4(ll.lon()));
	if (p[i].lon() > ll.lon() && p[next].lon() > ll.lon()) {
//		fpln("         hit 1");
		return std::pair<bool,bool>(false,false);
	} else if (p[i].lon() < ll.lon() && p[next].lon() < ll.lon()) {
//		fpln("         hit 2");
		return std::pair<bool,bool>(false,false);
	} else if (p[i].lon() == p[next].lon() && (p[i].lat() >= ll.lat() || p[next].lat() >= ll.lat())) {
//		fpln("         hit 3");
		return std::pair<bool,bool>(true,true); // call this function when already perturbed
	} else if (p[i].lon() == p[next].lon()) {
//		fpln("         hit 4");
		return std::pair<bool,bool>(false, false);
	} else {
		Vect3 thisv = GreatCircle::spherical2xyz(p[i]);
		Vect3 nextv = GreatCircle::spherical2xyz(p[next]);
		Vect3 llv = GreatCircle::spherical2xyz(ll);
		bool left = llv.dot(thisv.cross(nextv)) > 0;
		bool piright = p[i].lon() >= ll.lon();
		if ((piright && left) || !(piright || left)) {
//			fpln("      YYYYYYYYYYYYYYYYYYYYYYYYYYYY   hit 5  1");
			return std::pair<bool,bool>(true, false);
		} else {
//			fpln("      YYYYYYYYYYYYYYYYYYYYYYYYYYYY   hit 5  2");
			return std::pair<bool,bool>(false, false);
		}
	}
}

bool Poly2DLLCore::edges_crossed_upto(const std::vector<LatLonAlt>& p, const LatLonAlt& s, int i, int j) {
	return j <= i && upshot_crosses_edge(p,s,j).first;
}

//	NumEdgesCross: TYPE = [# num:nat,invalid:bool #]

std::pair<int,bool> Poly2DLLCore::number_upshot_crosses_upto(const std::vector<LatLonAlt>& p, const LatLonAlt& ll) {
	int totalnum = 0;
	bool invalid = false;
//fpln("number_upshot_crosses_upto p.size="+Fm0(p.size()));
	for (int i=0; i < (int) p.size(); i++) {
		std::pair<bool,bool> thiscross = upshot_crosses_edge(p,ll,i);
		int thisnum = thiscross.first?1:0;
		totalnum = totalnum+thisnum;
		invalid = invalid || thiscross.second;
//fpln("i="+Fm0(i)+" totalnum="+Fm0(totalnum)+" invalid="+Fmb(invalid));
	}
	return std::pair<int,bool>(totalnum, invalid);
}


std::vector<LatLonAlt> Poly2DLLCore::fix_polygon(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF) {
	//fpln("alling fixpoly with ll1 = "+p.get(0).lon()+" and ll2 = "+p.get(1).lon()+" and ll3 = "+p.get(2).lon());
	std::vector<LatLonAlt> fp; //  = new ArrayList<LatLonAlt>();
	for (int i = 0; i < (int) p.size(); i++) {
		if (std::abs(p[i].lon() - ll.lon()) < BUFF) { // removed lat test
			//fpln(" WAS INSIDE BUFFFFFFFFEEEEERRR");
			fp.push_back(LatLonAlt::mk(p[i].lat(), p[i].lon()-2*BUFF, p[i].alt()));
		} else {
			fp.push_back(p[i]);
		}
	}
	return fp;
}

bool Poly2DLLCore::spherical_inside_prelim(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF) {
	std::pair<int,bool> nuc = number_upshot_crosses_upto(p,ll);
//fpln("spherical_inside_prelim nuc="+Fmb(nuc.second)+" "+Fm0(nuc.first)+" "+Fm0(nuc.first%2));
	return nuc.second == false && (nuc.first % 2) != 0;
}

/**
 * Main function
 * @param p
 * @param ll
 * @param BUFF
 * @return
 */
bool Poly2DLLCore::spherical_inside(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF) {
	//fpln("calling spherical_inside with ll1 = "+p.get(0).lon()+" and ll2 = "+p.get(1).lon()+" and ll3 = "+p.get(2).lon());
	std::vector<LatLonAlt> fixp = fix_polygon(p, ll, BUFF);
	bool insidefixp = spherical_inside_prelim(fixp,ll,BUFF);
	return insidefixp;

}

bool Poly2DLLCore::spherical_outside_prelim(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF) {
	std::pair<int,bool> nuc = number_upshot_crosses_upto(p,ll);
	return nuc.second == false && nuc.first % 2 == 0;
}

/**
 * Main function
 * @param p
 * @param ll
 * @param BUFF
 * @return
 */
bool Poly2DLLCore::spherical_outside(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF) {
	std::vector<LatLonAlt> fixp = fix_polygon(p, ll, BUFF);
	bool outsidefixp = spherical_outside_prelim(fixp,ll,BUFF);
	return outsidefixp;
}

}
