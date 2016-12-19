/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "TurnGeneration.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "GreatCircle.h"
#include "VectFuns.h"

namespace larcfm {
using std::string;
  
  Triple<NavPoint,NavPoint,NavPoint> TurnGeneration::turnGenerator(const NavPoint& np1, const NavPoint& np2, const NavPoint& np3, double radius) {
//		Triple<Position,Position,Position> points = turnGenerator(np1.position(), np2.position(), np3.position(), radius);
//		Position botPos = points.first;
//		Position motPos = points.second;	
//		Position eotPos = points.third;	
//		Position botPos;
//		Position motPos;
//		Position eotPos;
//		if (np2.isLatLon()) {
//			//TODO: Alternatives
//			//double genRadius = radius;
//			//radius = 0.9996*radius;     // TODO:  THIS improves things -- why?
//			Triple<LatLonAlt,LatLonAlt,LatLonAlt> tgP =  turnGeneratorLLA(np1.lla(),np2.lla(),np3.lla(),radius);
//			//Triple<LatLonAlt,LatLonAlt,LatLonAlt> tgP =  turnGeneratorLLA_Alt(np1.lla(),np2.lla(),np3.lla(),radius);
//			//Triple<LatLonAlt,LatLonAlt,LatLonAlt> tgP =  turnGeneratorLLA_Alt3(np1.lla(),np2.lla(),np3.lla(),radius);
//			botPos = Position(tgP.first);
//			motPos = Position(tgP.second);
//			eotPos = Position(tgP.third);
//
//		} else {
//			Triple<Vect3,Vect3,Vect3> tgP =  turnGeneratorEucl(np1.point(),np2.point(),np3.point(),radius);
//			botPos = Position(tgP.first);
//			motPos = Position(tgP.second);
//			eotPos = Position(tgP.third);
//		}
//		Velocity vin = NavPoint::initialVelocity(np1,np2);
//		Velocity vout = NavPoint::initialVelocity(np2,np3);
//		Velocity vin2 = botPos.initialVelocity(np2.position(),100);
//	    int dir = Util::turnDir(vin.trk(),vout.trk());
//	    vin = vin.mkTrk(vin2.trk());
//		//Velocity vout2 = eotPos.initialVelocity(np3.position(),100);
//		Velocity vout2 = eotPos.initialVelocity(np2.position(),100);
//		vout = vout.mkTrk(vout2.trk()+M_PI);
//		//fpln(" $$$$$ TurnGenerator: vin = "+vin+" vout = "+vout);
//		double gs1 = vin.gs();
//		// make the time of the mot = the time of the middle vertex
//		double tMOT = np2.time();
//		double arcLength;
//		if (np2.isLatLon()) {
//			arcLength = arcLengthLLA(botPos.lla(),np2.lla(),radius);
//		} else {
//			arcLength = Util::turnDelta(vin.trk(),vout.trk())*radius;
//		}
//		//fpln(" $$$ TurnGenerator 241: arcLength= "+Units::str("ft",arcLength,6)+"  radius="+Units::str("ft",radius,6));
//		double turnTime = arcLength/gs1;
//		//fpln(" $$$$$ TurnGenerator: turnTime = "+turnTime);
//		double tBOT = tMOT - turnTime/2;
//		double tEOT = tMOT + turnTime/2;
//		double vs1 = NavPoint::initialVelocity(np1,np2).vs();
//		double vs2 = NavPoint::initialVelocity(np2,np3).vs();
//		double altMOT = np2.alt();
//		double altBOT = altMOT - vs1*turnTime/2;
//		double altEOT = altMOT + vs2*turnTime/2;
//        botPos = botPos.mkAlt(altBOT);
//        motPos = motPos.mkAlt(altMOT);
//        eotPos = eotPos.mkAlt(altEOT);
//        //double omega =  dir*gs1/radius;
//		//fpln(" $$$$$ TurnGenerator: turnTime = "+turnTime+" omega = "+Units::str("deg",omega,4)+" gs1 = "+Units::str("kn",gs1,4));
//		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos,tBOT, vin, dir*radius);
//        //fpln(" $$$ turnGenerator: npBOT = "+npBOT.toStringFull()+" alpha = "+alpha+" turnTime = "+turnTime);
//		//NavPoint npMOT = np2.makeMidpoint(motPos,tMOT,vin.mkTrk(trk2));
//		NavPoint npEOT = np2.makeOriginal().makeEOT(eotPos,tEOT, vout);
////		Position MOT = KinematicsPosition::turnOmega(npBOT.position(), vin, turnTime/2, omega).first;
////		NavPoint npMOT(motPos,tMOT); // .makeLabel("MOT");
//		//double trk2 = vin.trk() + omega * (tMOT-tBOT);
//		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT); // ,vin.mkTrk(trk2));
//		//fpln(" $$$ turnGenerator: npBOT = "+npBOT.toStringFull()+"\n          npEOT = "+npEOT.toStringFull());
//		return Triple<NavPoint,NavPoint,NavPoint>(npBOT, npMOT, npEOT);
//




		if (np1.time() > np2.time() || np2.time() > np3.time() || radius < 0.0) {
			return Triple<NavPoint,NavPoint,NavPoint>(NavPoint::INVALID(), NavPoint::INVALID(), NavPoint::INVALID());
		}

		Tuple5<Position,Position,Position,int,double> turn = turnGenerator(np1.position(), np2.position(), np3.position(), radius);
		Position botPos = turn.first;
		Position motPos = turn.second;
		Position eotPos = turn.third;
		int dir = turn.fourth;
		double arcLength = turn.fifth;

		Velocity vin = NavPoint::initialVelocity(np1,np2);  // used to set gs and vs
		Velocity vout = NavPoint::initialVelocity(np2,np3); // used to set gs and vs
		double gsIn = vin.gs();

		Velocity vin2 = botPos.initialVelocity(np2.position(),100);
		Velocity vout2 = np2.position().finalVelocity(eotPos,100);
		vin = vin.mkTrk(vin2.trk());
		vout = vout.mkTrk(vout2.trk());

		// make the time of the mot = the time of the middle vertex
		double tMOT = np2.time();

		double turnTime = arcLength/gsIn;
		double tBOT = tMOT - turnTime/2;
		double tEOT = tMOT + turnTime/2;
		double vs1 = vin.vs(); //NavPoint.initialVelocity(np1,np2).vs();
		double vs2 = vout.vs(); //NavPoint.initialVelocity(np2,np3).vs();
		double altMOT = np2.alt();
		double altBOT = altMOT - vs1*turnTime/2;
		double altEOT = altMOT + vs2*turnTime/2;
		botPos = botPos.mkAlt(altBOT);
		motPos = motPos.mkAlt(altMOT);
		eotPos = eotPos.mkAlt(altEOT);	

		int linearIndex = np2.linearIndex();
		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos, tBOT, vin, dir*radius, linearIndex);  // only BOT retains label
		NavPoint npEOT = np2.makeOriginal().makeLabel("").makeEOT(eotPos, tEOT, vout, linearIndex);
		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT, linearIndex).makeLabel("");
		return Triple<NavPoint,NavPoint,NavPoint>(npBOT, npMOT, npEOT);
	}
	
	
	Tuple5<Position,Position,Position,int,double> TurnGeneration::turnGenerator(const Position& np1, const Position& np2, const Position& np3, double radius) {
		//Triple<Position,Position,Position> tgP = turnGeneratorPosition(np1.position(), np2.position(), np3.position(), radius);
		Position botPos;
		Position motPos;	
		Position eotPos;	
		int direction;
		double distance;
		if (np2.isLatLon()) { 			
			Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> points = turnGeneratorLLA(np1.lla(),np2.lla(),np3.lla(),radius);
			//Triple<LatLonAlt,LatLonAlt,LatLonAlt> points = turnGeneratorLLA_Alt(np1.lla(),np2.lla(),np3.lla(),radius);
			//Triple<LatLonAlt,LatLonAlt,LatLonAlt> points = turnGeneratorLLA_Alt3(np1.lla(),np2.lla(),np3.lla(),radius);
			botPos = Position(points.first);
			motPos = Position(points.second);	
			eotPos = Position(points.third);
			direction = points.fourth;
			distance = points.fifth;
		} else {
			Tuple5<Vect3,Vect3,Vect3,int,double> points = turnGeneratorEucl(np1.point(),np2.point(),np3.point(),radius);
			botPos = Position(points.first);
			motPos = Position(points.second);	
			eotPos = Position(points.third);		
			direction = points.fourth;
			distance = points.fifth;
		}
		return Tuple5<Position,Position,Position,int,double>(botPos,motPos,eotPos,direction,distance);
	}

	
	

	Tuple5<Vect2,Vect2,Vect2,int,double> TurnGeneration::turnGeneratorEucl(const Vect2& pt1, const Vect2& pt2, const Vect2& pt3, double radius) {
		//fpln(" $$ turnGenerator: R = "+Units::str("NM",R));
		//fpln(" $$ turnGenerator: pt0 = "+pt0+" pt1 = "+pt1+"  pt2 = "+pt2+"  pt3 = "+pt3);		
		Vect2 ao = pt3.Sub(pt2);
		Vect2 bo = pt1.Sub(pt2);
		Vect2 ahat = ao.Hat();
		Vect2 bhat = bo.Hat();
		Vect2 v = ahat.Add(bhat);
		double k = radius/std::sqrt((v.sqv() - Util::sq(v.dot(ahat))));
		Vect2 w = v.Scal(k);
		Vect2 wdota = ahat.Scal(w.dot(ahat));
		Vect2 wdotb = bhat.Scal(w.dot(bhat));
		Vect2 BOT = pt2.Add(wdotb);
		Vect2 EOT = pt2.Add(wdota);
		double distAB = BOT.Sub(EOT).norm();
		double alpha = 2*(Util::asin_safe(distAB/(2*radius))); 
		double arcLength = alpha*radius;	//arc length
		int dir = VectFuns::rightOfLinePoints(pt1,pt2,pt3);

		// calculate MOT position
        Vect2 center = pt2.Add(w);
		Vect2 vhat = pt2.Sub(center).Hat();
		Vect2 MOT = center.Add(vhat.Scal(radius));
		//fpln(" $$$$$ turnGeneratorEucl: MOT = "+MOT);
		//double dist_B2 = w.dot(ahat);  // also = w.dot(bhat)
		return Tuple5<Vect2,Vect2,Vect2,int,double>(BOT,MOT,EOT,dir,arcLength);
	}

	

	Tuple5<Vect3,Vect3,Vect3,int,double> TurnGeneration::turnGeneratorEucl(const Vect3& p1, const Vect3& p2, const Vect3& p3, double radius) {
		//fpln(" $$ turnGenerator: R = "+Units::str("nm",R));
		Tuple5<Vect2,Vect2,Vect2,int,double> tge = turnGeneratorEucl(p1.vect2(), p2.vect2(), p3.vect2(), radius);
		Vect2 BOT = tge.first;
		Vect2 MOT = tge.second;
		Vect2 EOT = tge.third;
        double altBOT = p1.z; //Double.NaN;
        double altMOT = p1.z; //Double.NaN;
        double altEOT = p1.z; //Double.NaN;
		Vect3 BOTv3(BOT.x, BOT.y, altBOT);	
		Vect3 MOTv3(MOT.x, MOT.y, altMOT);	
		Vect3 EOTv3(EOT.x, EOT.y, altEOT);			
		return Tuple5<Vect3,Vect3,Vect3,int,double>(BOTv3,MOTv3,EOTv3,tge.fourth,tge.fifth);
	}


	Triple<NavPoint,NavPoint,NavPoint> TurnGeneration::turnGeneratorProjected(const NavPoint& np1, const NavPoint& np2, const NavPoint& np3, double R) {
		Vect2 pt1;
		Vect2 pt2;
		Vect2 pt3;
		Position p1 = np1.position();
		//fpln("\n >>>>>>>>>>>>>>>>>.. TurnGenerator: np1 = "+np1+" np2 = "+np2+" np3 = "+np3);
		Position p2 = np2.position();
		Position p3 = np3.position();
		// convert the points to Euclidean vectors (if necessary)
		EuclideanProjection proj = Projection::createProjection(p2.lla().lat(), p2.lla().lon(), 0); // preserves altitude
		if (p2.isLatLon()) {  
			double chkLen = Units::from("nm",10.0);
			LatLonAlt p1lla = p1.lla();
			if (p2.distanceH(p1) > chkLen) {
				p1lla = GreatCircle::linear_gc(p2.lla(), p1.lla(), chkLen);
				//fpln(" $$ TurnGenerator is using p1lla "+p1lla+" instead of "+p1.lla());
			}
			LatLonAlt p3lla = p3.lla();
			if (p2.distanceH(p3) > chkLen) {
				p3lla = GreatCircle::linear_gc(p2.lla(), p3.lla(), chkLen);
				//fpln(" $$ TurnGenerator is using p3lla "+p3lla+" instead of "+p3.lla());
			}
			pt1 = proj.project2(p1lla);
			pt2 = proj.project2(p2.lla());
			pt3 = proj.project2(p3lla);
		} else {
			pt1 = p1.vect2();
			pt2 = p2.vect2();
			pt3 = p3.vect2();
		}
		Vect2 ao = pt3.Sub(pt2);
		Vect2 bo = pt1.Sub(pt2);
		Vect2 ahat = ao.Hat();
		Vect2 bhat = bo.Hat();
		Vect2 v = ahat.Add(bhat);
		double k = R/std::sqrt((v.sqv() - Util::sq(v.dot(ahat))));
		Vect2 w = v.Scal(k);
		Vect2 wdota = ahat.Scal(w.dot(ahat));
		Vect2 wdotb = bhat.Scal(w.dot(bhat));
		Vect2 Center = pt2.Add(w);
		Vect2 BOT = pt2.Add(wdotb);
		Vect2 EOT = pt2.Add(wdota);
		double gs1 = np1.groundSpeed(np2);
		double distAB = BOT.Sub(EOT).norm();
		double sinTheta = distAB/(2*R);
		double alpha;
		if (std::abs(sinTheta) > 1.0) alpha = 0;
		else  alpha = 2*(Util::asin_safe(sinTheta)); 
		//fpln(" $$$$$ TurnGenerator: gs1 = "+Units::str("kn",gs1)+" alpha = "+alpha+" distAB "+Units::str("ft",distAB)+" R = "+Units::str("ft",R));
		double d2 = alpha*R;	//arc length
		Velocity v1 = np1.initialVelocity(np2);
		Velocity v2 = np2.initialVelocity(np3);
		// make the time of the mot = the time of the middle vertex
		double tMOT = np2.time();
		double turnTime = d2/gs1;
		//fpln(" $$$$$ TurnGenerator: turnTime = "+turnTime+" tMOT = "+tMOT+" alpha = "+Units::str("deg",alpha)+" gs1 = "+Units::str("kn",gs1));
		double tBOT = tMOT - turnTime/2;
		double tEOT = tMOT + turnTime/2;
		double vs1 = np1.initialVelocity(np2).vs();
		double vs2 = np2.initialVelocity(np3).vs();
		double altMOT = np2.alt();
		double altBOT = altMOT - vs1*turnTime/2;
		double altEOT = altMOT + vs2*turnTime/2;
		// calculate MOT position
		Vect2 vhat = pt2.Sub(Center).Hat();
		Vect2 mot = Center.Add(vhat.Scal(R));
		Vect3 v3BOT(BOT.x,BOT.y,altBOT);
		Vect3 v3MOT(mot.x,mot.y,altMOT);
		Vect3 v3EOT(EOT.x,EOT.y,altEOT);
		Position motPos;
		Position botPos;
		Position eotPos;
		if (p1.isLatLon()) {  
			motPos = Position(proj.inverse(v3MOT));
			botPos = Position(proj.inverse(v3BOT));
			eotPos = Position(proj.inverse(v3EOT));                         
		} else {
			motPos = Position(v3MOT);
			botPos = Position(v3BOT);
			eotPos = Position(v3EOT);
		}
		//double  vinTrk2(botPos,tBOT).initialVelocity(np2).trk();
		double vinTrk = pt2.Sub(BOT).trk();
		//fpln(" $$$ vinTrk = "+Units::str8("deg",vinTrk)+" vinTrk2 = "+Units::str8("deg",vinTrk2));
		//		double  voutTrk(eotPos,tEOT).initialVelocity(np3).trk();
		Velocity vin = v1.mkTrk(vinTrk);
		int dir = Util::turnDir(vin.trk(), v2.trk());
		//double signedRadius = Util::turnDir(vin.trk(), v2.trk())*R;
		double signedRadius = dir*R; // vin.gs()/omega;
		int linearIndex = np2.linearIndex();
		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos, tBOT, vin, signedRadius, linearIndex).makeLabel(np2.label());   // only BOT has label from np2
        //fpln(" $$$ turnGenerator: npBOT = "+npBOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT, linearIndex).makeLabel(""); // ,vin.mkTrk(trk2)).makeLabel("");   
		//fpln(" $$$ turnGenerator: npMOT = "+npMOT.toStringFull());
		double omega = dir*vin.gs()/R;    // TODO:  only used for EOT,  can calculate trkOut without this 
//		double trk2 = vin.trk() + omega * (tMOT-tBOT); // make vin track appropriate for each point
		double trk3 = vin.trk() + omega * (tEOT-tBOT);
		NavPoint npEOT = np2.makeOriginal().makeEOT(eotPos,tEOT, vin.mkTrk(trk3), linearIndex).makeLabel("");	
        //fpln(" $$$ turnGenerator: npEOT = "+npEOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		return Triple<NavPoint,NavPoint, NavPoint>(npBOT,npMOT,npEOT);
	}
	

	Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> TurnGeneration::turnGeneratorLLA(const LatLonAlt& p2, double trkIn, double trkOut, double radius) {
//        //fpln(" $$$$ turnGeneratorLLA: p2 = "+p2+" trkIn = "+Units::str("deg", trkIn)+" trkOut = "+Units::str("deg", trkOut)+" radius = "+Units::str("ft", radius));
//        double deltaTrack = Util::turnDelta(trkIn,trkOut);
//		int dir = Util::turnDir(trkIn,trkOut);
//		//fpln(" $$$$ turnGeneratorLLA: deltaTrack = "+Units::str("deg", deltaTrack)+" dir = "+dir);
//		double distance = radius*std::tan(deltaTrack/2.0);               // TODO ************* EUCLIDEAN ****************
//        //fpln("turnGeneratorLLA distance = "+distance+" distance2="+distance2+" delta="+(distance-distance2));
//		LatLonAlt botPos =  GreatCircle::linear_initial(p2, trkIn, -distance);
//		LatLonAlt eotPos =  GreatCircle::linear_initial(p2, trkOut, distance);
//		double cLineDist = radius/std::cos(deltaTrack/2.0) - radius;
//		double cTrk = trkIn + dir*(deltaTrack + M_PI)/2.0;
//		//fpln("  $$$$ turnGeneratorLLA:  cTrk = "+Units::str("deg", cTrk)+" cLineDist = "+cLineDist);
//	    //double foo = Util::to_2pi(trkIn+M_PI);
//	    //fpln(" $$$$ turnGeneratorLLA: "+Units::str("deg", foo)+"  "+Units::str("deg", trkOut)+" cTrk = "+Units::str("deg", Util::to_2pi(cTrk)));
//		LatLonAlt motPos =  GreatCircle::linear_initial(p2, cTrk, cLineDist);  // TODO: ????
//		//fpln(" $$ turnGeneratorLLA: botPos = "+botPos);
//		//fpln(" $$$$ turnGeneratorLLA: distance = "+Units::str("ft",distance));
//		return Quad<LatLonAlt,LatLonAlt,LatLonAlt,double>(botPos,motPos,eotPos,distance);

        double deltaTrack = Util::turnDelta(trkIn,trkOut);
		int dir = Util::turnDir(trkIn,trkOut);
		double distance = radius*std::tan(deltaTrack/2.0);               // TODO ************* EUCLIDEAN ****************
		LatLonAlt botPos =  GreatCircle::linear_initial(p2, trkIn, -distance);
		LatLonAlt eotPos =  GreatCircle::linear_initial(p2, trkOut, distance);
		double cLineDist = radius/std::cos(deltaTrack/2.0) - radius;
		double cTrk = trkIn + dir*(deltaTrack + M_PI)/2.0;
		LatLonAlt motPos =  GreatCircle::linear_initial(p2, cTrk, cLineDist);
		double arcLength = deltaTrack*radius;                            // TODO: ****** EUCLIDEAN *********
		//double arcLength = arcLengthLLA(botPos, p2, radius);
		return Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double>(botPos,motPos,eotPos,dir,arcLength);
	}
	

	Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> TurnGeneration::turnGeneratorLLA(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius) {
		double trkIn = GreatCircle::final_course(p1, p2);
		double trkOut = GreatCircle::initial_course(p2, p3);
		Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> tG = turnGeneratorLLA(p2, trkIn, trkOut, radius);
		double altBOT = p1.alt();
		double altMOT = p1.alt(); 
		double altEOT = p1.alt(); 
		LatLonAlt botPos = tG.first.mkAlt(altBOT);
		LatLonAlt motPos = tG.second.mkAlt(altMOT);              
		LatLonAlt eotPos = tG.third.mkAlt(altEOT);
		return Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double>(botPos,motPos,eotPos,tG.fourth,tG.fifth);
	}
	

	Triple<LatLonAlt,LatLonAlt,LatLonAlt> TurnGeneration::turnGeneratorLLA_Alt2(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius) {
		double A = GreatCircle::angle_between(p1, p2, p3)/2.0;
		Triple<double,double,double> bCc = GreatCircle::side_angle_angle(GreatCircle::angle_from_distance(radius,0.0), A, M_PI/2, true);
		double c = GreatCircle::distance_from_angle(bCc.third,  0.0);
		double track1 = GreatCircle::initial_course(p2, p1);
		double track2 = GreatCircle::initial_course(p2, p3);
		LatLonAlt botPos = GreatCircle::linear_initial(p2, track1, c);
		LatLonAlt eotPos = GreatCircle::linear_initial(p2, track2, c);
		double track1f = track1+M_PI;
		int dir = Util::turnDir(track1f, track2);
		double track3 = track2+A*dir;
		double b = GreatCircle::distance_from_angle(bCc.first,  0.0);
		LatLonAlt motPos = GreatCircle::linear_initial(p2, track3, b-radius);
		return Triple<LatLonAlt,LatLonAlt,LatLonAlt>(botPos,motPos,eotPos);
	}
	
	


	double TurnGeneration::arcLengthLLA(const LatLonAlt& bot, const LatLonAlt& intercept, double radius) {
		double a = GreatCircle::angle_from_distance(radius, 0.0);
		double b = GreatCircle::angular_distance(bot, intercept);
		if (Util::almost_equals(b, 0.0)) return 0.0;
		double B = GreatCircle::side_angle_side(a, M_PI/2, b).second;
		double angle = B*2;
		double radius_prime = GreatCircle::chord_distance(2*radius)/2;
		return angle * radius_prime;
	}
	




	
	double TurnGeneration::Rprime(double TurnRad) { // Rad is turn radius (gc dist)
		return GreatCircle::spherical_earth_radius*std::sin(TurnRad/GreatCircle::spherical_earth_radius);
	}

	int TurnGeneration::turnDirection(const Vect3& p, const Vect3& q, const Vect3& r) {
		return -Util::sign(r.dot(p.cross(q)));
	}


	Vect3 TurnGeneration::normalvect(const Vect3& p, const Vect3& q, const Vect3& r){
		if (r.dot(p.cross(q))>0) {
			return (p.cross(q)).Hat();
		}
		return ((p.cross(q)).Hat()).Scal(-1);
	}

	Vect3 TurnGeneration::point_on_line(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r){ // Rad is turn radius (gc dist)
		double ERad = GreatCircle::spherical_earth_radius;
		Vect3 n1 = normalvect(p,q,r);
		Vect3 n2 = normalvect(r,q,p);
		Vect3 n1xn2 = n1.cross(n2);
		double Rp = Rprime(TurnRad);
		if (std::abs(n1xn2.x)>=Util::max(std::abs(n1xn2.y),std::abs(n1xn2.z))) {
			double a=n1.y;
			double b=n1.z;
			double c=n2.y;
			double d=n2.z;
			double e=Rp-n1.x*ERad;
			double f=Rp-n2.x*ERad;
			double det = n1xn2.x;
			return Vect3(ERad,(d*e-b*f)/det,(-c*e+a*f)/det);
		} else if (std::abs(n1xn2.y)>std::abs(n1xn2.z)) {
			double a=n1.x;
			double b=n1.z;
			double c=n2.x;
			double d=n2.z;
			double e=Rp-n1.y*ERad;
			double f=Rp-n2.y*ERad;
			double det = -n1xn2.y;
			return Vect3((d*e-b*f)/det,ERad,(-c*e+a*f)/det);
		} else {
			double a=n1.x;
			double b=n1.y;
			double c=n2.x;
			double d=n2.y;
			double e=Rp-n1.z*ERad;
			double f=Rp-n1.z*ERad;
			double det=n1xn2.z;
			return Vect3((d*e-b*f)/det,(-c*e+a*f)/det,ERad);
		}
	}

	 Vect3 TurnGeneration::turnCenterxyz(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r) {
		double ERad = GreatCircle::spherical_earth_radius;
		Vect3 pt = point_on_line(TurnRad,p,q,r);
		Vect3 n1 = normalvect(p,q,r);
		Vect3 n2 = normalvect(r,q,p);
		Vect3 n1xn2 = n1.cross(n2);
		Vect3 n;
		if (p.dot(n1xn2)>0) {
			n=n1xn2;
		} else {
			n=n1xn2.Scal(-1);
		}
		double t = (-(pt.dot(n))+Util::sqrt_safe(Util::sq(pt.dot(n))-n.sqv()*(pt.sqv()-Util::sq(ERad))))/(n.sqv());
		return pt.AddScal(t,n);	
	}

	 Vect3 TurnGeneration::BOT(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r) {
		double ERad = GreatCircle::spherical_earth_radius;
		Vect3 c = turnCenterxyz(TurnRad,p,q,r);
		Vect3 n1 = normalvect(p,q,r);
		Vect3 depBOT = c.Add(n1.Scal(-Rprime(TurnRad)));
		double depBOTnorm = depBOT.norm();
		return depBOT.Scal(ERad/depBOTnorm);
	}

	 Vect3 TurnGeneration::EOT(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r) {
		double ERad = GreatCircle::spherical_earth_radius;
		Vect3 c = turnCenterxyz(TurnRad,p,q,r);
		Vect3 n2 = normalvect(r,q,p);
		Vect3 depEOT = c.Add(n2.Scal(-Rprime(TurnRad)));
		double depEOTnorm = depEOT.norm();
		return depEOT.Scal(ERad/depEOTnorm);
	}
	
	
	Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> TurnGeneration::turnGeneratorLLA_Alt3(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius) {
		Vect3 p1e = GreatCircle::spherical2xyz(p1.lat(), p1.lon());
		Vect3 p2e = GreatCircle::spherical2xyz(p2.lat(), p2.lon());
		Vect3 p3e = GreatCircle::spherical2xyz(p3.lat(), p3.lon());
		
		double ERad = GreatCircle::spherical_earth_radius;
		double turnRad = std::abs(radius);
		double Rp = Rprime(turnRad);
		Vect3 c = turnCenterxyz(turnRad,p1e,p2e,p3e);
		Vect3 n1 = normalvect(p1e,p2e,p3e);
		Vect3 n2 = normalvect(p3e,p2e,p1e);
		Vect3 depBOT = c.Add(n1.Scal(-Rp));
		double depBOTnorm = depBOT.norm();
		Vect3 depEOT = c.Add(n2.Scal(-Rp));
		double depEOTnorm = depEOT.norm();
		double Fac = Util::sqrt_safe(1-std::pow(Rp/ERad,2));
		Vect3 dirVect = p2e.AddScal(-(c.Hat2D().dot(p2e.Hat2D())), c);

		Vect3 bote = depBOT.Scal(ERad/depBOTnorm);
		Vect3 eote = depEOT.Scal(ERad/depEOTnorm);
		Vect3 mote = c.Scal(Fac).AddScal(Rp, dirVect.Hat2D());
		int dir = turnDirection(p1e,p2e,p3e);
		double cordLen = (bote.Sub(eote)).norm()/Rp;
		double turnAng = 2*std::asin(cordLen/2);
		double distance = Rp*turnAng;



		LatLonAlt bot = GreatCircle::xyz2spherical(bote);
		LatLonAlt mot = GreatCircle::xyz2spherical(mote);
		LatLonAlt eot = GreatCircle::xyz2spherical(eote);

		return Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double>(bot,mot,eot,dir,distance);
	}
		

}
