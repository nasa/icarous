/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;

/**
 *  TurnGeneration provides methods to generate the beginning, middle, and end of turn (BOTs, MOTs, and EOTs).
 *  This class finds solutions that are
 *  based on spherical trigonometry without having to use projections (the original versions of these methods
 *  in TrajGen used Euclidean projections).   
 */
public final class TurnGeneration {

	/**
	 * Find the beginning, middle, and end points of a turn using three points and the given radius.  It is assumed that the three
	 * points are in ordered such that the second point is the vertex of the turn.  <p>
	 * 
	 * It is expected that these properties hold:
	 * <ul>
	 * <li>The BOT/EOT velocity is track angle of the velocity at the BOT/EOT point.  
	 * <li>The ground speed and vertical speed are set by the speed from np1 to np2 for BOT, and np2 to np3 for EOT.  
	 * <li>The time of the MOT is the same as the time for np2
	 * <li>The times of the BOT and EOT points are set as if the turn is traversed at a ground speed equal to 
	 *     the ground speed from np1 to np2.
	 * <li>The altitude of the MOT point is set to np2's altitude
	 * <li>The altitude of BOT is set as if there is a constant vertical speed from np1 to np2
	 * <li>The altitude of EOT is set as if there is a constant vertical speed from np2 to np3
	 * </ul> 
	 * 
	 * @param np1 point 1
	 * @param np2 point 2 (the vertex, or turn point)
	 * @param np3 point 3
	 * @param radius the turn radius (always positive)
	 * @return The beginning, middle, and ending turn points (ie, BOT, MOT, EOT)
	 * 
	 * NOTE: TrajGen is currently using THIS ONE.
	 */
	public static Triple<NavPoint,NavPoint,NavPoint> turnGenerator(NavPoint np1, NavPoint np2, NavPoint np3, double radius) {
		if (np1.time() > np2.time() || np2.time() > np3.time() || radius < 0.0) {
			return new Triple<NavPoint,NavPoint,NavPoint>(NavPoint.INVALID, NavPoint.INVALID, NavPoint.INVALID);
		}

		Tuple5<Position,Position,Position,Integer,Double> turn = turnGenerator(np1.position(), np2.position(), np3.position(), radius);
		Position botPos = turn.first;
		Position motPos = turn.second;	
		Position eotPos = turn.third;	
		int dir = turn.fourth;
		double arcLength = turn.fifth;

		//Velocity vv = botPos.initialVelocity(np2.position(), 100); 
		//Position center = dir==1?botPos.linear(Velocity.make(vv.PerpR().Hat2D()), radius):botPos.linear(Velocity.make(vv.PerpL().Hat2D()), radius);
		//f.pln("turngeneratorLLA arclength="+arcLength+" calc="+(PositionUtil.angle_between(botPos, center, eotPos)*radius));		

		Velocity vin = NavPoint.initialVelocity(np1,np2);  // used to set gs and vs
		Velocity vout = NavPoint.initialVelocity(np2,np3); // used to set gs and vs
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
		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos, tBOT, vin, dir*radius,linearIndex);
		NavPoint npEOT = np2.makeOriginal().makeLabel("").makeEOT(eotPos, tEOT, vout,linearIndex);							
		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT,linearIndex).makeLabel(""); 
		//f.pln(" $$$$$ npBOT = "+npBOT+"  npEOT = "+npEOT+"  npEOT = "+npEOT);
		return new Triple<NavPoint,NavPoint,NavPoint>(npBOT, npMOT, npEOT);
	}


	/**
	 * Find the beginning, middle, and end points of a turn using three points and the given radius.  The three
	 * points are in ordered such that the second point is the vertex of the turn.  The BOT is along the line
	 * from p1 to p2, the EOT is along the line from p2 to p3, the MOT is in middle of the turn.<p>
	 * 
	 * @param p1 point 1
	 * @param p2 point 2 (the vertex, or turn point)
	 * @param p3 point 3
	 * @param radius the turn radius (always positive)
	 * @return The beginning, middle, and ending turn points (ie, BOT, MOT, EOT), direction of turn (-1 for left, 1 for right), arc distance along turn from right to left. 
	 */
	public static Tuple5<Position,Position,Position,Integer,Double> turnGenerator(Position p1, Position p2, Position p3, double radius) {
		Position botPos;
		Position motPos;	
		Position eotPos;	
		int dir;
		double distance;
		if (p2.isLatLon()) { 			
			Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double> points = turnGeneratorLLA(p1.lla(),p2.lla(),p3.lla(),radius);
			//Triple<LatLonAlt,LatLonAlt,LatLonAlt> points = turnGeneratorLLA_Alt2(p1.lla(),p2.lla(),p3.lla(),radius);
			//Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double>  points = turnGeneratorLLA_Alt3(p1.lla(),p2.lla(),p3.lla(),radius);
			botPos = new Position(points.first);
			motPos = new Position(points.second);	
			eotPos = new Position(points.third);	
			dir = points.fourth;
			distance = points.fifth;
		} else {
			Tuple5<Vect3,Vect3,Vect3,Integer,Double> points = turnGeneratorEucl(p1.point(),p2.point(),p3.point(),radius);
			botPos = new Position(points.first);
			motPos = new Position(points.second);	
			eotPos = new Position(points.third);		
			dir = points.fourth;
			distance = points.fifth;
		}
		return new Tuple5<Position,Position,Position,Integer,Double>(botPos,motPos,eotPos,dir,distance);
	}



	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal Components Only)
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param radius Radius of the turn (always positive)
	 * @return BOT,MOT,EOT positions, direction, and arclength from BOT to EOT   
	 */
	public static Tuple5<Vect2,Vect2,Vect2,Integer,Double> turnGeneratorEucl(Vect2 pt1, Vect2 pt2, Vect2 pt3, double radius) {
		Vect2 ao = pt3.Sub(pt2);
		Vect2 bo = pt1.Sub(pt2);
		Vect2 ahat = ao.Hat();
		Vect2 bhat = bo.Hat();
		Vect2 v = ahat.Add(bhat);
		double k = radius/Math.sqrt((v.sqv() - Util.sq(v.dot(ahat))));
		Vect2 w = v.Scal(k);
		Vect2 wdota = ahat.Scal(w.dot(ahat));
		Vect2 wdotb = bhat.Scal(w.dot(bhat));
		Vect2 BOT = pt2.Add(wdotb);
		Vect2 EOT = pt2.Add(wdota);
		double distAB = BOT.Sub(EOT).norm();
		double alpha = 2*(Math.asin(distAB/(2*radius))); 
		double arcLength = alpha*radius;	
		int dir = VectFuns.rightOfLinePoints(pt1,pt2,pt3);

		// calculate MOT position
		Vect2 center = pt2.Add(w);
		Vect2 vhat = pt2.Sub(center).Hat();
		Vect2 MOT = center.Add(vhat.Scal(radius));
		return new Tuple5<Vect2,Vect2,Vect2,Integer,Double>(BOT,MOT,EOT,dir,arcLength);
	}


	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal components only, altitudes
	 * are all set to p1's altitude.
	 * 
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param radius Radius of the turn (always positive)
	 * @return BOT,EOT pair   
	 */
	private static Tuple5<Vect3,Vect3,Vect3,Integer,Double> turnGeneratorEucl(Vect3 p1, Vect3 p2, Vect3 p3, double radius) {
		Tuple5<Vect2,Vect2,Vect2,Integer,Double> tge = turnGeneratorEucl(p1.vect2(), p2.vect2(), p3.vect2(), radius);
		Vect2 BOT = tge.first;
		Vect2 MOT = tge.second;
		Vect2 EOT = tge.third;
		double altBOT = p1.z(); 
		double altMOT = p1.z();
		double altEOT = p1.z(); 
		Vect3 BOTv3 = new Vect3(BOT.x, BOT.y, altBOT);	
		Vect3 MOTv3 = new Vect3(MOT.x, MOT.y, altMOT);	
		Vect3 EOTv3 = new Vect3(EOT.x, EOT.y, altEOT);			
		return new Tuple5<Vect3,Vect3,Vect3,Integer,Double>(BOTv3,MOTv3,EOTv3,tge.fourth,tge.fifth);
	}

	/**
	 * Return the BOT, MOT, and EOT TCPs for a turn. 
	 * NOTE.   MOT.alt = np.alt()
	 * velocity in metadata is set correctly for BOT, with track values modified to match the MOT and EOT points
	 * 
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param R Radius of the turn
	 * @return BOT,MOT,EOT triple.   
	 */
	@Deprecated // ONLY USED IN UNIT TESTS
	public static Triple<NavPoint,NavPoint,NavPoint> turnGeneratorProjected(NavPoint np1, NavPoint np2, NavPoint np3, double R) {
		Vect2 pt1;
		Vect2 pt2;
		Vect2 pt3;
		Position p1 = np1.position();
		//f.pln("\n >>>>>>>>>>>>>>>>>.. TurnGenerator: np1 = "+np1+" np2 = "+np2+" np3 = "+np3);
		Position p2 = np2.position();
		Position p3 = np3.position();
		// convert the points to Euclidean vectors (if necessary)
		EuclideanProjection proj = Projection.createProjection(p2.lla().lat(), p2.lla().lon(), 0); // preserves altitude
		if (p2.isLatLon()) {  
			double chkLen = Units.from("NM",10.0);
			LatLonAlt p1lla = p1.lla();
			if (p2.distanceH(p1) > chkLen) {
				p1lla = GreatCircle.linear_gc(p2.lla(), p1.lla(), chkLen);
				//f.pln(" $$ TurnGenerator is using p1lla "+p1lla+" instead of "+p1.lla());
			}
			LatLonAlt p3lla = p3.lla();
			if (p2.distanceH(p3) > chkLen) {
				p3lla = GreatCircle.linear_gc(p2.lla(), p3.lla(), chkLen);
				//f.pln(" $$ TurnGenerator is using p3lla "+p3lla+" instead of "+p3.lla());
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
		double k = R/Math.sqrt((v.sqv() - Util.sq(v.dot(ahat))));
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
		if (Math.abs(sinTheta) > 1.0) alpha = 0;
		else  alpha = 2*(Math.asin(sinTheta)); 
		//f.pln(" $$$$$ TurnGenerator: gs1 = "+Units.str("kn",gs1)+" alpha = "+alpha+" distAB "+Units.str("ft",distAB)+" R = "+Units.str("ft",R));
		double d2 = alpha*R;	//arc length
		Velocity v1 = np1.initialVelocity(np2);
		Velocity v2 = np2.initialVelocity(np3);
		// make the time of the mot = the time of the middle vertex
		double tMOT = np2.time();
		double turnTime = d2/gs1;
		//f.pln(" $$$$$ TurnGenerator: turnTime = "+turnTime+" tMOT = "+tMOT+" alpha = "+Units.str("deg",alpha)+" gs1 = "+Units.str("kn",gs1));
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
		Vect3 v3BOT = new Vect3(BOT.x,BOT.y,altBOT);
		Vect3 v3MOT = new Vect3(mot.x,mot.y,altMOT);
		Vect3 v3EOT = new Vect3(EOT.x,EOT.y,altEOT);
		Position motPos;
		Position botPos;
		Position eotPos;
		if (p1.isLatLon()) {  
			motPos = new Position(proj.inverse(v3MOT));
			botPos = new Position(proj.inverse(v3BOT));
			eotPos = new Position(proj.inverse(v3EOT));                         
		} else {
			motPos = new Position(v3MOT);
			botPos = new Position(v3BOT);
			eotPos = new Position(v3EOT);
		}
		//double  vinTrk2 = new NavPoint(botPos,tBOT).initialVelocity(np2).trk();
		double vinTrk = pt2.Sub(BOT).trk();
		//f.pln(" $$$ vinTrk = "+Units.str8("deg",vinTrk)+" vinTrk2 = "+Units.str8("deg",vinTrk2));
		//		double  voutTrk = new NavPoint(eotPos,tEOT).initialVelocity(np3).trk();
		Velocity vin = v1.mkTrk(vinTrk);
		int dir = Util.turnDir(vin.trk(), v2.trk());
		//double signedRadius = Util.turnDir(vin.trk(), v2.trk())*R;
		double signedRadius = dir*R; // vin.gs()/omega;
		int ix = np2.linearIndex();
		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos, tBOT, vin, signedRadius, ix).makeLabel(np2.label());   // only BOT has label from np2
		//f.pln(" $$$ turnGenerator: npBOT = "+npBOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT,ix).makeLabel(""); // ,vin.mkTrk(trk2)).makeLabel("");   
		//f.pln(" $$$ turnGenerator: npMOT = "+npMOT.toStringFull());
		double omega = dir*vin.gs()/R;    // TODO:  only used for EOT,  can calculate trkOut without this 
		double trk3 = vin.trk() + omega * (tEOT-tBOT);
		NavPoint npEOT = np2.makeOriginal().makeEOT(eotPos,tEOT, vin.mkTrk(trk3),ix).makeLabel("");	
		//f.pln(" $$$ turnGenerator: npEOT = "+npEOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		return new Triple<NavPoint,NavPoint, NavPoint>(npBOT,npMOT,npEOT);
	}

	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal Components Only, altitudes
	 * are not accurately computed)
	 * 
	 * @param p2 the turn point
	 * @param trkIn the "track in" to the turn point
	 * @param trkOut the "track out" of the turn point
	 * @param radius radius of the turn
	 * @return returns BOT, MOT, EOT, direction, and arc distance from BOT to EOT along the curve
	 */
	public static Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double> turnGeneratorLLA(LatLonAlt p2, double trkIn, double trkOut, double radius) {
		double deltaTrack = Util.turnDelta(trkIn,trkOut);
		int dir = Util.turnDir(trkIn,trkOut);
		double distance = radius*Math.tan(deltaTrack/2.0);               // TODO ************* EUCLIDEAN ****************
		LatLonAlt botPos =  GreatCircle.linear_initial(p2, trkIn, -distance);
		LatLonAlt eotPos =  GreatCircle.linear_initial(p2, trkOut, distance);
		double cLineDist = radius/Math.cos(deltaTrack/2.0) - radius;
		double cTrk = trkIn + dir*(deltaTrack + Math.PI)/2.0;
		LatLonAlt motPos =  GreatCircle.linear_initial(p2, cTrk, cLineDist);  
		double arcLength = deltaTrack*radius;                            // TODO: ****** EUCLIDEAN *********	
		//double arcLength2 = GreatCircle.small_circle_arc_length(radius, deltaTrack);
	    //f.pln(" $$$ arcLength = "+Units.str("NM",arcLength,12)+" Delta arcLength = "+Units.str("m",arcLength2-arcLength,12));
		//double arcLength2 = arcLengthLLA(botPos, p2, radius);
		return new Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double>(botPos,motPos,eotPos,dir,arcLength);		
	}

	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal Components Only, altitudes
	 * are set to p1's altitude)
	 * @param p1 first point
	 * @param p2 second point (the turn point)
	 * @param p3 third point
	 * @param radius radius of turn
	 * @return returns BOT, MOT, EOT, direction, and arc distance from BOT to EOT along the curve
	 */
	public static Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double> turnGeneratorLLA(LatLonAlt p1, LatLonAlt p2, LatLonAlt p3, double radius) {
		double trkIn = GreatCircle.final_course(p1, p2);
		double trkOut = GreatCircle.initial_course(p2, p3);
		Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double> tG = turnGeneratorLLA(p2, trkIn, trkOut, radius);
		double altBOT = p1.alt();
		double altMOT = p1.alt(); 
		double altEOT = p1.alt(); 
		LatLonAlt botPos = tG.first.mkAlt(altBOT);
		LatLonAlt motPos = tG.second.mkAlt(altMOT);              
		LatLonAlt eotPos = tG.third.mkAlt(altEOT);
		return new Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double>(botPos,motPos,eotPos,tG.fourth,tG.fifth);
	}





	/**
	 * Altitudes are not set.
	 * 
	 * @param p1
	 * @param p2
	 * @param p3
	 * @param radius as a distance, not an angle
	 * @return
	 */
	public static Triple<LatLonAlt,LatLonAlt,LatLonAlt> turnGeneratorLLA_Alt2(LatLonAlt p1, LatLonAlt p2, LatLonAlt p3, double radius) {
		double A = GreatCircle.angle_between(p1, p2, p3)/2.0;
		@SuppressWarnings("deprecation")
		Triple<Double,Double,Double> bCc = GreatCircle.side_angle_angle(GreatCircle.angle_from_distance(radius,0.0), A, Math.PI/2, true);
		double c = GreatCircle.distance_from_angle(bCc.third,  0.0);
		double track1 = GreatCircle.initial_course(p2, p1);
		double track2 = GreatCircle.initial_course(p2, p3);
		LatLonAlt botPos = GreatCircle.linear_initial(p2, track1, c);
		LatLonAlt eotPos = GreatCircle.linear_initial(p2, track2, c);
		double track1f = track1+Math.PI;
		int dir = Util.turnDir(track1f, track2);
		double track3 = track2+A*dir;
		double b = GreatCircle.distance_from_angle(bCc.first,  0.0);
		LatLonAlt motPos = GreatCircle.linear_initial(p2, track3, b-radius);
		return new Triple<LatLonAlt,LatLonAlt,LatLonAlt>(botPos,motPos,eotPos);
	}



	//TODO: GEH NEW:  UNUSED
	private static double arcLengthLLA(LatLonAlt bot, LatLonAlt intercept, double radius) {
		double a = GreatCircle.angle_from_distance(radius, 0.0);
		double b = GreatCircle.angular_distance(bot, intercept);
		if (Util.almost_equals(b, 0.0)) return 0.0;
		double B = GreatCircle.side_angle_side(a, Math.PI/2, b).second;
		double angle = B*2;
		double radius_prime = GreatCircle.chord_distance(2*radius)/2;
		return angle * radius_prime;
	}




	/**
	 * Rprime takes the turnRadius (great circle distance) and returns another distance.
	 *  Given a point c on the sphere (center of turn, for instance), and given another point
	 *  p on the sphere that has radius TurnRad (great circle radius), this functions
	 *  returns the distance (closest) between c and the line through the origin and p.
	 * @param TurnRad
	 * @return new radius
	 */
	private static double Rprime(double TurnRad) { // Rad is turn radius (gc dist)
		return GreatCircle.spherical_earth_radius*Math.sin(TurnRad/GreatCircle.spherical_earth_radius);
	}

	/**
	 * p,q,r are assumed to be on the surface of a sphere. This gives 1 for a right turn and -1 for a left turn.
	 * @param p point
	 * @param q point
	 * @param r point
	 * @return -1 or 1
	 */
	private static int turnDirection(Vect3 p, Vect3 q, Vect3 r) { 
		return -Util.sign(r.dot(p.cross(q)));
	}

	/**
	 * returns a unit normal vector to the plane containing p, q, and the origin, in the direction of r.
	 * @param p Vect3
	 * @param q Vect3
	 * @param r Vect3
	 * @return Vect3
	 */
	private static Vect3 normalvect(Vect3 p, Vect3 q, Vect3 r){
		return p.cross(q).Hat().Scal(-turnDirection(p,q,r));
	}

	/**
	 * Plane1 is set of all points Rprime(TurnRad) distance from the plane containing p,q,0, in the direction of r
	 * Plane2 is set of all points Rprime(TurnRad) distance from the plane containing q,r,0, in the direction of p
	 * This function computes one point that is on the line of intersection between these two planes.
	 * @param TurnRad
	 * @param p
	 * @param q
	 * @param r
	 * @return Vect3 on the line
	 */
	private static Vect3 point_on_line(double TurnRad, Vect3 p, Vect3 q, Vect3 r){ // Rad is turn radius (gc dist)
		double ERad = GreatCircle.spherical_earth_radius;
		Vect3 n1 = normalvect(p,q,r);
		Vect3 n2 = normalvect(r,q,p);
		Vect3 n1xn2 = n1.cross(n2);
		double Rp = Rprime(TurnRad);
		if (Math.abs(n1xn2.x)>=Util.max(Math.abs(n1xn2.y),Math.abs(n1xn2.z))) {
			double a=n1.y;
			double b=n1.z;
			double c=n2.y;
			double d=n2.z;
			double e=Rp-n1.x*ERad;
			double f=Rp-n2.x*ERad;
			double det = n1xn2.x;
			return new Vect3(ERad,(d*e-b*f)/det,(-c*e+a*f)/det);
		} else if (Math.abs(n1xn2.y)>Math.abs(n1xn2.z)) {
			double a=n1.x;
			double b=n1.z;
			double c=n2.x;
			double d=n2.z;
			double e=Rp-n1.y*ERad;
			double f=Rp-n2.y*ERad;
			double det = -n1xn2.y;
			return new Vect3((d*e-b*f)/det,ERad,(-c*e+a*f)/det);
		} else {
			double a=n1.x;
			double b=n1.y;
			double c=n2.x;
			double d=n2.y;
			double e=Rp-n1.z*ERad;
			double f=Rp-n2.z*ERad;
			double det=n1xn2.z;
			return new Vect3((d*e-b*f)/det,(-c*e+a*f)/det,ERad);
		}
	}

	private static Vect3 turnCenterxyz(double TurnRad, Vect3 p, Vect3 q, Vect3 r) {
		double ERad = GreatCircle.spherical_earth_radius;
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
		double t = (-(pt.dot(n))+Util.sqrt_safe(Util.sq(pt.dot(n))-n.sqv()*(pt.sqv()-Util.sq(ERad))))/(n.sqv());
		return pt.AddScal(t,n);	
	}

	public static Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double> turnGeneratorLLA_Alt3(LatLonAlt p1, LatLonAlt p2, LatLonAlt p3, double radius) {
		Vect3 p1e = GreatCircle.spherical2xyz(p1.lat(), p1.lon());
		Vect3 p2e = GreatCircle.spherical2xyz(p2.lat(), p2.lon());
		Vect3 p3e = GreatCircle.spherical2xyz(p3.lat(), p3.lon());

		double ERad = GreatCircle.spherical_earth_radius;
		double turnRad = Math.abs(radius);
		double Rp = Rprime(turnRad);
		Vect3 c = turnCenterxyz(turnRad,p1e,p2e,p3e);
		Vect3 n1 = normalvect(p1e,p2e,p3e);
		Vect3 n2 = normalvect(p3e,p2e,p1e);
		Vect3 depBOT = c.Add(n1.Scal(-Rp));
		double depBOTnorm = depBOT.norm();
		Vect3 depEOT = c.Add(n2.Scal(-Rp));
		double depEOTnorm = depEOT.norm();
		double Fac = Util.sqrt_safe(1-Math.pow(Rp/ERad,2));
		Vect3 dirVect = p2e.AddScal(-(c.Hat2D().dot(p2e.Hat2D())), c); //TODO: changed to Hat2D

		Vect3 bote = depBOT.Scal(ERad/depBOTnorm);
		Vect3 eote = depEOT.Scal(ERad/depEOTnorm);
		Vect3 mote = c.Scal(Fac).AddScal(Rp, dirVect.Hat2D()); //TODO: changed to Hat2D
		int dir = turnDirection(p1e,p2e,p3e);
		double cordLen = (bote.Sub(eote)).norm()/Rp;
		double turnAng = 2*Math.asin(cordLen/2);
		double distance = Rp*turnAng;

		//		f.pln("p1 "+p1e);
		//		f.pln("p2 "+p2e);
		//		f.pln("p3 "+p3e);
		//		f.pln("bot "+bote);
		//		f.pln("eot "+eote);

		LatLonAlt bot = GreatCircle.xyz2spherical(bote);
		LatLonAlt eot = GreatCircle.xyz2spherical(eote);
		LatLonAlt mot = GreatCircle.xyz2spherical(mote);

		bot = bot.mkAlt(p1.alt());
		mot = mot.mkAlt(p1.alt());
		eot = eot.mkAlt(p1.alt());

		return new Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double>(bot,mot,eot,dir,distance);
	}


//	/**
//	 * experimental
//	 * This provides a list of turn points (BOT/EOT, BOT/EOTBOT/EOT, or BOT/EOT/BOT/EOT, or none) such that the 2D path: start-{BOT-EOT}-targ starts at point p1 with direction trk1
//	 * and ends at point p2 with direction trk2.  Turns will use radius R1 and R2, repsectively.
//	 * Altitudes of all points are ignored.  It is possible that two or more of these points overlap.
//	 */
//	public ArrayList<Vect2> turnToTrackThroughPoint(Vect2 p1, double trk1, Vect2 p2, double trk2, double R1, double R2) {
//		ArrayList<Vect2> ret = new ArrayList<Vect2>();
//		Vect2 v1 = Vect2.mkTrkGs(trk1, 100);
//		Vect2 v2 = Vect2.mkTrkGs(trk2, 100);
//		Vect2 center1;
//		Vect2 center2;
//		Vect2 tmpbot2; // tangent from p1 to second turn circle, this estimates the second BOT 
//		double dir1 = p2.Sub(p1).det(v1); // p2 is on this side of state1 
//		double dir2 = p1.Sub(p2).det(v2); // p1 is on this side of state2
//		if (Util.almost_equals(dir2, 0)) { // p1 roughly in line with p2
//			if (Util.almost_equals(dir1, 0)) {
//				return ret; // in a straight line, nothing to do
//			}
//			center2 = p2;
//		} 
//		
//		if (dir2 > 0) { // p1 to right of p2
//			center2 = p2.AddScal(R2, v2.PerpR());
//			if (p1.distance(center2) < R2) return null; // error, can't make second turn, too close
//		} else if (dir2 < 0) { // p1 to left of p2
//			center2 = p2.AddScal(R2, v2.PerpL());  
//			if (p1.distance(center2) < R2) return null; // error, can't make second turn, too close
//		}
//		
//		// interior center of similitude (intersection of cross tangents between the circles), from Wolfram Mathworld:
//		
//		Vect2 icos = center1.Scal(R1).Add(center2.Scal(R2)).Scal(R1+R2);
//	}

}
