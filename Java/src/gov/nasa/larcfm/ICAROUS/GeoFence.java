/**
 * GeoFence
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */
package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.IO.SeparatedInput;   

import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.enums.*;
import gov.nasa.larcfm.Util.AircraftState;


public class GeoFence{
	
	public enum FENCE_TYPE{KEEP_IN,KEEP_OUT};

	public FENCE_TYPE fType;
	public int ID;
	public int numVertices;
	public double floor;
	public double ceiling;



	public static final double BUFF= 0.1;

	public Position SafetyPoint      = null;
	public Position RecoveryPoint    = null;
	public Position ClosestPoint     = null;
	public boolean violation         = false;
	public boolean conflict          = false;
	public boolean isconvex          = false;
	public Vect2 closestEdgeVector;
	public EuclideanProjection proj;
	public SimplePoly geoPolyLLA;
	public SimplePoly geoPolyLLA2; // expanded or contracted based on type
	public Poly3D geoPoly3D;
	public CDPolycarp geoPolyCarp;
	public PolyPath geoPolyPath;
	public CDIIPolygon cdp;
	public PolycarpResolution pcr;
	public PolyUtil pu;
        public List<Vect2> fenceVertices;      // Original vertices of the polygon in local NED frame
        public List<Vect2> fenceVertices2;     // Vertices after expansion or contraction
	public PolycarpDetection pcDet;
	public PolycarpEdgeProximity pep;

	public ParameterData pData;
	public double entryTime;
	public double exitTime;


	public GeoFence(int IDIn,int TypeIn,int numVerticesIn,double floorIn,double ceilingIn,ParameterData pdata){
		geoPolyLLA     = new SimplePoly(floorIn,ceilingIn);
		geoPoly3D      = new Poly3D();
		
		if(TypeIn == 0)
			fType          = FENCE_TYPE.KEEP_IN;
		else
			fType          = FENCE_TYPE.KEEP_OUT;
		
		ID             = IDIn;
		numVertices    = numVerticesIn;
		floor          = floorIn;
		ceiling        = ceilingIn;
		geoPolyCarp    = new CDPolycarp();
		geoPolyPath    = new PolyPath();
		geoPolyCarp.setCheckNice(false);
		cdp            = new CDIIPolygon(geoPolyCarp);
		pcr            = new PolycarpResolution();
		pu             = new PolyUtil();
		fenceVertices  = new ArrayList<Vect2>();
		pcDet          = new PolycarpDetection();
		pep            = new PolycarpEdgeProximity();
		pData          = pdata;    		
	}

	public void AddVertex(int index,float lat,float lon){

		
		double hthreshold = pData.getValue("HTHRESHOLD");
		Position pos = Position.makeLatLonAlt(lat,lon,0);
		geoPolyLLA.addVertex(pos);

		if(geoPolyLLA.size() == numVertices){
			isconvex = geoPoly3D.poly2D().isConvex();

			proj       = Projection.createProjection(geoPolyLLA.getVertex(0));
			geoPoly3D  = geoPolyCarp.makeNicePolygon(geoPolyLLA.poly3D(proj));
			Velocity v = Velocity.makeTrkGsVs(0.0,0.0,0.0);
			
			for(int i=0;i<geoPoly3D.size();++i){
				fenceVertices.add(geoPoly3D.getVertex(i));
			}

			// Expand or contract polygon based on type
			if(fType == FENCE_TYPE.KEEP_IN){
				fenceVertices2 = PolycarpResolution.contract_polygon_2D(BUFF,hthreshold,fenceVertices);		
			}
			else{
				fenceVertices2 = PolycarpResolution.expand_polygon_2D(BUFF,hthreshold,fenceVertices);
			}

			Poly2D p2D     = new Poly2D(fenceVertices2);
			Poly3D p3D     = new Poly3D(p2D,floor,ceiling);
			geoPolyLLA2    = SimplePoly.make(p3D,proj);
			geoPolyPath.addPolygon(geoPolyLLA2,v,0);
		}				    
	}

	public Position GetClosestPoint(Position pos){

		double dist = Double.MAX_VALUE;
		Position ClosestPoint = null;	
		Vect3 so = proj.project(pos);
		for(int i=0;i<numVertices;i++){
			Vect2 A = fenceVertices.get(i);
			Vect2 B;
			if(i< (numVertices - 1)){
				B = fenceVertices.get(i+1);
			}
			else{
				B = fenceVertices.get(0);
			}
			Vect2 C = so.vect2();
			Vect2 D = pep.closest_point(A,B,C,BUFF);
			double val = D.Sub(C).norm();
			if(val < dist){
				dist = val;
				LatLonAlt LLA = proj.inverse(D,pos.alt());
				ClosestPoint  = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
				closestEdgeVector = B.Sub(A);
			}
		}

		return ClosestPoint;
	}

	public void CheckViolation(AircraftState acState, double currentTime,Plan FP){
		
		double alt;

		Position pos = acState.positionLast();
		Velocity vel = acState.velocityLast();
		Vect3 so = proj.project(pos);

		SafetyPoint = GetSafetyPoint(acState);

		GetClosestPoint(pos);

		double lookahead    = pData.getValue("LOOKAHEAD");	
		double hthreshold   = pData.getValue("HTHRESHOLD");
		double vthreshold   = pData.getValue("VTHRESHOLD");
		double hstepback    = pData.getValue("HSTEPBACK");
		double vstepback    = pData.getValue("VSTEPBACK");		

		//System.out.println("Distance from edge:"+geoPoly3D.distanceFromEdge(so));

		// Keep in geofence
		if(fType == FENCE_TYPE.KEEP_IN){	    
			if(geoPolyCarp.nearEdge(so,geoPoly3D,hthreshold,vthreshold)){
				conflict  = true;				
			}else{
		        conflict = CollisionDetection(pos,vel.vect2(),0,lookahead);				
			}

			
			if(geoPolyCarp.definitelyInside(so,geoPoly3D)){
				violation = false;		
			}
			Vect2 so_2 = so.vect2();
			Vect2 recpoint = pcr.inside_recovery_point(BUFF,hstepback,fenceVertices2,so_2);
			
			if(Math.abs(so.z - ceiling) <= vthreshold){
				alt = ceiling - vstepback;
			}else{
				alt = so.z;				
			}
			
			LatLonAlt LLA = proj.inverse(recpoint,pos.alt());
			RecoveryPoint = Position.makeLatLonAlt(LLA.latitude(),"degree",LLA.longitude(),"degree",alt,"m");	
			//System.out.println(RecoveryPoint.toStringNP(6));
		}
		//Keep out Geofence
		else{
			Vect2 so_2 = so.vect2();
			Vect2 recpoint = pcr.outside_recovery_point(BUFF,hthreshold,fenceVertices2,so_2);
			LatLonAlt LLA = proj.inverse(recpoint,pos.alt());;
			RecoveryPoint = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());	    
			cdp.detection(FP,geoPolyPath,currentTime,FP.getLastTime());
			boolean val = CollisionDetection(pos,vel.vect2(),0,lookahead);
			if(val){							
				conflict = true;
				entryTime = cdp.getTimeIn(0);
				exitTime  = cdp.getTimeOut(0);
			}
			else{
				conflict = false;
			}

			if(geoPolyCarp.definitelyInside(so,geoPoly3D)){
				violation = true;		
			}else{
				violation = false;
			}	    
		}
	}

	public Position GetSafetyPoint(AircraftState acState){

		double Min  = Double.MAX_VALUE;
		double dist = 0.0;
		double x1   = 0.0;
		double y1   = 0.0;
		double x2   = 0.0;
		double y2   = 0.0;
		double x3   = 0.0;
		double y3   = 0.0;
		double x30  = 0.0;
		double y30  = 0.0;
		double x0   = 0.0;
		double y0   = 0.0;

		double m    = 0.0;
		double a    = 0.0;
		double b    = 0.0;
		double c    = 0.0;

		int vert_i  = 0;
		int vert_j  = 0;

		boolean insegment;

		Vect2 C,CD,CDe;
		LatLonAlt LLA;

		Position pos  = acState.positionLast();
		Position pos0 = acState.position(acState.size()-2);

		LatLonAlt p   = pos.zeroAlt().lla();
		LatLonAlt p0  = pos0.zeroAlt().lla();
		Vect2 xy      = proj.project(p).vect2();
		Vect2 xy0     = proj.project(p0).vect2();
		x3            = xy.x();
		y3            = xy.y();
		x30           = xy0.x();
		y30           = xy0.y();

		Position Safe = Position.makeXYZ(0.0,0.0,0.0);
		Position recPoint = Position.makeXYZ(0.0,0.0,0.0);

		int stepbacktype    = pData.getInt("STEPBACKTYPE");
		double hstepback    = pData.getValue("HSTEPBACK");
		double vstepback    = pData.getValue("VSTEPBACK");
		double hthreshold   = pData.getValue("HTHRESHOLD");
		double vthreshold   = pData.getValue("VTHRESHOLD");	


		// Check if perpendicular intersection lies within line segment
		for(int i=0;i<geoPoly3D.size();i++){
			vert_i = i;

			if(i == geoPoly3D.size() - 1)
				vert_j = 0;
			else
				vert_j = i + 1;

			x1 = geoPoly3D.poly2D().getVertex(vert_i).x();
			y1 = geoPoly3D.poly2D().getVertex(vert_i).y();

			x2 = geoPoly3D.poly2D().getVertex(vert_j).x();
			y2 = geoPoly3D.poly2D().getVertex(vert_j).y();

			m  = (y2 - y1)/(x2 - x1);

			a  = m;
			b  = -1;
			c  = (y1-m*x1);

			x0 = (b*(b*x3 - a*y3) - a*c)/(Math.pow(a,2)+Math.pow(b,2));
			y0 = (a*(-b*x3 + a*y3) - b*c)/(Math.pow(a,2)+Math.pow(b,2));

			insegment = false;

			Vect2 AB = new Vect2(x2-x1,y2-y1);
			Vect2 AC = new Vect2(x0-x1,y0-y1);
			C        = new Vect2(x0,y0);

			double projAC = AC.dot(AB)/Math.pow(AB.norm(),2);

			if(projAC >= 0 && projAC <= 1)
				insegment = true;
			else
				insegment = false;    	

			if(insegment){
				dist = Math.abs(a*x3 + b*y3 + c)/Math.sqrt(Math.pow(a,2) + Math.pow(b,2));
				CD  = new Vect2(x3-x0,y3-y0);		
				CDe = C.Add(CD.Scal(hstepback/CD.norm()));

				LLA         = proj.inverse(CDe,pos.alt());
				Safe        = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());

			}
			else{
				double d1 = Math.sqrt( Math.pow(x3-x1,2) + Math.pow(y3-y1,2) );
				double d2 = Math.sqrt( Math.pow(x3-x2,2) + Math.pow(y3-y2,2) );
				dist = Math.min(d1,d2);

				if(d1 <= d2){
					CD  = new Vect2(x3-x1,y3-y1);
					C   = new Vect2(x1,y1);
				}
				else{
					CD  = new Vect2(x3-x2,y3-y2);
					C   = new Vect2(x2,y2);
				}


				CDe       = C.Add(CD.Scal(hstepback/CD.norm()));

				LLA       = proj.inverse(CDe,pos.alt());
				Safe      = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
			}

			if(dist < Min){
				Min = dist;
				recPoint = Safe;

			}


		}



		if(stepbacktype == 0){
			CD  = new Vect2(x30-x3,y30-y3);
			C   = new Vect2(x3,y3);
			CDe = C.Add(CD.Scal(hstepback/CD.norm()));

			LLA         = proj.inverse(CDe,pos.alt());
			recPoint  = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
		}

		double alt;
		if( (ceiling - pos.alt()) <= vthreshold){
			alt = (ceiling - vstepback);
			if(RecoveryPoint != null){
				recPoint = RecoveryPoint.mkAlt(alt);
			}
		}
		else if( (pos.alt() - floor) <= vthreshold){
			alt = (floor + vstepback);
			if(RecoveryPoint != null){
				recPoint = RecoveryPoint.mkAlt(alt);
			}
		}


		return recPoint;
	}



	public boolean CheckWaypointFeasibility(Position CurrPos, Position NextWP){

		LatLonAlt p1      = CurrPos.lla();
		LatLonAlt p2      = NextWP.lla();
		Vect3 CurrPosVec  = proj.project(p1);
		Vect3 NextWPVec   = proj.project(p2);
		int vertexi,vertexj;

		boolean InPlaneInt;

		if( (NextWP.alt() > ceiling) || (NextWP.alt() < floor)){
			return false;
		}
		
		for(int i=0;i<numVertices;i++){
			vertexi = i;

			// **
			if(i==numVertices - 1){
				vertexj = 0;
			}
			else{
				vertexj = vertexi + 1;
			}
			// **
			InPlaneInt = LinePlaneIntersection(geoPoly3D.getVertex(vertexi),geoPoly3D.getVertex(vertexj),
					floor,ceiling,CurrPosVec,NextWPVec);

			if(InPlaneInt){
				return false;
			}
		}

		return true;

	}

	public boolean LinePlaneIntersection(Vect2 A, Vect2 B,double floor, double ceiling,Vect3 CurrPos,Vect3 NextWP){

		double x1 = A.x();
		double y1 = A.y();
		double z1 = floor;

		double x2 = B.x();
		double y2 = B.y();
		double z2 = ceiling;

		Vect3 l0  = new Vect3(CurrPos.x(),CurrPos.y(),CurrPos.z());
		Vect3 p0  = new Vect3(x1,y1,z1);

		Vect3 n   = new Vect3(-(z2-z1)*(y2-y1),(z2-z1)*(x2-x1),0);
		Vect3 l   = new Vect3( NextWP.x() - CurrPos.x(), NextWP.y() - CurrPos.y(), NextWP.z() - CurrPos.z() );

		double d  = (p0.Sub(l0).dot(n))/(l.dot(n));

		Vect3 PntI = l0.Add(l.Scal(d));

		// **
		Vect3 OA   = new Vect3(x2-x1,y2-y1,0);
		Vect3 OB   = new Vect3(0,0,z2-z1);
		Vect3 OP   = PntI.Sub(p0);
		Vect3 CN   = NextWP.Sub(CurrPos);
		Vect3 CP   = PntI.Sub(CurrPos);

		double proj1      = OP.dot(OA)/Math.pow(OA.norm(),2);
		double proj2      = OP.dot(OB)/Math.pow(OB.norm(),2);
		double proj3      = CP.dot(CN)/Math.pow(CN.norm(),2);


		if(proj1 >= 0 && proj1 <= 1){
			if(proj2 >= 0 && proj2 <= 1){
				if(proj3 >= 0 && proj3 <= 1)
					return true;
			}
		}

		return false;

	}

	public ArrayList<Position> LineCircleIntersection(Position pos,Double Radius){

		ArrayList<Position> Pts = new ArrayList<Position>();	

		for(int i=0;i<fenceVertices.size();i++){

			int j;
			if(i == fenceVertices.size() - 1){
				j = 0;
			}else{
				j = i+1;
			}


			Vect2 X = proj.project(pos).vect2();

			// Shift origin to pos
			double x0 = X.x();
			double y0 = X.y();

			Vect2 A = fenceVertices.get(i);
			Vect2 B = fenceVertices.get(j);



			double dx = (B.x() - x0) - (A.x() - x0);
			double dy = (B.y() - y0) - (A.y() - y0);
			double dr = Math.sqrt( Math.pow(dx,2) + Math.pow(dy,2) );
			double D  = (A.x()-x0)*(B.y()-y0) - (B.x()-x0)*(A.y()-y0);

			//Discriminant
			double delta = Math.pow(Radius*dr,2) - Math.pow(D,2);

			if (delta<0){

			}
			else if(delta == 0){


			}
			else{

				double sgn;
				if(dy < 0){
					sgn = -1;
				}
				else{
					sgn = 1;
				}

				double x1 = (D*dy + sgn*dx*Math.sqrt(delta))/Math.pow(dr,2) + x0;
				double x2 = (D*dy - sgn*dx*Math.sqrt(delta))/Math.pow(dr,2) + x0;

				double y1 = (-D*dx + Math.abs(dy)*Math.sqrt(delta))/Math.pow(dr,2) + y0;
				double y2 = (-D*dx - Math.abs(dy)*Math.sqrt(delta))/Math.pow(dr,2) + y0;

				Vect2 pt1 = new Vect2(x1,y1);
				Vect2 pt2 = new Vect2(x2,y2);



				LatLonAlt LLA1 = proj.inverse(pt1,pos.alt());
				LatLonAlt LLA2 = proj.inverse(pt2,pos.alt());

				if( (x1 >= A.x() && x1 <= B.x()) || (x1 >= B.x() && x1 <= A.x()) ){
					if( (y1 >= A.y() && y1 <= B.y()) || (y1 >= B.y() && y1 <= A.y()) ){
						Pts.add(new Position(LLA1));
					}
				}

				if( (x2 >= A.x() && x2 <= B.x()) || (x2 >= B.x() && x2 <= A.x()) ){
					if( (y2 >= A.y() && y2 <= B.y()) || (y2 >= B.y() && y2 <= A.y()) ){
						Pts.add(new Position(LLA2));
					}
				}


				//System.out.println(Pts.get(0));
				//System.out.println(Pts.get(1));


			}


		}

		return Pts;
	}

	public boolean CollisionDetection(Position pos, Vect2 v,double startTime,double stopTime){

		Vect2 s  = proj.project(pos).vect2();
		Vect2 pv = new Vect2(0,0);
		boolean insideBad = false;

		if(fType == FENCE_TYPE.KEEP_OUT){
			insideBad = true;
		}

		return PolycarpDetection.Static_Collision_Detector(startTime,stopTime,fenceVertices,pv,s,v,BUFF,insideBad);
	}

	public void print(){
		System.out.println("Type: "+fType);
		System.out.println("ID:" + ID);
		System.out.println("Num vertices:"+numVertices);
		System.out.println("Floor:"+floor);
		System.out.println("Ceiling:"+ceiling);
		System.out.println("Vertices information");
		System.out.println("Convex:"+isconvex);
		for(int i=0;i<numVertices;i++){
			Position vertex = geoPolyLLA.getVertex(i);
			System.out.println("Lat :"+vertex.latitude());
			System.out.println("Lon :"+vertex.longitude());
		}

	}

}




