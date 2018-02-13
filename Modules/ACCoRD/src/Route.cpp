/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Route.h"
#include "Plan.h"
#include "Position.h"
#include "NavPoint.h"
#include "format.h"
#include "string_util.h"
#include "TrajGen.h"
#include "PlanUtil.h"
#include "Units.h"
#include "Velocity.h"
#include <string>
#include <vector>

namespace larcfm {

const std::string Route::virtualName = "$virtual";

Route::Route() {
	}
	
Route::Route(const Route& gsp) {
		positions = gsp.positions;
		names = gsp.names;
		radius_v = gsp.radius_v;
	}

Route::Route(const Plan& lpc, int start, int end) {
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.label());
		}
	}
	
Route::Route(const Plan& lpc) {
		int start = 0;
		int end = lpc.size()-1;
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.label());
		}

	}

	Route Route::mkRoute(const Plan& fp, int start, int end, double radius) {
		if (start < 0) start = 0;
		if (end >= fp.size()) end = fp.size()-1;
		Route rt;
		for (int i = start; i <= end; i++) {
			NavPoint np = fp.point(i);
			rt.add(np.position(),np.label(),radius);
		}
		return rt;
	}

	 Route Route::mkRouteNamedOnly(const Plan& fp) {
		Route rt;
		for (int i = 0; i < fp.size(); i++) {
			NavPoint np = fp.point(i);
			if (! larcfm::equals(np.label(),"")) {
			    rt.add(np.position(),np.label());
			}
		}
		return rt;
	}
	
	 Route Route::mkRoute(const Plan& fp, double radius) {
	     return mkRoute(fp,0,fp.size()-1,radius);
	}

	
	Route Route::mkRouteBankAngle(const Plan& fp, double bankAngle) {
		Plan lpc = fp.copyWithIndex();
		//Plan kpc = TrajGen.generateTurnTCPs(lpc, bankAngle);
		bool continueGen = true;
		//f.pln(" $$$ mkRouteBankAngle: lpc = "+lpc.toStringGs());
		Plan kpc = TrajGen::generateTurnTCPs(lpc,bankAngle,continueGen);
		//f.pln(" $$$ mkRouteBankAngle: kpc = "+kpc);
		if (kpc.hasError()) {
			fpln("WARNING: Route.mkRouteBankAngle: "+kpc.getMessageNoClear());
		}
		Route rt;
	    double radius = 0.0;
	    std::string labelBOT = "";
	    int lastLinIndex = -1;
		for (int j = 0; j < kpc.size(); j++) {
		    NavPoint np = kpc.point(j);
		    //f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>>.. makeRoute: np = "+np);
		    if (kpc.isBOT(j)) {
		    	radius = kpc.signedRadius(j);
		    	labelBOT = np.label();
		    } else if (kpc.isEOT(j)) {
		    	radius = 0.0;
		    	labelBOT = "";
		    } else {
		    	std::string label = np.label();
		    	if (radius != 0) label = labelBOT;
		    	int linIndex = kpc.getTcpData(j).getLinearIndex();
		    	//f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>>.. makeRoute: linIndex = "+linIndex+" lastLinIndex = "+lastLinIndex);
		    	if (linIndex != lastLinIndex) { 
		    		NavPoint np_lpc = lpc.point(linIndex);
			    	//f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>> radius = "+Units.str("NM",radius));
		    	    rt.add(np_lpc.position(),label,radius);
		    	    lastLinIndex = linIndex;
		    	}
		    }
		}
		if (rt.size() != fp.size()) {
			fpln("WARNING: Route.mkRouteBankAngle: route size "+Fm0(rt.size())+" != plan size "+Fm0(fp.size()));
			fpln("plan="+fp.toString());
			fpln("route="+rt.toString());
		}
		return rt;
	}

	Route Route::mkRouteCut(const Route& fp, int start, int end) {
		if (start < 0) start = 0;
		if (end >= fp.size()) end = fp.size()-1;
		Route rt;
		for (int i = start; i <= end; i++) {
			//f.pln(" $$ mkRouteCut: i = "+i+" fp.position(i) = "+fp.position(i)+" fp.name(i) = "+fp.name(i));
			rt.add(fp.position(i),fp.name(i),fp.radius(i));
		}
		return rt;

	}

	
	int Route::size() const{
		return positions.size();
	}
	
	Position Route::position(int i) const {
		if (i < 0 || i >= size()) return Position::INVALID();
		else return positions[i];
	}
	
	Position Route::positionByDistance(double dist, bool linear) const{
        double anyGs = Units::from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.getFirstTime();
		if ( ! linear) {
			p = TrajGen::generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil::advanceDistance(p, startTime, dist, linear).first;
	}

	Position Route::positionByDistance(int i, double dist, bool linear) const{
        double anyGs = Units::from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.time(i);
		if ( ! linear) {
			p = TrajGen::generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil::advanceDistance(p, startTime, dist, linear).first;
	}
	
	std::string Route::name(int i) const{
		if (i < 0 || i >= size()) return "<INVALID>";
		else return names[i];
	}
	
	double Route::radius(int i) const{
		if (i < 0 || i >= size()) return 0;
		else return radius_v[i];
	}

	void Route::add(const Position& pos, const std::string& label) {
		positions.push_back(pos);
		names.push_back(label);
		radius_v.push_back(0.0);
	}
	
	void Route::add(const Position& pos,  const std::string& label, double rad) {
		//f.pln(" ################### Route.add: "+pos+" "+label+" radius = "+Units.str("nm",rad));
		positions.push_back(pos);
		names.push_back(label);
		radius_v.push_back(rad);
	}

	void Route::add(int ix, const Position& pos, const std::string& label, double rad) {
		positions.insert(positions.begin()+ix,pos);
		names.insert(names.begin()+ix,label);
		radius_v.insert(radius_v.begin()+ix,rad);
	}
	
	void Route::set(int ix, const Position& pos, const std::string& label, double rad) {
		positions[ix] = pos;
		names[ix] = label;
		radius_v[ix] = rad;
	}

		
	void Route::remove(int i) {
		positions.erase(positions.begin()+i);
		names.erase(names.begin()+i);
		radius_v.erase(radius_v.begin()+i);
	}
	
	void Route::removeFirst() {
		positions.erase(positions.begin());
		names.erase(names.begin());
		radius_v.erase(radius_v.begin());
	}

	
	void Route::removeLast() {
		remove(positions.size()-1);
	}

	
	void Route::add(const Route& p, int ix) {
		positions.push_back(p.positions[ix]);
		names.push_back(p.names[ix]);
		radius_v.push_back(p.radius_v[ix]);
	}
	
	void Route::addAll(const Route& p) {
		positions.insert(positions.end(), p.positions.begin(), p.positions.end());
		names.insert(names.end(), p.names.begin(), p.names.end());
		radius_v.insert(radius_v.end(), p.radius_v.begin(), p.radius_v.end());
	}
	
	Route Route::append(const Route& p2) {
		Route rtn;
		rtn.addAll(*this);
		rtn.addAll(p2);
		return rtn;
	}


	void Route::updateWithDefaultRadius(double default_radius) {
		for (int i = 0; i <= size(); i++) {
			if (radius_v[i] == 0.0) {
				radius_v[i] = default_radius;
			}
		}
	}

	int Route::findName(const std::string& nm, int startIx) const {
		for (int i = startIx; i < (int) positions.size(); i++) {
			std::string name = names[i];
			if (larcfm::equals(name, nm)) return i;
		}
		return -1;
	}
	
	int Route::findName(const std::string& nm) const {
		return findName(nm, 0);
	}

	
	void Route::setName(int i, const std::string& name) {
		if (i < 0 || i >= size()) {
			fpln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			names[i] = name;
		}
	}
	
	void Route::setPosition(int i, const Position& pos) {
		if (i < 0 || i >= size()) {
			fpln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			positions[i] = pos;
		}
	}
	
	void Route::setRadius(int i, double rad) {
		if (i < 0 || i >= size()) {
			fpln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			radius_v[i] = rad;
		}
	}

	Route Route::copy() const{
		Route rtn;
		for (int i = 0; i < (int) positions.size(); i++) {
             rtn.add(positions[i], names[i], radius_v[i]);
		}
		return rtn;
	}
	
	double Route::pathDistance(int i, int j, bool linear) const{
		//f.pln(" $$ pathDistance: ENTER ============================== i = "+i+" j = "+j);
        double anyGs = Units::from("kts",300);
		Plan linPlan = linearPlan(0.0,anyGs);
		Plan kpc = linPlan;
		
		if (i >= size()) {
			i = size()-1;
		}
		if (j >= size()) {
			j = size()-1;
		}
		if (i < 0) {
			i = 0;
		}
		if (j < 0) {
			j = 0;
		}		
		int kix = i;
		int kjx = j;
		if ( ! linear) {

		    kpc = TrajGen::generateTurnTCPsRadius(linPlan); //, 0.0); // , bankAngle); // -- is this right?

		    if (kpc.hasError()) { // No valid radius values are available
		    	linear = true;
		    	//f.pln(" $$ pathDistance: $$$$$$$$$$$$$ HERE I AM $$$$$$$$$$$$$$$$ "+kpc.getMessageNoClear());
		    	return linPlan.pathDistance(i,j,linear);
		    } else {
		    	//f.pln(" $$ Route.pathDistance: ................... kpc = "+kpc.toString());

		    	std::vector<int> iAl = kpc.findLinearIndex(i);

		    	//f.pln(" iAl = "+iAl);
		    	if (iAl.size() == 0) {
		    		fpln(" $$ Route.pathDistance: iAl.size() == 0");
		    		return -1;
		    	}
		    	if (kpc.isBOT(iAl[0])) {
		    		kix = iAl[1];
		    	} else {
		    		kix = iAl[0];
		    	}

		    	std::vector<int> jAl = kpc.findLinearIndex(j);

		    	//f.pln(" j="+j+"  jAl = "+jAl);
		    	if (jAl.size() == 0) {
		    		fpln(" $$ Route.pathDistance: jAl.size() == 0");
		    		return -1;
		    	}
		    	if (kpc.isBOT(jAl[0])) {
		    		kjx = jAl[1];
		    	} else {
		    		kjx = jAl[0];
		    	}
		    }
		}
		double rtn = kpc.pathDistance(kix,kjx,linear);
		//f.pln(" $$ Route.pathDistance: kix = "+kix+"  kjx="+kjx+" rtn = "+Units.str("NM",rtn));	
		//f.pln(" $$ pathDistance: EXIT ============================== i = "+i+" j = "+j+" rtn = "+Units.str("NM",rtn));
		return rtn;
	}
	
	
	double Route::pathDistance(bool linear) const{
		return pathDistance(0,size()-1,linear);
	}
	
	
	double Route::pathDistance() const{
		bool linear = false;
		return pathDistance(0,size()-1,linear);
	}

	Position Route::positionFromDistance(double dist, double gs, double defaultBank, bool linear) const{
		Plan p = linearPlan(0,gs);
		double startTime = p.getFirstTime();
		if (!linear) {
			p = TrajGen::generateTurnTCPs(p, defaultBank);
		}
		return PlanUtil::advanceDistance(p, startTime, dist, false).first;
	}

	Velocity Route::velocityFromDistance(double dist, double gs, double defaultBank, bool linear) const{
		Plan p = linearPlan(0,gs);
		if (!linear) {
			p = TrajGen::generateTurnTCPs(p, defaultBank);
		}
		return p.velocityByDistance(dist);
	}


	Plan Route::linearPlan(double startTime, double gs) const{
		Plan lpc = Plan("");
		if (positions.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = positions[0];
		NavPoint nvp = NavPoint(lastNp,startTime).makeLabel(names[0]);
		TcpData  tcp = TcpData::makeSource(nvp).setLinearIndex(0);
		lpc.add(nvp,tcp);
		for (int i = 1; i < (int) positions.size(); i++) {
			Position np = positions[i];
			double pathDist = np.distanceH(lastNp);
			double t = lastT + pathDist/gs;
			double rad = radius_v[i];
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs)+" t = "+t+" rad = "+Units.str("ft",rad));
			nvp = NavPoint(np,t).makeLabel(names[i]);
			tcp = TcpData::makeSource(nvp).setRadiusSigned(rad).setLinearIndex(i);
			//NavPt navP = new NavPt(nvp,tcp);
			if (larcfm::equals(names[i],virtualName)) tcp = tcp.setVirtual();
			lpc.add(nvp,tcp);
			lastT = t;
			lastNp = np;
		}
		return lpc;

	}
	
	bool Route::equals(const Route& fp) const{
		for (int i = 0; i < fp.size(); i++) {                // Unchanged
			if (position(i) != fp.position(i)) return false;
			if (! larcfm::equals(name(i), fp.name(i))) return false;
			if (std::abs(radius_v[i] - fp.radius_v[i]) > 1E-10) return false;
		}
		return true;
	}


	
	std::string Route::toString() const{
		std::string rtn = "PrePlan size = "+Fm0(positions.size())+"\n";
		double dist = 0;
		for (int i = 0; i < (int) positions.size(); i++) {
			rtn += " "+padLeft(""+Fm0(i),2)+" "+positions[i].toString2D(6)+" "+padRight(names[i],15);
			if (radius_v[i] != 0.0) rtn += " radius_v = "+Units::str("NM",radius_v[i]);
			if (i > 0) dist += pathDistance(i-1,i, false);
			rtn += " dist="+Units::str("NM",dist);
			rtn += "\n";
		}
		return rtn;
	}
	
	std::string Route::toString(double startTime, double gs) const{
	    return linearPlan(startTime,gs).toStringGs();
	}
	

}
