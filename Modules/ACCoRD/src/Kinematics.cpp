/* Kinematics
 *
 * Authors:  Ricky Butler              NASA Langley Research Center
 *           George Hagen              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *           Cesar Munoz               NASA Langley Research Center
 *           Anthony Narkawicz         NASA Langley Research Center
 *           Aaron Dutle               NASA Langley Research Center
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Triple.h"
#include "Velocity.h"
#include "Kinematics.h"
#include "StateVector.h"
#include "Constants.h"
#include "VectFuns.h"
#include "Quad.h"
#include "Tuple5.h"
#include "format.h"
#include <cmath>
#include <float.h>


namespace larcfm {

double Kinematics::dirSign(bool turnRight) {
	return (turnRight?1.0:-1.0);
}

double Kinematics::turnRadius(double speed, double bank, double g) {
	double abank = std::abs(bank);
	if ( g <= 0.0 ) {
		g = Units::gn; // I could flag this as an error, but I will assume sea-level gravity.
	}
	if ( abank >= M_PI/2.0 ) {
		return 0.0;
	}
	if ( abank == 0.0 ) {
		return MAXDOUBLE;
	}
	return speed*speed/(g*std::tan(abank));
}

double Kinematics::turnRadius(double speed, double bank) {
	return Kinematics::turnRadius(speed, bank, Units::gn);
}


double Kinematics::turnRadiusByRate(double speed, double omega) {
	  if (Util::almost_equals(omega,0.0)) return MAXDOUBLE;
	  return std::abs(speed/omega);
 }


double Kinematics::speedOfTurn(double R, double bank) {
	double abank = std::abs(bank);
	if ( abank >= M_PI/2) {
		fpln("Kinematics::speedOfTurn: BANK ANGLE TOO LARGE! "+Units::str("deg",bank));
		return NaN;
	}
	return std::sqrt(Units::gn*std::tan(abank)*R);
}

double Kinematics::turnRate(double speed, double bankAngle) {
	if (Util::almost_equals(bankAngle,0)) return 0.0;
	//return Util::sign(bank)*speed/turnRadius(speed,bank);
	return Units::gn*std::tan(bankAngle)/speed;
}


double Kinematics::bankAngleRadius(double speed, double R) {
	if (R <= 0.0) return 0.0;
	return Util::atan2_safe(speed*speed,(R*Units::gn));
}

double Kinematics::bankAngleRadius(double speed, double R, bool turnRight) {
	return dirSign(turnRight)*bankAngleRadius(speed, R);
}

double Kinematics::bankAngleGoal(double track, double goalTrack, double maxBank) {
	return dirSign(Util::clockwise(track, goalTrack))*maxBank;
}

double Kinematics::bankAngle(double speed, double turnRate) {
	double tanb = turnRate*speed/Units::gn;
	//if (std::abs(tanb) >= M_PI/2) return -1;
	double b = std::atan(tanb);
	return b;
}

double Kinematics::turnRateGoal(const Velocity& vo, double goalTrack, double maxBank) {
	double bankAngle = bankAngleGoal(vo.trk(), goalTrack, maxBank);
	return turnRate(vo.gs(), bankAngle);
}


bool Kinematics::turnDone(double currentTrack, double targetTrack, bool turnRight) {
	if (Util::turnDelta(currentTrack,targetTrack) < 0.0001) return true;
	if (turnRight) return !Util::clockwise(currentTrack,targetTrack);
	else return Util::clockwise(currentTrack,targetTrack);
}

double Kinematics::turnTime(const Velocity& v0, double goalTrack, double maxBank, bool turnRight) {
	double deltaTrk = Util::turnDelta(v0.trk(), goalTrack, turnRight);
	return turnTime(v0.gs(), deltaTrk, maxBank);
}

double Kinematics::turnTime(const Velocity& v0, double goalTrack, double maxBank) {
	double deltaAng = Util::signedTurnDelta(v0.trk(), goalTrack);
	return turnTime(v0.gs(), deltaAng, maxBank);
}


double Kinematics::turnTime(double groundSpeed, double deltaTrack, double bankAngle) {
	double omega = Kinematics::turnRate(groundSpeed, bankAngle);
	if (omega == 0.0) return MAXDOUBLE;
	return std::abs(deltaTrack/omega);
}

double Kinematics::turnTime(double deltaTrack, double omega) {
	if (omega == 0) return MAXDOUBLE;
	return std::abs(deltaTrack/omega);
}

bool Kinematics::turnRight(const Velocity& v0, double goalTrack) {
	return Util::clockwise(v0.trk(),goalTrack);
}


//Vect3 Kinematics::turnPos(const Vect3& s0, const Velocity& vo, double t, double R, bool turnRight) {
//	int dir = -1;
//	if (turnRight) dir = 1;
//	double theta = vo.compassAngle();
//	double omega = dir*vo.gs()/R;
//	double xT = s0.x + dir*R*(std::cos(theta) - std::cos(omega*t+theta));
//	double yT = s0.y - dir*R*(std::sin(theta) - std::sin(omega*t+theta));
//	double zT = s0.z + vo.z*t;
//	return Vect3(xT,yT,zT);
//}
//
//Velocity Kinematics::turnVel(const Velocity& vo, double t, double R, bool turnRight) {
//	int dir = -1;
//	if (turnRight) dir = 1;
//	double omega = vo.gs()/R;
//	double del = dir*omega*t;
//	return Velocity::mkTrkGsVs(vo.trk()+del,vo.gs(),vo.vs());
//}


std::pair<Vect3,Velocity> Kinematics::linear(const std::pair<Vect3,Velocity>& sv0, double t) {
	Vect3 s0 = sv0.first;
	Velocity v0 = sv0.second;
	return std::pair<Vect3,Velocity>(s0.linear(v0,t),v0);
}

std::pair<Vect3,Velocity> Kinematics::linear(Vect3 so, Velocity vo, double t) {
	return std::pair<Vect3,Velocity>(so.linear(vo,t),vo);
}

std::pair<Vect3,Velocity> Kinematics::turnOmega(const Vect3& s0, const Velocity& v0, double t, double omega) {
	if (Util::almost_equals(omega,0))
		return std::pair<Vect3,Velocity>(s0.linear(v0,t),v0);
	//	const Vect3& s0 = sv0.first;
	//	const Velocity& v0 = sv0.second;
	//	double v = v0.gs();
	//	double theta = v0.trk();
	//	double xT = s0.x + (v/omega)*(std::cos(theta) - std::cos(omega*t+theta));
	//	double yT = s0.y - (v/omega)*(std::sin(theta) - std::sin(omega*t+theta));
	//	double zT = s0.z + v0.z*t;
	//	Vect3 ns = Vect3(xT,yT,zT);
	//	Velocity nv = Velocity::mkTrkGsVs(v0.trk()+ omega*t,v0.gs(),v0.vs());
	//
	// New implementation avoids calculating track and groundspeed,
	// reduces trig functions to one sine and one cosine.
	Velocity nv = v0.mkAddTrk(omega*t);
	double xT = s0.x + (v0.y-nv.y)/omega;
	double yT = s0.y + (-v0.x+nv.x)/omega;
	double zT = s0.z + v0.z*t;
	Vect3 ns = Vect3(xT,yT,zT);
	return std::pair<Vect3,Velocity>(ns,nv);
}

Vect2 Kinematics::center(const Vect3& s0, const Velocity& v0, double omega) {
	double v = v0.gs();
	double theta = v0.trk();
	double R = v/omega;
	return Vect2(s0.x + R*std::cos(theta),s0.y - R*std::sin(theta));
}

///**   *** EXPERIMENTAL ***
// * Position/Velocity
// * @param s0          starting position
// * @param center
// * @param d           distance into turn
// * @param gsAt_d
// * @return Position/Velocity after t time
// */
//std::pair<Vect3,Velocity> Kinematics::turnByDist(const Vect3& s0, const Vect3& center, int dir, double d, double gsAt_d) {
//	  double R = s0.distanceH(center);
//	  //f.pln(" $$$$$ turnByDist: R = "+Units.str("nm",R));
//	  //double omega = Util.sign(d)*gsAt_d/R;
//	  double dt = std::abs(d/gsAt_d);
//	  Velocity vPerp = Velocity::make(s0.Sub(center));
//	  //int dir = Util::sign(d);
//	  double currentTrk = vPerp.trk()+dir*M_PI/2;
//	  Velocity vo = Velocity::mkTrkGsVs( currentTrk , gsAt_d ,0.0);
//	  //f.pln(" $$$$$ turnByDist: vo = "+vo+" dt = "+dt+" currentTrk = "+Units.str("deg",currentTrk));
//	  return turn(s0, vo, dt, R, dir > 0);
//	  //return turnOmega(s0,vo,dt,omega);
//}


/**
 * Position/Velocity
 * @param s0          starting position
 * @param center
 * @param d           distance into turn
 * @param gsAt_d
 * @return Position/Velocity after t time
 */
std::pair<Vect3,Velocity> Kinematics::turnByDist2D(const Vect3& so, const Vect3& center, int dir, double d, double gsAt_d) {
	//f.pln(" $$$$ turnByDist: so = "+so+" center = "+center);
    //double R = GreatCircle.distance(so, center);
    double R = so.distanceH(center);
    if (R==0.0) return std::pair<Vect3,Velocity>(so,Velocity::INVALID());
	double alpha = dir*d/R;
	//double vFinalTrk = GreatCircle.initial_course(center,so);
	double trkFromCenter = Velocity::mkVel(center, so, 100.0).trk();
	double nTrk = trkFromCenter + alpha;
	//Vect3 sn = GreatCircle.linear_initial(center, nTrk, R);
	Vect3 sn = center.linearByDist2D(nTrk, R);
	//f.pln(" $$$$ turnByDist: sn = "+sn);
	sn = sn.mkZ(0.0);
	//double final_course = GreatCircle.final_course(center,sn);
	//f.pln(" $$ d = "+d+" final_course = "+final_course+" nTrk = "+nTrk);
	double finalTrk = nTrk + dir*M_PI/2;
    Velocity vn = Velocity::mkTrkGsVs(finalTrk,gsAt_d,0.0);
	//double finalTrk = vo.trk()+alpha;
	//double finalTrk = final_course + Util.sign(d)*Math.PI/2;
	return std::pair<Vect3,Velocity>(sn,vn);

}



std::pair<Vect3,Velocity> Kinematics::turnOmega(const std::pair<Vect3,Velocity>& sv0, double t, double omega) {
	if (Util::almost_equals(omega,0))
		return  linear(sv0, t);
	Vect3 s0 = sv0.first;
	Velocity v0 = sv0.second;
	return turnOmega(s0,v0,t,omega);
}

std::pair<Vect3,Velocity> Kinematics::turn(const Vect3& s0, const Velocity& v0, double t, double R,  bool turnRight) {
	if (Util::almost_equals(R,0)) {
		return std::pair<Vect3,Velocity>(s0,v0);
	}
	int dir = -1;
	if (turnRight) dir = 1;
	double omega = dir*v0.gs()/R;
	return turnOmega(s0,v0,t,omega);
}


std::pair<Vect3,Velocity> Kinematics::turn(const std::pair<Vect3,Velocity>& sv0, double t, double R,  bool turnRight) {
	return turn(sv0.first,sv0.second,t,R,turnRight);
}

std::pair<Vect3,Velocity> Kinematics::turn(const Vect3& s0, const Velocity& v0, double t, double bank) {
	if (Util::almost_equals(bank,0)) {
		return std::pair<Vect3,Velocity>(s0.linear(v0,t),v0);
	} else {
		double R = turnRadius(v0.gs(),bank);
		bool turnRight = (bank >= 0);
		return turn(s0,v0,t,R,turnRight);
	}
}

std::pair<Vect3,Velocity>  Kinematics::turnUntilTimeRadius(std::pair<Vect3,Velocity> svo, double t, double turnTime, double R, bool turnRight) {
	  std::pair<Vect3,Velocity> tPair;
	  if (t <= turnTime) {
		  tPair = Kinematics::turn(svo, t, R, turnRight);
	  } else {
		  tPair = Kinematics::turn(svo, turnTime, R, turnRight);
		  tPair = linear(tPair,t-turnTime);
	  }
	  return tPair;
}


std::pair<Vect3,Velocity> Kinematics::turnUntilTimeOmega(const Vect3& so, const Velocity& vo, double t, double turnTime, double omega) {
	return turnUntilTimeOmega(std::pair<Vect3,Velocity>(so, vo), t, turnTime, omega);
}

std::pair<Vect3,Velocity> Kinematics::turnUntilTimeOmega(const std::pair<Vect3,Velocity>& svo, double t, double turnTime, double omega) {
	std::pair<Vect3,Velocity> tPair;
	if (t <= turnTime) {
		tPair = Kinematics::turnOmega(svo, t, omega);
	} else {
		tPair = Kinematics::turnOmega(svo, turnTime, omega);
		tPair = linear(tPair,t-turnTime);
	}
	return tPair;
}

std::pair<Vect3,Velocity> Kinematics::turnUntil(const std::pair<Vect3,Velocity>& svo, double t, double goalTrack, double maxBank) {
	double omega = turnRateGoal(svo.second, goalTrack, maxBank);
	double turnTime = Kinematics::turnTime(svo.second, goalTrack, maxBank);

	return turnUntilTimeOmega(svo,t,turnTime,omega);
	//
	//	bool turnRight = Util::clockwise(vo.trk(),goalTrack);
	//	double R = Kinematics::turnRadius(vo.gs(), maxBank);
	//	double turnTm = turnTime(vo,goalTrack,maxBank,turnRight);
	//	Vect3 soAfterTurn = Kinematics::turn(so, vo, turnTm, R, turnRight).first;
	//	Velocity nv;
	//	if (t <= turnTm) {
	//		std::pair<Vect3,Velocity> np = Kinematics::turn(so, vo, t, R, turnRight).first;
	//		soAfterTurn = np.first;
	//		nv = np.second;
	//	} else {
	//		Velocity nvo = turnVel(vo, turnTm, R, turnRight);
	//		soAfterTurn = soAfterTurn.linear(nvo,t-turnTm);
	//		nv = vo.mkTrk(goalTrack);
	//	}
	//	return std::pair<Vect3,Velocity>(soAfterTurn,nv);
}

std::pair<Vect3,Velocity> Kinematics::turnUntil(const Vect3& so, const Velocity& vo, double t, double goalTrack, double maxBank) {
	return turnUntil(std::pair<Vect3,Velocity>(so, vo), t, goalTrack, maxBank);
}



Vect3 Kinematics::positionAfterTurn(const Vect3& so, const Velocity& vo, double goalTrack, double maxBank) {
	//	bool turnRight = Util::clockwise(vo.trk(),goalTrack);
	//	double turnTm = Kinematics::turnTime(vo, goalTrack, maxBank, turnRight);
	//	double R = Kinematics::turnRadius(vo.gs(), maxBank);
	//	Vect3 return1 = Kinematics::turn(so, vo, turnTm, R, turnRight).first;

	double omega = turnRateGoal(vo, goalTrack, maxBank);
	double turnTime = Kinematics::turnTime(vo, goalTrack, maxBank);
	return turnOmega(std::pair<Vect3,Velocity>(so,vo), turnTime, omega).first;
}


double Kinematics::closestTimeOnTurn(const Vect3& s0, const Velocity& v0, double omega, const Vect3& x, double endTime) {
	Vect2 center2 = center(s0,v0,omega);
	if (x.vect2().almostEquals(center2)) return -1.0;
	double trk1 = s0.vect2().Sub(center2).trk();
	double trk2 = x.vect2().Sub(center2).trk();
	double delta = Util::turnDelta(trk1, trk2, Util::sign(omega));
	double t = std::abs(delta/omega);
	if (endTime > 0 && (t < 0 || t > endTime)) {
		double maxTime = 2*M_PI/std::abs(omega);
		if (t > (maxTime + endTime) / 2) {
			return 0.0;
		} else {
			return endTime;
		}
	}
	return t;
}

double Kinematics::closestDistOnTurn(const Vect3& s0, const Velocity& v0, double R, int dir, const Vect3& x, double maxDist) {
	Vect2 center = centerOfTurn(s0.vect2(), v0.vect2(), R, dir);
	if (x.vect2().almostEquals(center)) return -1.0;
	double trk1 = s0.vect2().Sub(center).trk();
	double trk2 = x.vect2().Sub(center).trk();
	double delta = Util::turnDelta(trk1, trk2, dir);
	double d = delta*R;
	if (maxDist > 0 && (d < 0 || d > maxDist)) {
		double maxD = 2*M_PI*R;
		if (d > (maxD + maxDist) / 2) {
			return 0.0;
		} else {
			return maxDist;
		}
	}
	return d;
}


/** Q function from ACCoRD.TangentLine. Returns the point on the circle (with 0,0 origin)  that is tangent with the outside point
 * @param s vertex point
 * @param D radius of the circle
 * @param eps direction of turn (+1 is turn LEFT, -1 is turn RIGHT in he absolute frame)
 * @return tangent point on the circle, or an INVALID vector if the vertex is within the circle
 */
Vect2 Q(const Vect2& s, double D, int eps) {
	double sq_s = s.sqv();
	double sq_D = Util::sq(D);
	double delta = sq_s -sq_D;
	if (delta >= 0) {
		double alpha = sq_D/sq_s;
		double beta  = D*Util::sqrt_safe(delta)/sq_s;
		return Vect2(alpha*s.x+eps*beta*s.y,
				alpha*s.y-eps*beta*s.x);
	}
	return Vect2::INVALID();
}


/**
 * true iff the trajectory following "from" to "to" is a left turn
 * "from" is your current vector of travel, "to" is your goal vector of travel
 */
bool isLeftTurn(const Vect2& from, const Vect2& to) {
	double detv = to.det(from);
	return (detv < 0);
}

/**
 * Converts a "true" to a +1 value, a "false" to a -1
 */
int boolean2eps(bool b) {
	if (b) return 1; else return -1;
}


std::pair<Vect2,Vect2> Kinematics::directTo(const Vect2& bot, const Vect2& v0, const Vect2& goal, double R) {
	//f.pln("@@@ p0 = "+f.sStr(p0)+"  v0 = "+f.vStr(v0)+" np = "+f.sStr(np)+"  R = "+Units.str("nm",R));
	Vect2 newV = goal.Sub(bot);					// vector to goal
	int eps = boolean2eps(isLeftTurn(v0,newV));	  // left = 1, right = -1
	Vect2 vperp;
	if (eps > 0)    // Turn left
		vperp = v0.PerpL().Hat();    // unit velocity vector (perpendicular to initial velocity)
	else
		vperp = v0.PerpR().Hat();    // unit velocity vector (perpendicular to initial velocity)
	Vect2 center = bot.Add(vperp.Scal(R));		// center of turn
	//f.pln("%% center = "+f.sStr(center)+" eps = "+eps);
	// Shift coordinate system so that center is located at (0,0) Use ACCoRD tangent point Q calculation
	Vect2 s = goal.Sub(center);					// from center to goal
	Vect2 rop = Q(s,R,eps);						// tangent in relative frame (wrt center of circle)
	Vect2 EOT = rop.Add(center);				// return from relative (translate tangent point back to absolute frame)
	//		if (EOT.isInvalid())
	//			f.pln(" ERROR in Kinematics.directTo: attempt to perform directTo to point inside turn radius!");
	return std::pair<Vect2,Vect2>(EOT,center);
}

Quad<Vect3,Velocity,double,int> Kinematics::directToPoint(const Vect3& so, const Velocity& vo, const Vect3& wp, double R){
	//double R = Math.abs(vo.gs()/omega);
	Vect2 EOT = directTo(so.vect2(),vo.vect2(),wp.vect2(),R).first;
	if (EOT.isInvalid()) return Quad<Vect3,Velocity,double,int>(Vect3::INVALID(), Velocity::INVALIDV(), -1.0, 0);
	double finalTrack = wp.vect2().Sub(EOT).trk();
	//f.pln(" $$ directToPoint: finalTrack = "+Units.str("deg", finalTrack) );
	// this should not be based on final track direction, but rather on the actual turn taken.
	//		double turnDelta = Util.signedTurnDelta(vo.trk(),finalTrack);
	double turnDir = boolean2eps(!isLeftTurn(vo.vect2(),wp.Sub(so).vect2())); // swapped the sign
	double turnDelta = Util::turnDelta(vo.trk(), finalTrack, turnDir > 0);	// angle change in that direction
	double omega = turnDir*vo.gs()/R;
	double turnTime = std::abs(turnDelta/omega);
	std::pair<Vect3,Velocity> p2 = turnOmega(so,vo,turnTime,omega);
	return Quad<Vect3,Velocity,double,int>(p2.first, p2.second, turnTime, (int)turnDir);
}

Triple<Vect3,double,double> Kinematics::genDirectToVertex(const Vect3& so, const Velocity& vo, const Vect3& wp, double bankAngle, double timeBeforeTurn){
	//std::pair<Vect2,Vect2> eot = directTo(Vect2 bot, Vect2 v0, Vect2 goal, double R) {
	Vect3 soPlus = so.Add(vo.Scal(timeBeforeTurn));
	double R = Kinematics::turnRadius(vo.gs(), bankAngle);
	//public Triple<Vect3,Velocity,double> directToPoint(Vect3 soPlus, Velocity vo, Vect3 wp, double R) {
	Quad<Vect3,Velocity,double,int> dtp = directToPoint(soPlus,vo,wp,R);
	if (dtp.third < 0) {
		return Triple<Vect3,double,double>(Vect3::INVALID(), -1.0, -1.0);
	}
	Vect3 si = dtp.first;
	Velocity vi = dtp.second;
	std::pair<Vect3,double> ipPair = VectFuns::intersection(soPlus,vo,si,vi);
	if (ipPair.second != ipPair.second) { // NaN
		return Triple<Vect3,double,double>(Vect3::INVALID(), -1.0, -1.0);
	}
	Vect3 ip = ipPair.first;
	return Triple<Vect3,double,double>(ip,ipPair.second+timeBeforeTurn,dtp.third+timeBeforeTurn);
}



/**
 * find center of turn determined by line (so,vo) with radius R and direction dir
 * @param so
 * @param vo
 * @param R
 * @param dir direction: 1 = right, -1 = left
 * @return
 */
Vect2 Kinematics::centerOfTurn(const Vect2& so, const Vect2& vo, double R, int dir) {
	Vect2 vperp;
	if (dir > 0)    // turn Right
		vperp = vo.PerpR().Hat();    // unit velocity vector
	else
		vperp = vo.PerpL().Hat();    // unit velocity vector
	Vect2 center = so.Add(vperp.Scal(R));
	//fpln("%% center = "+f.sStr(center));
	return center;
}

/**
 * find center of turn determined by line (so,vo) with bankAngle and direction TurnRight
 * @param so
 * @param vo
 * @param R
 * @param dir direction: 1 = right, -1 = left
 * @return
 */
Vect2 Kinematics::centerOfTurn(const Vect2& so, const Vect2& vo, double bankAng, bool turnRight) {
	double R = Kinematics::turnRadius(vo.norm(), bankAng);
	int dir = -1;
	if (turnRight) dir = 1;
	return centerOfTurn(so,vo,R,dir);
}


bool Kinematics::testLoSTrk(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
		double bankAngOwn, bool turnRightOwn, double stopTime, double D, double H) {
	//fpln(" $$$$ testLoSTrk: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	bool rtn = false;
	for (double t = 0; t < stopTime; t = t + step) {
		//Vect3 soAtTm = turnUntilPosition(so, vo, nvo.track(), bankAngOwn, t, turnRightOwn);
		Vect3 soAtTm = turnUntil(so, vo, t, nvo.trk(), bankAngOwn).first;
		Vect3 siAtTm = si.linear(vi,t);
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		if (distH < D && distV< H) rtn =true;
	}
	return rtn;
}






// ****************************** Ground Speed KINEMATIC CALCULATIONS *******************************


Vect3 Kinematics::gsAccelPos(const Vect3& so3, const Velocity& vo3, double tm, double a) {
	Vect2 so = so3.vect2();
	Vect2 vo = vo3.vect2();
	Vect2 sK = so.Add(vo.Hat().Scal(vo.norm()*tm+0.5*a*tm*tm));
	double nz = so3.z + vo3.z*tm;
	return Vect3(sK,nz);
}



std::pair<Vect3,Velocity> Kinematics::gsAccel(const Vect3& so, const Velocity& vo,  double t, double a) {
	double nvoGs = vo.gs() + a*t;
	Velocity nvo = vo.mkGs(nvoGs);
	return std::pair<Vect3,Velocity>(gsAccelPos(so,vo,t,a),nvo);
}


double Kinematics::gsAccelTime(const Velocity& vo,double goalGs, double gsAccel) {
	if (gsAccel < 0) std::cout << " gsAccelTime: gsAccel MUST BE Non-negative!!!! " << std::endl;
	double deltaGs = std::abs(vo.gs() - goalGs);
	if (deltaGs == 0.0) return 0;
	double rtn = deltaGs/gsAccel;
	return rtn;
}


Triple<Vect3,Velocity,double> Kinematics::gsAccelGoal(const Vect3& so, const Velocity& vo, double goalGs, double gsAccel) {
	int sgn = 1;
	if (goalGs < vo.gs()) sgn = -1;
	double accelTime = gsAccelTime(vo, goalGs, gsAccel);
	Vect3 nso = gsAccelPos(so, vo, accelTime, sgn*gsAccel);
	Velocity nvo = vo.mkGs(goalGs);
	return Triple<Vect3,Velocity,double>(nso,nvo,accelTime);
}

//std::pair<Vect3,Velocity> Kinematics::gsAccelUntilRWB(const Vect3& so, const Velocity& vo, double t, double goalGs, double gsAccel) {
//	if (gsAccel < 0 ) {
//		std::cout <<  "Kinematics::gsAccelUntil: user supplied negative gsAccel!!" << std::endl;
//		gsAccel = -gsAccel;                              // make sure user supplies positive value
//	}
//	double accelTime = gsAccelTime(vo,goalGs,gsAccel);
//	int sgn = 1;
//	if (goalGs < vo.gs()) sgn = -1;
//	//Vect3 ns = gsAccelPos(so, vo, accelTime, sgn*gsAccel);
//	double a = sgn*gsAccel;
//	Vect2 so2 = so.vect2();
//	Vect2 vo2 = vo.vect2();
//	Vect2 voHat = vo2.Hat();
//	double voNorm = vo2.norm();
//	Vect2 sK = so2.Add(voHat.Scal(voNorm*accelTime+0.5*a*accelTime*accelTime));
//	double nz = so.z + vo.z*accelTime;
//	Vect3 ns = Vect3(sK,nz);
//	Velocity nv = vo.mkGs(goalGs);
//	if (t <= accelTime) {
//		//ns = gsAccelPos(so, vo, t, sgn*gsAccel);
//		sK = so2.Add(voHat.Scal(voNorm*t+0.5*a*t*t));
//		double nz = so.z + vo.z*t;
//		ns = Vect3(sK,nz);
//		nv = vo.mkGs(vo.gs()+sgn*gsAccel*t);
//	} else {
//		ns = ns.linear(nv,t-accelTime);
//		nv = vo.mkGs(goalGs);
//	}
//	//fpln(">>>> EXIT ns = "+f.sStr(ns));
//	return std::pair<Vect3,Velocity>(ns,nv);
//}

std::pair<Vect3,Velocity> Kinematics::gsAccelUntil(const Vect3& so, const Velocity& vo, double t, double goalGs, double gsAccel_d) {
	if (gsAccel_d < 0 ) {
		std::cout <<  "Kinematics::gsAccelUntil: user supplied negative gsAccel!!" << std::endl;
		gsAccel_d = -gsAccel_d;                              // make sure user supplies positive value
	}
	double accelTime = gsAccelTime(vo,goalGs,gsAccel_d);
	int sgn = 1;
	if (goalGs < vo.gs()) sgn = -1;
	double a = sgn*gsAccel_d;
	std::pair<Vect3,Velocity> nsv = gsAccel( so, vo, accelTime, a);
	if (t <= accelTime) {
	  return gsAccel(so, vo, t, a);
	} else {
	  return gsAccel(nsv.first, nsv.second, t-accelTime, 0);
	}
}



std::pair<Vect3,Velocity> Kinematics::gsAccelUntil(const std::pair<Vect3,Velocity>& sv0, double t, double goalGs, double gsAccel) {
	return gsAccelUntil(sv0.first, sv0.second, t, goalGs, gsAccel);
}

/**
 * Accelerate for a given distance.  Return the end gs and time.  Negative time indicates an error.
 */
std::pair<double,double> Kinematics::gsAccelToDist(double gsIn, double dist, double gsAccel) {
	if (gsIn < 0 || dist < 0 || (gsAccel < 0 && dist < -0.5*gsIn*gsIn/gsAccel)) {
		return std::pair<double,double>(0.0,-1.0);
	}

	double A = 0.5*gsAccel;
	double B = gsIn;
	double C = -dist;

	double ta = (-B+std::sqrt(B*B-4*A*C))/(2*A); // try first root
	double tb = (-B-std::sqrt(B*B-4*A*C))/(2*A);
	double t = -1;
	if (ta >= 0) {
		t = ta;
	} else if (tb >= 0) {
		t = tb;
	}
	return std::pair<double,double>(gsIn+gsAccel*t, t);
}


std::pair<double,double> Kinematics::gsAccelToRTA(double gsIn, double dist, double rta, double gsAccel) {

	double avgGs = dist/rta;
	int sign = 1;
	if (avgGs < gsIn) {
		sign = -1;
	}
	double a = gsAccel*sign;
	double A = 0.5*a;
	double B = -a*rta;
	double C = dist - gsIn*rta;
	double z = B*B-4*A*C;
	if (z < 0.0) {
		return std::pair<double,double>(-1,-1);
	}
	double ta = (-B+sqrt(z))/(2*A); // try first root
	double tb = (-B-sqrt(z))/(2*A);
	double t = -1;
	if (ta < rta && ta > 0) {
		t = ta;
	} else if (tb < rta && tb > 0) {
		t = tb;
	}
	return std::pair<double,double>(gsIn + a*t, t);
}




bool Kinematics::testLoSGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
		double gsAccelOwn, double stopTime, double D, double H) {
	//		fpln(" $$$$ testLoSGs: vo = "+vo.toString()+" vi = "+vi.toString()+"  nvo = "+nvo.toString()+" stopTime="+Fm3(stopTime));
	double step = 1.0;
	bool rtn = false;
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = gsAccelUntil(so, vo, t, nvo.gs(), gsAccelOwn).first;
		Vect3 siAtTm = si.linear(vi,t);
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		if (distH < D && distV < H) rtn =true;
	}
	return rtn;
}



// ****************************** Vertical Speed KINEMATIC CALCULATIONS *******************************

/**
 * Return the elevation angle (alternatively the negative glide-slope angle) for a climb (descent)
 * @return elevation angle [radians]
 */
double Kinematics::elevationAngle(Velocity v) {
	return Util::atan2_safe(v.vs(), v.gs());
}


Vect3 Kinematics::vsAccelPos(const Vect3& so3, const Velocity& vo3, double t, double a) {
	return Vect3(so3.x + t*vo3.x, so3.y + t*vo3.y, so3.z + vo3.z*t + 0.5*a*t*t);
}

std::pair<Vect3,Velocity> Kinematics::vsAccel(const Vect3& so3, const Velocity& vo3,  double t, double a) {
	double nvoVs = vo3.vs() + a*t;
	Velocity nvo = vo3.mkVs(nvoVs);
	return std::pair<Vect3,Velocity>(vsAccelPos(so3,vo3,t,a),nvo);
}

std::pair<Vect3,Velocity> Kinematics::vsAccel(const std::pair<Vect3,Velocity>& sv0,  double t, double a) {
	  return vsAccel(sv0.first, sv0.second,t,a);
}


double Kinematics::vsAccelTime(const Velocity& vo,double goalVs, double vsAccel) {
	return vsAccelTime(vo.vs(),goalVs, vsAccel);;
}

double Kinematics::vsAccelTime(double vs, double goalVs, double vsAccel) {
	double deltaVs = vs - goalVs;
	double rtn = std::abs(deltaVs/vsAccel);
	//f.pln("#### vsAccelTime: vs() = "+Units.str("fpm",vs)+" deltaVs = "+Units.str("fpm",deltaVs)+" rtn = "+rtn);
	return rtn;
}


Triple<Vect3,Velocity,double> Kinematics::vsAccelGoal(const Vect3& so, const Velocity& vo, double goalVs, double vsAccel) {
	int sgn = 1;
	if (goalVs < vo.vs()) sgn = -1;
	double accelTime = vsAccelTime(vo, goalVs, vsAccel);
	Vect3 nso = vsAccelPos(so, vo, accelTime,  sgn*vsAccel);
	Velocity nvo = Velocity::mkVxyz(vo.x,vo.y,goalVs);
	return Triple<Vect3,Velocity,double>(nso,nvo,accelTime);
}


std::pair<Vect3,Velocity> Kinematics::vsAccelUntil(const Vect3& so, const Velocity& vo, double t, double goalVs, double vsAccel_d) {
	if (vsAccel_d < 0 ) {
		std::cout << "Kinematics::vsAccelUntil: user supplied negative vsAccel!!" << std::endl;
		vsAccel_d = -vsAccel_d;                              // make sure user supplies positive value
	}
	double accelTime = vsAccelTime(vo,goalVs, vsAccel_d);
	int sgn = 1;
	if (goalVs < vo.vs()) sgn = -1;
	Vect3 ns = Vect3::ZERO();
	if (t <= accelTime)
		return vsAccel(so,vo,t,sgn*vsAccel_d);
	else {
		Vect3 posEnd = vsAccelPos(so,vo,accelTime,sgn*vsAccel_d);
		Velocity nvo = Velocity::mkVxyz(vo.x,vo.y, goalVs);
		return linear(posEnd,nvo,t-accelTime);
	}
}


std::pair<Vect3,Velocity> Kinematics::vsAccelUntil(const std::pair<Vect3,Velocity>& sv0, double t, double goalVs, double vsAccel) {
	return vsAccelUntil(sv0.first, sv0.second, t, goalVs, vsAccel);
}



bool Kinematics::testLoSVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
		double vsAccelOwn, double stopTime, double D, double H) {
	//fpln(" $$$$ testLoSTrk: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	bool rtn = false;
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = vsAccelUntil(so, vo, t, nvo.vs(), vsAccelOwn).first;
		Vect3 siAtTm = si.linear(vi,t);
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		if (distH < D && distV < H) rtn =true;
	}
	return rtn;
}


//static double antiDer1(double voz, double t, double a1) {   // alpha
//	//fpln(" $$$$ alpha: t ="+t+" a1 = "+a1+" voz = "+Units.str("fpm",voz)+"   return:"+(voz*t + 0.5*a1*t*t));
//	return voz*t + 0.5*a1*t*t;
//}
//
//static double antiDer2(double climbRate, double t) { // beta
//	return  climbRate*t;  // voz*t+ a1*T1*t;
//}
//
//static double antiDer3(double climbRate, double t, double a2) {  // gamma
//	return climbRate*t + 0.5*a2*t*t;
//}
//
////// Computes time for the vsLevelOut method,
//// Note: if T2 < T1, there is no constant vertical speed phase, If T1 < 0, target altitude is not achieveable
////
//// allowClimbRateChange   if true, the climbRate can be reduced, otherwise T1 might be set to -1, which
////                                 indicates failure
//// return <T1 = end of first accel ,T2 = start of constant vertical speed phase, T3 = start of deceleration, climbRate'>
////
//Quad<double,double,double,double> Kinematics::vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//		double a1, double a2, bool allowClimbRateIncrease, bool allowClimbRateDecrease) {
//	Vect3    s0 = sv0.first;
//	Velocity v0 = sv0.second;
//	double soz = s0.z;
//	double voz = v0.z;
//	int altDir = -1;
//	if (targetAlt >= soz) altDir = 1;
//	climbRate = altDir*std::abs(climbRate);
//	int vsDir1 = -1;
//	if (climbRate >= voz) vsDir1 = 1;
//	int vsDir2 = -altDir;
//	a1 = vsDir1*std::abs(a1);
//	a2 = vsDir2*std::abs(a2);
//	//fpln(" vsLevelOutTimes: altDir = "+altDir+" vsDir1 = "+vsDir1);
//	//fpln(" #### vsLevelOutTimes: a = "+a+" climbRate = "+Units.str("fpm",climbRate)+" targetAlt = "+Units.str("ft",targetAlt));
//	double T1 = (climbRate - voz)/a1;
//	double S = (targetAlt-soz);
//	double S1 = antiDer1(voz,T1,a1);
//	double t3 = -climbRate/a2;
//	double S3 = antiDer3(climbRate,t3,a2);
//	double S2 = S - S1 - S3;
//	//fpln(" #### S = "+Units.str("ft",S)+" S1 = "+Units.str("ft",S1)+" S3 = "+Units.str("ft",S3)+" S2 = "+Units.str("ft",S2));
//	double T2 = S2/climbRate + T1;
//	double T3 = -climbRate/a2 + T2;
//	double cc = 0;
//	if (T2 < T1) {
//		//fpln("  vsLevelOutTimes: Case 2: no constant vertical speed phase! T1 = "+T1+" T2 ="+T2);
//		if (allowClimbRateDecrease) {
//			//double aa = a;
//			//double bb = 2*voz;
//			//cc = voz*voz/(2*a) - S;
//			double aa = 0.5*a1*(1 - a1/a2);
//			double bb = voz - (a1/a2)*voz;
//			cc = -voz*voz/(2*a2) - S;
//			double root1 = Util::root(aa,bb,cc,1);
//			double root2 = Util::root(aa,bb,cc,-1);
//			//fpln(" root1 = "+root1+" root2 = "+root2);
//			if (root1 >= 0) T1 = root1;
//			else if (root2 >= 0) T1 = root2;
//			else {
//				fpln(" vsLevelOut: Both roots are negative!  root1 = "+Fm2(root1)+" root2 = "+Fm2(root2));
//				T1 = -1;
//				return Quad<double,double,double,double>(-1.0,-1.0,-1.0,climbRate);
//			}
//			T2 = T1;
//			climbRate = voz + a1*T1;
//			T3 = -climbRate/a2 + T1;
//		} else {
//			T1 = -1;  // FAILURE
//		}
//	}
//	// Deal with special case where current vertical speed already exceeds climbRate (CHANGE climbRate)
//	if (vsDir1 != altDir && allowClimbRateIncrease) {
//		climbRate = voz;  // increase climb rate
//		//fpln(" vsLevelOutTimes: recompute climbRate = "+Units.str("fpm",climbRate) );
//		Quad<double,double,double,double> qTemp = vsLevelOutTimesRWB(sv0, climbRate, targetAlt, a1, a2, false, false);
//		T1 = qTemp.first;
//		T2 = qTemp.second;
//		T3 = qTemp.third;
//	}
//	//fpln(" T1 = "+T1+"  T2 = "+T2+" T3 = "+T3+" t3 = "+t3+" climbRate = "+Units.str("fpm",climbRate));
//	return Quad<double,double,double,double>(T1,T2,T3,climbRate);
//}
//
//Quad<double,double,double,double> Kinematics::vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//		double a, bool allowClimbRateChange) {
//	return vsLevelOutTimesRWB(sv0, climbRate, targetAlt, a, -a, allowClimbRateChange, allowClimbRateChange);
//}
//
//Quad<double,double,double,double> Kinematics::vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//		double a) {
//	return vsLevelOutTimesRWB(sv0, climbRate, targetAlt, a, -a, false, true);
//}
//
//
//double Kinematics::vsLevelOutTimeRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange) {
//	Quad<double,double,double,double> qV = vsLevelOutTimesRWB(sv0,climbRate,targetAlt,a, allowClimbRateChange);
//	if (qV.first < 0) return -1;
//	else return qV.third;
//}
//
///** returns Pair that contains position and velocity at time t due to level out maneuver
// *
// * @param sv0        current position and velocity vectors
// * @param t          time point of interest
// * @param T1         end of first accel
// * @param T2         end of constant vertical speed phase
// * @param T3         end of deceleration (Level after this)
// * @param climbRate  climb rate
// * @param targetAlt  target altitude
// * @param a1         first acceleration
// * @param a2         second acceleration
// * @return
// */
//std::pair<Vect3, Velocity> Kinematics::vsLevelOutCalculationRWB(const std::pair<Vect3,Velocity>& sv0, double t,  double T1, double T2,
//		double T3, double climbRate, double targetAlt, double a1, double a2) {
//	Vect3    s0 = sv0.first;
//	Velocity v0 = sv0.second;
//	//fpln(f.Fm1(t)+"  #### vsLevelOutCalculation: s0 = "+f.sStr(s0)+" v0 = "+v0+" a = "+a+" climbRate = "+Units.str("fpm",climbRate)+" targetAlt = "+Units.str("ft",targetAlt));
//	//fpln(f.Fm1(t)+"  #### vsLevelOutCalculation: T1 ="+T1+" T2 = "+T2+" climbRate = "+Units.str("fpm",climbRate)+" targetAlt = "+Units.str("ft",targetAlt));
//	double soz = s0.z;
//	double voz = v0.z;
//	int altDir = -1;
//	if (targetAlt >= soz) altDir = 1;
//	climbRate = altDir*std::abs(climbRate);
//	int vsDir1 = -1;
//	if (climbRate >= voz) vsDir1 = 1;
//	int vsDir2 = -altDir;
//	a1 = vsDir1*std::abs(a1);
//	a2 = vsDir2*std::abs(a2);
//	Velocity nv = Velocity::ZEROV;
//	Vect3    ns = Vect3::ZERO();
//	if (t <= T1) {
//		nv = v0.mkVs(voz + a1*t);
//		ns = s0.linear(v0,t).mkZ(soz + antiDer1(voz,t,a1));
//		//fpln(t+" T<=T1: vsDir1 = "+vsDir1+" soz = "+Units.str("ft",soz)+" antiDer1(voz,t,a1) = "+Units.str("ft",antiDer1(voz,t,a1)));
//	} else if (t <= T2) {
//		nv = v0.mkVs(climbRate);
//		//fpln(t+" T<=T2: soz = "+Units.str("ft",soz)+" antiDer1(voz,T1,a1) = "+Units.str("ft",antiDer1(voz,T1,a1))+" antiDer2(climbRate,t-T1) = "+Units.str("ft",antiDer2(climbRate,t-T1)));
//		ns = s0.linear(v0,t).mkZ(soz + antiDer1(voz,T1,a1)+antiDer2(climbRate,t-T1));
//	} else if (t <= T3) {
//		nv = v0.mkVs(climbRate + a2*(t-T2));
//		//fpln("t<=T3: soz = "+Units.str("ft",soz)+" alpha(voz,T1,a) = "+Units.str("ft",antiDer1(voz,T1,a))
//		//		+" beta(climbRate,T2-T1,T1,a) = "+Units.str("ft",antiDer2(climbRate,t-T1,a))+" gamma(voz,t-T2,T1,T2,a) = "+Units.str("ft",antiDer3(voz,t-T2,a)));
//		ns = s0.linear(v0,t).mkZ(soz + antiDer1(voz,T1,a1)+ antiDer2(climbRate,T2-T1) +antiDer3(climbRate,t-T2,a2));
//	} else {
//		nv = v0.mkVs(0);
//		ns = s0.linear(v0,t).mkZ(soz + antiDer1(voz,T1,a1)+ antiDer2(climbRate,T2-T1) + antiDer3(climbRate,T3-T2,a2));
//	}
//	//fpln(f.Fm1(t)+"  #### vsLevelOutCalculation: vsDir = "+vsDir+" T2 = "+T2+"  ns = "+f.sStr(ns));
//	return std::pair<Vect3, Velocity>(ns,nv);
//}
//
//
//std::pair<Vect3,Velocity> Kinematics::vsLevelOutRWB(const std::pair<Vect3,Velocity>& sv0, double t, double climbRate, double targetAlt, double a, bool allowClimbRateChange) {
//	Quad<double,double,double,double> qV =  vsLevelOutTimesRWB(sv0,climbRate,targetAlt,a, allowClimbRateChange);
//	double T1 = qV.first;
//	double T2 = qV.second;
//	double T3 = qV.third;
//	if (T1 < 0) {
//		return std::pair<Vect3,Velocity>(Vect3::INVALID,Velocity::INVALIDV);
//	}
//	return vsLevelOutCalculationRWB(sv0, t, T1, T2, T3, qV.fourth, targetAlt, a,-a);
//}
//
//StateVector Kinematics::vsLevelOutFinalRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange) {
//	Quad<double,double,double,double> qV =  vsLevelOutTimesRWB(sv0,climbRate,targetAlt,a, allowClimbRateChange);
//	double T1 = qV.first;
//	double T3 = qV.third;
//	if (T1 < 0) {         //  overshoot case
//		return StateVector(Vect3::INVALID,Velocity::INVALIDV,-1.0);
//	}
//	return StateVector(vsLevelOutCalculationRWB(sv0, qV.third, T1, qV.second, qV.third, qV.fourth,targetAlt,a,-a), T3);
//}
//
//
///** This version prioritizes being able to reach an altitude at all and then achieving the specified climb rate.
// *  The returned Statevector contains position,velocity, and time to reach target altitude.
// * @param sv0
// * @param climbRate
// * @param targetAlt
// * @param a
// * @return
// */
//StateVector Kinematics::vsLevelOutFinalRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a) {
//	Quad<double,double,double,double> qV =  vsLevelOutTimesRWB(sv0,climbRate,targetAlt,a);
//	double T1 = qV.first;
//	double T3 = qV.third;
//	if (T1 < 0) {         //  overshoot case
//		return StateVector(Vect3::INVALID,Velocity::INVALIDV,-1.0);
//	}
//	return StateVector(vsLevelOutCalculationRWB(sv0, qV.third, T1, qV.second, qV.third, qV.fourth,targetAlt,a,-a), T3);
//}
//
//double Kinematics::vsLevelOutClimbRateRWB(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
//		double a1, double a2, bool allowClimbRateChange) {
//	Quad<double,double,double,double> ntp = vsLevelOutTimesRWB(svo, climbRate, targetAlt, a1, a2, false, false);
//	//fpln(" $$$ vsLevelOutTimes: "+ntp.first+" "+ ntp.second+" "+ ntp.third+" "+ntp.fourth+" "+ntp.fifth);
//	return  vsLevelOutCalculationRWB(svo, ntp.first, ntp.first, ntp.second, ntp.third, ntp.fourth, targetAlt, a1, a2).second.z;
//}


double V1(double voz, double a1, double t) {   // alpha
	//fpln(" $$$$ alpha: t ="+t+" a = "+a+" voz = "+Units::str("fpm",voz)+"   return:"+(voz*t + 0.5*a*t*t));
	return voz + a1*t;
}

double S1(double voz, double a1, double t) {   // alpha
	//fpln(" $$$$ alpha: t ="+t+" a = "+a+" voz = "+Units::str("fpm",voz)+"   return:"+(voz*t + 0.5*a*t*t));
	return voz*t + 0.5*a1*t*t;
}

double T3(double voz, double a1) {   // alpha
	//fpln(" $$$$ alpha: t ="+t+" a = "+a+" voz = "+Units::str("fpm",voz)+"   return:"+(voz*t + 0.5*a*t*t));
	return -voz/a1;
}

double S3(double voz, double a1) {   // alpha
	//fpln(" $$$$ alpha: t ="+t+" a = "+a+" voz = "+Units::str("fpm",voz)+"   return:"+(voz*t + 0.5*a*t*t));
	return S1(voz, a1, T3(voz, a1));
}


/** Returns a statevector that holds position, velocity and relative time at final level out position
 *
 * @param sv0
 * @param climbRate
 * @param targetAlt
 * @param a
 * @param allowClimbRateChange
 * @return
 */
StateVector Kinematics::vsLevelOutFinal(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange) {
	Tuple5<double, double,double,double,double> qV =  vsLevelOutTimes(sv0,climbRate,targetAlt,a, allowClimbRateChange);
	double T1 = qV.first;
	double T3 = qV.third;
	if (T1 < 0) {         //  overshoot case
		//fpln(" $$$$$$ vsLevelOutFinal: T1 < 0,      targetAlt = "+Units::str("ft",targetAlt)+" currentAlt = "+Units::str("ft",sv0.first.z()));
		return StateVector(Vect3::INVALID(),Velocity::INVALIDV(),-1.0);
	}
	return StateVector(vsLevelOutCalculation(sv0, targetAlt, qV.fourth, qV.fifth, T1, qV.second, T3, T3),T3);
}

StateVector Kinematics::vsLevelOutFinal(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a) {
	Tuple5<double,double,double,double,double> qV =  vsLevelOutTimes(sv0,climbRate,targetAlt,a);
	double T1 = qV.first;
	double T3 = qV.third;
	if (T1 < 0) {         //  overshoot case
		//f.pln(" $$$$$$ vsLevelOutFinal: T1 < 0,      targetAlt = "+Units.str("ft",targetAlt)+" currentAlt = "+Units.str("ft",sv0.first.z()));
		return StateVector(Vect3::INVALID(),Velocity::INVALIDV(),-1.0);
	}
	return StateVector(vsLevelOutCalculation(sv0, targetAlt, qV.fourth, qV.fifth, T1, qV.second, T3, T3),T3);
}


bool Kinematics::overShoot(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt, double accelup,
		double acceldown, bool allowClimbRateChange){
	double a2 = vsLevelOutTimes(svo, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange).fifth;
	int sgnv = -1;
	if (svo.second.z>=0) sgnv =1;
	int altDir = -1;
	if (targetAlt-svo.first.z>=0) altDir = 1;
	if (sgnv==altDir && std::abs(targetAlt-svo.first.z)< std::abs(S3(svo.second.z, a2))) return true;
	else return false;
}


///**
// * Returns true if time t is within the constant velocity segment of the climb
// * All values are in internal units
// */
//bool Kinematics::inConstantClimb(double sz, double vz, double t, double climbRate, double targetAlt, double a) {
//	std::pair<Vect3,Velocity> sv0 = std::pair<Vect3,Velocity>(Vect3::mkXYZ(0,0,sz),Velocity::mkVxyz(0, 0, vz));
//	Quad<double,double,double,double> qV =  vsLevelOutTimesRWB(sv0,climbRate,targetAlt,a,true);
//	return t > qV.first && t < qV.second;
//}
//


/** Helper function for vsLevelOutTimesAD.
 *  Note: This could be integrated into the function vsLevelOutTimesAD as a recursive call if desired.
 *
 * @param s0z          initial vertical position
 * @param v0z		   initial vertical velocity
 * @param climbRate    desired vertical speed for the climb/descent (positive), sign calculated in code
 * @param targetAlt    target altitude
 * @param accelup      maximum positive acceleration
 * @param acceldown    maximum negative acceleration
 * @param allowClimbRateChange	if true, if the current velocity is of greater magnitude than the specified climb rate,
 * 										then continue at the current velocity (prioritize achieving the desired altitude).
 * 										If false, first achieve the goal climb rate (prioritize achieving the indicated vs)
 *
 *
 * @return <T1 = end of first accel ,T2 = end of constant vertical speed phase, T3 = end of deceleration, a1 = acceleration for phase 1, a2 =acceleration for phase 2>
 */
Tuple5<double,double,double,double,double> Kinematics::vsLevelOutTimesBase(double s0z, double v0z, double climbRate, double targetAlt,
		double accelup, double acceldown, bool allowClimbRateChange) {

	int altDir = -1;
	if (targetAlt >= s0z) altDir = 1;
	climbRate = altDir*std::abs(climbRate);
	if (allowClimbRateChange) climbRate = altDir*(Util::max(std::abs(climbRate), std::abs(v0z)));
	double S = targetAlt-s0z;
	double a1 = acceldown;
	if (climbRate>=v0z) a1 = accelup;
	double a2 = accelup;
	if (targetAlt>=s0z) a2 = acceldown;
	double T1 = (climbRate - v0z)/a1;

	if (std::abs(S)>= std::abs(S1(v0z, a1, T1)+S3(V1(v0z, a1, T1), a2))) {
		double T2 = (S - S1(v0z, a1, T1)-S3(V1(v0z, a1, T1), a2))/climbRate;
		//fpln("times1 case1");
		return Tuple5<double,double,double,double,double>(T1, T1+T2, T1+T2+T3(climbRate, a2), a1, a2);
	}
	else {
		double aa = 0.5*a1*(1 - a1/a2);
		double bb = v0z*(1- (a1/a2));
		double cc = -v0z*v0z/(2*a2) - S;
		double root1 = Util::root(aa,bb,cc,1);
		double root2 = Util::root(aa,bb,cc,-1);
		if (root1<0)  T1 = root2;
		else if (root2<0) T1 = root1;
		else
			T1= Util::min(root1, root2);
		//fpln("times1 case2");
		return Tuple5<double, double,double,double,double>(T1, T1, T1+T3(V1(v0z, a1, T1), a2),a1,a2);
	}
}

Tuple5<double,double,double,double,double> Kinematics::vsLevelOutTimes(double s0z, double v0z, double climbRate, double targetAlt,
		double accelup, double acceldown, bool allowClimbRateChange) {

	int sgnv = -1;
	if (v0z >= 0) sgnv = 1;
	int altDir = -1;
	if (targetAlt >= s0z) altDir = 1;
	double S = targetAlt-s0z;
	double a1 = acceldown;
	if (targetAlt>=s0z) a1 = accelup;
	double a2 = accelup;
	if (targetAlt>=s0z) a2 = acceldown;


	if (sgnv==altDir || Util::almost_equals(v0z, 0.0)) {
		if (std::abs(S)>=std::abs(S3(v0z, a2))) {
			//fpln(" ##times Case1.1");
			return vsLevelOutTimesBase(s0z, v0z, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange);
		}
		else {
			Tuple5<double,double,double, double, double> ot = vsLevelOutTimesBase(s0z+S3(v0z, a2), 0.0, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange);
			//fpln("times Case1.2");
			return Tuple5<double, double,double,double,double>(-v0z/a2+ot.first, -v0z/a2+ot.second, -v0z/a2+ot.third , ot.fourth, ot.fifth);
		}
	}
	else {
		Tuple5<double,double,double, double, double> ot = vsLevelOutTimesBase(s0z+ S3(v0z, a1), 0.0, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange);
		//fpln("times Case 2");
		return Tuple5<double,double,double,double,double>(-v0z/a1+ot.first, -v0z/a1+ot.second, -v0z/a1+ot.third , ot.fourth, ot.fifth);
	}
}


Tuple5<double,double,double,double,double> Kinematics::vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
		double accelup, double acceldown, bool allowClimbRateChange) {
	double s0z = svo.first.z;
	double v0z = svo.second.z;
	return vsLevelOutTimes(s0z, v0z, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange);
}

Tuple5<double,double,double,double,double> Kinematics::vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
		double a, bool allowClimbRateChange) {
	double s0z = svo.first.z;
	double v0z = svo.second.z;
	return vsLevelOutTimes(s0z, v0z, climbRate, targetAlt, a, -a, allowClimbRateChange);
}

Tuple5<double,double,double,double,double> Kinematics::vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt, double a) {
	double s0z = svo.first.z;
	double v0z = svo.second.z;
	return vsLevelOutTimes(s0z, v0z, climbRate, targetAlt, a, -a, true);
}


double Kinematics::vsLevelOutClimbRate(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
		double accelup, double acceldown, bool allowClimbRateChange) {
	Tuple5<double,double,double,double,double> ntp = vsLevelOutTimes(svo, climbRate, targetAlt, accelup, acceldown, allowClimbRateChange);
	//fpln(" $$$ vsLevelOutTimes: "+ntp.first+" "+ ntp.second+" "+ ntp.third+" "+ntp.fourth+" "+ntp.fifth);
	return vsLevelOutCalculation(svo, targetAlt, ntp.fourth, ntp.fifth, ntp.first, ntp.second, ntp.third, ntp.first).second.z;

}


double Kinematics::vsLevelOutTime(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange) {
	Tuple5<double,double,double,double,double> qV = vsLevelOutTimes(sv0,climbRate,targetAlt,a, -a, allowClimbRateChange);
	if (qV.first < 0) return -1;
	else return qV.third;
}


double Kinematics::vsLevelOutTime(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a) {
	return vsLevelOutTime(sv0, climbRate, targetAlt, a, true);
}

std::pair<double, double> Kinematics::vsLevelOutCalc(double soz, double voz, double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t) {
	double nz = 0;
	double nvs = 0;
	if (t <= t1) {
		nvs = (voz + a1*t);
		nz = (soz + S1(voz,a1, t));
		//	fpln("#Phase1, time "+t+" Vel "+nv.z+" Pos "+ns.z);
	} else if (t <= t2) {
		nvs = (voz+a1*t1);
		nz = (soz + S1(voz,a1, t1)+ V1(voz, a1, t1)*(t-t1));
		//	fpln("#Phase2, time "+t+" Vel "+nv.z+" Pos "+ns.z);
	} else if (t <= t3) {
		nvs = (voz+a1*t1+a2*(t-t2));
		nz = (soz + S1(voz,a1, t1)+ V1(voz, a1, t1)*(t2-t1) + S1(V1(voz, a1, t1),a2, t-t2));
		//    fpln("#Phase3, time "+t+" Vel "+nv.z+" Pos "+ns.z);
	} else {
		nvs = 0;
		nz = targetAlt;
		//	fpln("#Phase4, time "+t+" Vel "+nv.z+" Pos "+ns.z);
	}
	return std::pair<double, double>(nz,nvs);
}



/** returns Pair that contains position and velocity at time t due to level out maneuver based on vsLevelOutTimesAD
 *
 * @param sv0        			current position and velocity vectors
 * @param t          			time point of interest
 * @param climbRate  			climb rate
 * @param targetAlt  			target altitude
 * @param a1         			first acceleration
 * @param a2         			second acceleration
 * @param allowClimbRateChange allows climbRate to change to initial velocity if it can help.
 * @return
 */
std::pair<Vect3, Velocity> Kinematics::vsLevelOutCalculation(const std::pair<Vect3,Velocity>& sv0,
		double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t) {
	Vect3 s0 = sv0.first;
	Velocity v0 = sv0.second;
	double soz = s0.z;
	double voz = v0.z;
	std::pair<double, double> vsL = vsLevelOutCalc(soz,voz, targetAlt, a1, a2, t1, t2, t3, t);
	double nz = vsL.first;
	double nvs = vsL.second;
	Velocity nv = v0.mkVs(nvs);
	Vect3 ns = s0.linear(v0,t).mkZ(nz);
	return std::pair<Vect3, Velocity>(ns,nv);
}

std::pair<Vect3, Velocity> Kinematics::vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
		double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange) {
	Tuple5<double,double,double,double,double> LevelParams = vsLevelOutTimes(sv0, climbRate, targetAlt, accelUp, accelDown, allowClimbRateChange);
	return vsLevelOutCalculation(sv0, targetAlt, LevelParams.fourth, LevelParams.fifth, LevelParams.first, LevelParams.second, LevelParams.third, t);
}

std::pair<Vect3, Velocity> Kinematics::vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
		double targetAlt, double a, bool allowClimbRateChange) {
	return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, allowClimbRateChange);
}

std::pair<Vect3, Velocity> Kinematics::vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
		double targetAlt, double a) {
	return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, true);
}


//// ******************************* Other **************************
//
//// time of closest approach
//double Kinematics::tau(const Vect3& s, const Vect3& vo, const Vect3& vi) {
//	double rtn;
//	Vect3 v = vo.Sub(vi);
//	double nv = v.norm();
//	if (Util::almost_equals(nv,0.0)) {
//		rtn = std::numeric_limits<double>::max();        // pseudo infinity
//	} else
//		rtn = -s.dot(v)/(nv*nv);
//	return rtn;
//}// tau
//
//double Kinematics::distAtTau(const Vect3& s, const Vect3& vo, const Vect3& vi, bool futureOnly) {
//	double tau = Kinematics::tau(s,vo,vi);
//	if (tau < 0 && futureOnly)
//		return s.norm();                 // return distance now
//	else {
//		Vect3 v = vo.Sub(vi);
//		Vect3 sAtTau = s.Add(v.Scal(tau));
//		return sAtTau.norm();
//	}
//}

}
