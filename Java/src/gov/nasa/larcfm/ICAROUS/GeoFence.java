/**
 * GeoFence
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.PolyUtil;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.ACCoRD.CDPolycarp;
import gov.nasa.larcfm.ACCoRD.CDIIPolygon;
import gov.nasa.larcfm.ACCoRD.PolycarpResolution;
import gov.nasa.larcfm.IO.SeparatedInput;


import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.enums.*;
import gov.nasa.larcfm.Util.AircraftState;


public class GeoFence{
    
    public int Type;
    public int ID;
    public int numVertices;
    public double floor;
    public double ceiling;
    public static double hthreshold;
    public static double vthreshold;
    public static double hstepback;
    public static double vstepback;
    
    
    Position SafetyPoint      = null;
    Position RecoveryPoint    = null;
    boolean violation         = false;
    boolean conflict          = false;
    boolean isconvex          = false;

    EuclideanProjection proj;
    SimplePoly geoPolyLLA;
    Poly3D geoPoly3D;
    CDPolycarp geoPolyCarp;
    PolyPath geoPolyPath;
    CDIIPolygon cdp;
    PolycarpResolution pcr;
    PolyUtil pu;
    ArrayList<Vect2> fenceVertices;
    

    public double entryTime;
    public double exitTime;
    private double lookahead;
    private int stepbacktype;
                
    public GeoFence(int IDIn,int TypeIn,int numVerticesIn,double floorIn,double ceilingIn){
	geoPolyLLA     = new SimplePoly(floorIn,ceilingIn);
	geoPoly3D      = new Poly3D();
	Type           = TypeIn;
	ID             = IDIn;
	numVertices    = numVerticesIn;
	floor          = floorIn;
	ceiling        = ceilingIn;
	geoPolyCarp    = new CDPolycarp();
	geoPolyPath    = new PolyPath();
	CDPolycarp.setCheckNice(false);
	cdp            = new CDIIPolygon(geoPolyCarp);
	pcr            = new PolycarpResolution();
	pu             = new PolyUtil();
	fenceVertices  = new ArrayList<Vect2>();

	try{
	    FileReader in = new FileReader("params/icarous.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    ParameterData parameters = reader.getParametersRef();
	    
	    hthreshold   = parameters.getValue("hthreshold");
	    vthreshold   = parameters.getValue("vthreshold");
	    hstepback    = parameters.getValue("hstepback");
	    vstepback    = parameters.getValue("vstepback");
	    lookahead    = parameters.getValue("lookahead");
	    stepbacktype = parameters.getInt("stepbacktype");
	}
	catch(FileNotFoundException e){
	    System.out.println("parameter file not found");
	}
			
	
    }

    public void AddVertex(int index,float lat,float lon){

	Position pos = Position.makeLatLonAlt(lat,lon,0);
	geoPolyLLA.addVertex(pos);
	
	if(geoPolyLLA.size() == numVertices){
	    isconvex = geoPoly3D.poly2D().isConvex();

	    // [TODO] This expansion doesn't work with CDIIPolygon.detection()
	    // Expand fence if it is a keep out fence 
	    //if(Type == 1){
		//geoPolyLLA = pu.bufferedConvexHull(geoPolyLLA,hthreshold,vthreshold);
	        
	    //}
	    
	    proj       = Projection.createProjection(geoPolyLLA.getVertex(0));
	    geoPoly3D  = geoPolyCarp.makeNicePolygon(geoPolyLLA.poly3D(proj));
	    Velocity v = Velocity.makeTrkGsVs(0.0,0.0,0.0);
	    geoPolyPath.addPolygon(geoPolyLLA,v,0);

	    for(int i=0;i<geoPoly3D.size();++i){
		fenceVertices.add(geoPoly3D.getVertex(i));
	    }
	}
				    
    }

    public void CheckViolation(AircraftState acState, double currentTime,Plan FP){

	double hdist;
	double vdist;

	double alt;

	Position pos = acState.positionLast();
	Vect3 so = proj.project(pos);

	SafetyPoint = GetSafetyPoint(acState);
	
	//System.out.println("Distance from edge:"+geoPoly3D.distanceFromEdge(so));
	
	// Keep in geofence
	if(Type == 0){	    
	    if(geoPolyCarp.nearEdge(so,geoPoly3D,hthreshold,vthreshold)){
		conflict  = true;				
	    }else{
		conflict = false;
		
	    }

	    if(geoPolyCarp.definitelyInside(so,geoPoly3D)){
		violation = false;		
	    }else{

		Vect2 so_2 = so.vect2();

		Vect2 recpoint = pcr.inside_recovery_point(hthreshold,fenceVertices,so_2);
		
		LatLonAlt LLA = proj.inverse(recpoint,pos.alt());;
		RecoveryPoint = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
		
	    }
	    	   	    	    	    	    
	}
	//Keep out Geofence
	else{

	    Vect2 so_2 = so.vect2();

	    Vect2 recpoint = pcr.outside_recovery_point(hthreshold,fenceVertices,so_2);

	    LatLonAlt LLA = proj.inverse(recpoint,pos.alt());;
	    RecoveryPoint = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());


	    
	    cdp.detection(FP,geoPolyPath,0,FP.getLastTime());
	    	    
	    
	    
	    if(cdp.conflictBetween(currentTime,currentTime + lookahead)){
		
		
		//System.out.println("Conflict size:"+cdp.size());
		//System.out.println("FP last time:"+FP.getLastTime());
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
	    recPoint = RecoveryPoint.mkAlt(alt);
	}
	else if( (pos.alt() - floor) <= vthreshold){
	    alt = (floor + vstepback);                
	    recPoint = RecoveryPoint.mkAlt(alt);
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

    

    public void print(){
	System.out.println("Type: "+Type);
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

    
    

