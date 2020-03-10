/*
 * Copyright (c) 2017-2019 United States Government as represented by
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
	
Route::Route(const Route& gsp) : names(gsp.names), infos(gsp.infos), positions(gsp.positions), radius_v(gsp.radius_v) {
		//positions = gsp.positions;
		//names = gsp.names;
		//radius_v = gsp.radius_v;
	}

Route::Route(const Plan& lpc, int start, int end) {
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.name(),lpc.getInfo(i));
		}
	}
	
Route::Route(const Plan& lpc) {
		int start = 0;
		int end = lpc.size()-1;
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.name(),lpc.getInfo(i));
		}

	}

	Route Route::mkRoute(const Plan& fp, int start, int end, double radius) {
		if (start < 0) start = 0;
		if (end >= fp.size()) end = fp.size()-1;
		Route rt;
		for (int i = start; i <= end; i++) {
			NavPoint np = fp.point(i);
			rt.add(np.position(),np.name(),fp.getInfo(i),radius);
		}
		return rt;
	}

	 Route Route::mkRouteNamedOnly(const Plan& fp) {
		Route rt;
		for (int i = 0; i < fp.size(); i++) {
			NavPoint np = fp.point(i);
			if (! larcfm::equals(np.name(),"")) {
			    rt.add(np.position(),np.name(),fp.getInfo(i));
			}
		}
		return rt;
	}
	
	 Route Route::mkRoute(const Plan& fp, double radius) {
	     return mkRoute(fp,0,fp.size()-1,radius);
	}


	Route Route::mkRouteBankAngle(const Plan& fp, double bankAngle) {
		Plan lpc = fp.copy();
		Route rt = Route();
		for (int i = 0; i < lpc.size(); i++) {
			NavPoint np = lpc.point(i);
			std::string info = lpc.getInfo(i);
			double turnDelta = Util::turnDelta(lpc.trkIn(i), lpc.trkOut(i));
			double gsIn = lpc.gsIn(i);
			//f.pln(" $$$$$ mkRouteBankAngle: i = "+i+" gsIn = "+Units.str("kn",gsIn));
			if (Util::almost_equals(gsIn,0.0)) {
				rt.add(np.position(),np.name(),info,0.0);
			} else {
				double R = lpc.vertexRadius(i);        // R stored in a linear plan
				//f.pln(" $$>> generateTurnTCPs: i = "+i+" VERTEX R = "+Units.str("NM",R));
				if (Util::almost_equals(R, 0.0)) {      // no specified radius
				    R = Kinematics::turnRadius(gsIn, bankAngle);
				}
				//f.pln(" $$$$$ mkRouteBankAngle: i = "+i+" R = "+Units.str("NM",R));
				//double turnTime = turnDelta*R/gsIn;
				double gsOut_i = lpc.gsOut(i);
				//f.pln(" $$$$$ mkRouteBankAngle: i = "+i+" gsOut_i = "+gsOut_i);
				if (turnDelta >= Plan::MIN_TRK_DELTA_GEN && gsOut_i > 1E-10) {  // do not generate when gsOut is near 0
					rt.add(np.position(),np.name(),info,R);
				} else {
					rt.add(np.position(),np.name(),info,0.0);
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
			rt.add(fp.position(i),fp.name(i),fp.info(i),fp.radius(i));
		}
		return rt;

	}

	
	int Route::size() const{
		return static_cast<int>(positions.size());
	}
	
	Position Route::position(int i) const {
		if (i < 0 || i >= size()) return Position::INVALID();
		else return positions[i];
	}
	
	Position Route::positionFromDistance(double dist, bool linear) const{
        double anyGs = Units::from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.getFirstTime();
		if ( ! linear) {
			p = TrajGen::generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil::positionFromDistance(p, startTime, dist, linear).first;
	}

	Position Route::positionFromDistance(int i, double dist, bool linear) const{
        double anyGs = Units::from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.time(i);
		if ( ! linear) {
			p = TrajGen::generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil::positionFromDistance(p, startTime, dist, linear).first;
	}
	
	std::string Route::name(int i) const{
		if (i < 0 || i >= size()) return "<INVALID>";
		else return names[i];
	}
	
	std::string Route::info(int i) const{
		if (i < 0 || i >= size()) return "<INVALID>";
		else return infos[i];
	}


	double Route::radius(int i) const{
		if (i < 0 || i >= size()) return 0;
		else return radius_v[i];
	}

//	bool Route::isAltPreserve(int i) const{
//		if (i < 0 || i >= size()) return false;
//		else return altPreserve_v[i];
//	}
//
//	void Route::setAltPreserve(int i, bool val) {
//		if (i >= 0 && i < size()) {
//			altPreserve_v[i] = val;
//		}
//	}


	void Route::add(const Position& pos, const std::string& label, const std::string& data) {
		positions.push_back(pos);
		names.push_back(label);
		infos.push_back(data);
		radius_v.push_back(0.0);
	}
	

	void Route::add(const Position& pos,  const std::string& label, const std::string& data, double rad) {
		//f.pln(" ################### Route.add: "+pos+" "+label+" radius = "+Units.str("nm",rad));
		positions.push_back(pos);
		names.push_back(label);
		infos.push_back(data);
		radius_v.push_back(rad);
	}

	void Route::add(int ix, const Position& pos, const std::string& label, const std::string& data, double rad) {
		positions.insert(positions.begin()+ix,pos);
		names.insert(names.begin()+ix,label);
		infos.push_back(data);
		radius_v.insert(radius_v.begin()+ix,rad);
	}
	
	void Route::set(int ix, const Position& pos, const std::string& label, const std::string& data, double rad) {
		positions[ix] = pos;
		names[ix] = label;
		infos.push_back(data);
		radius_v[ix] = rad;
	}

		
	void Route::remove(int i) {
		if (i < 0 || i >= static_cast<int>(positions.size())) return;
		positions.erase(positions.begin()+i);
		names.erase(names.begin()+i);
		infos.erase(infos.begin()+i);
		radius_v.erase(radius_v.begin()+i);
	}
	
	void Route::removeFirst() {
		positions.erase(positions.begin());
		names.erase(names.begin());
		infos.erase(infos.begin());
		radius_v.erase(radius_v.begin());
	}

	
	void Route::removeLast() {
		remove(static_cast<int>(positions.size())-1);
	}

	
	void Route::add(const Route& p, int ix) {
		positions.push_back(p.positions[ix]);
		names.push_back(p.names[ix]);
		infos.push_back(p.infos[ix]);
		radius_v.push_back(p.radius_v[ix]);
	}
	
	void Route::addAll(const Route& p) {
		positions.insert(positions.end(), p.positions.begin(), p.positions.end());
		names.insert(names.end(), p.names.begin(), p.names.end());
		infos.insert(infos.end(), p.infos.begin(), p.infos.end());
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
	

	int Route::findInfo(const std::string&  nm, int startIx) const {
		for (int i = startIx; i < (int) positions.size(); i++) {
			if (larcfm::equals(infos[i],nm)) return i;
		}
		return -1;
	}

	int Route::findInfo(const std::string&  nm) const {
		return findInfo(nm, 0);
	}

	void Route::setInfo(int i, const std::string& data) {
		if (i < 0 || i >= size()) {
			fpln(" $$$ ERROR: Route.setInfo: index out of range");
		} else {
			infos[i] = data;
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
             rtn.add(positions[i], names[i], infos[i], radius_v[i]);
		}
		return rtn;
	}
	
	double Route::pathDistance(int i, int j, bool linear) const{
		double anyGs = Units::from("kts",300);
		Plan lpc = linearPlan(0.0,anyGs);
		Plan kpc = lpc;
		if (i >= size()) {
			i = size()-1;
		}
		if (j >= size()) {
			j = size()-1;
		}
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		int kix = i;
		int kjx = j;
		double rtn = -1;
		if ( linear) {
			rtn = kpc.pathDistance(kix,kjx,linear);
		} else {
			lpc.setInfo(i,"$startIx");
			lpc.setInfo(j,"$endIx");
			kpc = TrajGen::generateTurnTCPsRadius(lpc);
			//f.pln(" $$$ Route.pathDistance: kpc = "+kpc);
			kix = kpc.findInfo("$startIx");
			kjx = kpc.findInfo("$endIx");
//			if (kpc.isBOT(kix) && TrajGen::vertexNameInBOT) {
//				kix = kpc.findMOT(kix);
//			}
//			if (kpc.isBOT(kjx) && TrajGen::vertexNameInBOT) {
//				kjx = kpc.findMOT(kjx);
//			}
			//f.pln(" $$$ Route.pathDistance: kix = "+kix+" kjx = "+kjx);
			rtn = kpc.pathDistance(kix,kjx);
		}
		//f.pln(" $$>>>>>>>> EXIT Route.pathDistance("+i+","+j+") = kpc.pathDist("+kix+","+kjx+"): rtn = "+Units.str("NM",rtn,12));
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
		return PlanUtil::positionFromDistance(p, startTime, dist, false).first;
	}

	Velocity Route::velocityFromDistance(double dist, double gs, double defaultBank, bool linear) const{
		Plan p = linearPlan(0,gs);
		if (!linear) {
			p = TrajGen::generateTurnTCPs(p, defaultBank);
		}
		return p.velocityFromDistance(dist);
	}


	Plan Route::linearPlan(double startTime, double gs) const{
		Plan lpc = Plan("");
		if (positions.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = positions[0];
		NavPoint nvp = NavPoint(lastNp,startTime,names[0]);
		TcpData  tcp = TcpData(); // .setLinearIndex(0);
		lpc.add(nvp,tcp);
		for (int i = 1; i < (int) positions.size(); i++) {
			Position np = positions[i];
			double pathDist = np.distanceH(lastNp);
			double t = lastT + pathDist/gs;
			double rad = radius_v[i];
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs)+" t = "+t+" rad = "+Units.str("ft",rad));
			tcp = TcpData().setRadiusSigned(rad); //.setLinearIndex(i);
			//NavPt navP = new NavPt(nvp,tcp);
			std::string nm = name(i);
			if (larcfm::equals(nm,virtualName)) {
				tcp = tcp.setVirtual();
				nm = "";
			}
			nvp = NavPoint(np,t,nm);
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
		std::string rtn = "size = "+Fm0(positions.size())+"\n";
		double dist = 0;
		for (int i = 0; i < static_cast<int>(positions.size()); i++) {
			rtn += " "+padLeft(""+Fm0(i),2)+" "+positions[i].toString2D(6)+" "+padRight(names[i],15)+" "+padRight(infos[i],15);
			if (radius_v[i] != 0.0) rtn += " radius_v = "+Units::str("NM",radius_v[i]);
			if (i > 0) dist += pathDistance(i-1,i, false);
			rtn += ", dist="+Units::str("NM",dist);
			rtn += "\n";
		}
		return rtn;
	}
	
	std::string Route::toString(double startTime, double gs) const{
	    return linearPlan(startTime,gs).toStringGs();
	}
	

}
