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
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.ACCoRD.CDPolycarp;
import gov.nasa.larcfm.IO.SeparatedInput;
import gov.nasa.larcfm.Util.ParameterData;

import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.enums.*;



public class GeoFence{
    
    public int Type;
    public int ID;
    public int numVertices;
    public double floor;
    public double ceiling;
    public static double hthreshold;
    public static double vthreshold;
    
    Position SafetyPoint  = null;
    boolean violation         = false;
    boolean conflict          = false;
    boolean isconvex          = false;

    EuclideanProjection proj;
    SimplePoly geoPolyLLA;
    Poly3D geoPoly3D;
    CDPolycarp geoPolyCarp;
                
    public GeoFence(int IDIn,int TypeIn,int numVerticesIn,double floorIn,double ceilingIn){
	geoPolyLLA     = new SimplePoly(floorIn,ceilingIn);
	geoPoly3D      = new Poly3D();
	Type           = TypeIn;
	ID             = IDIn;
	numVertices    = numVerticesIn;
	floor          = floorIn;
	ceiling        = ceilingIn;
	geoPolyCarp    = new CDPolycarp();
	CDPolycarp.setCheckNice(false);

	try{
	    FileReader in = new FileReader("params/Geofence.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    ParameterData parameters = reader.getParametersRef();
	    
	    hthreshold   = parameters.getValue("hthreshold");
	    vthreshold   = parameters.getValue("vthreshold");
	}
	catch(FileNotFoundException e){
	    System.out.println("Geofence parameters not found");
	}
			
	
    }

    public void AddVertex(int index,float lat,float lon){

	Position pos = Position.makeLatLonAlt(lat,lon,0);
	geoPolyLLA.addVertex(pos);
	
	if(geoPolyLLA.size() == numVertices){
	    isconvex = geoPoly3D.poly2D().isConvex();

	    proj      = Projection.createProjection(geoPolyLLA.centroid());
	    geoPoly3D = geoPolyCarp.makeNicePolygon(geoPolyLLA.poly3D(proj));
	    
	}
				    
    }
        
    public Position GetSafetyPoint(Position pos){
	
	double Min  = Double.MAX_VALUE;
	double dist = 0.0;
	double x1   = 0.0;
	double y1   = 0.0;
	double x2   = 0.0;
	double y2   = 0.0;
	double x3   = 0.0;
	double y3   = 0.0;
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
	
	LatLonAlt p = pos.zeroAlt().lla();
	Vect2 xy    = proj.project(p).vect2();
	x3          = xy.x();
	y3          = xy.y();
	Position Safe = Position.makeXYZ(0.0,0.0,0.0);
			
	// Check if perpendicular intersection lies within line segment
	for(int i=0;i<geoPoly3D.size();i++){
	    vert_i = i;

	    if(i == geoPoly3D.size() - 1)
		vert_j = 0;
	    else
		vert_j = i + 1;

	    x1 = geoPoly3D.getVertex(vert_i).x();
	    y1 = geoPoly3D.getVertex(vert_i).y();

	    x2 = geoPoly3D.getVertex(vert_j).x();
	    y2 = geoPoly3D.getVertex(vert_j).y();
	    
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

	    double hstepback = hthreshold + 2;
	    
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

		
		CDe = C.Add(CD.Scal(hstepback/CD.norm()));
		    
		LLA       = proj.inverse(CDe,pos.alt());
		Safe      = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
	    }

	    

	}

	double alt;
	double vstepback = vthreshold + 2;
	if( (ceiling - pos.alt()) <= vthreshold){
	    alt = (ceiling - vstepback);
	    Safe = Safe.mkAlt(alt);
	}
	else if( (pos.alt() - floor) <= vthreshold){
	    alt = (floor + vstepback);                
	    Safe = Safe.mkAlt(alt);
	}
	

	return Safe;
    }
    
    public void CheckViolation(Position pos){

	double hdist;
	double vdist;

	double alt;

	Vect3 so = proj.project(pos);

	System.out.println("Distance from edge:"+geoPoly3D.distanceFromEdge(so));
	
	// Keep in geofence
	if(Type == 0){	    
	    if(geoPolyCarp.nearEdge(so,geoPoly3D,hthreshold,vthreshold)){
		conflict = true;
	    }

	    if(geoPolyCarp.definitelyInside(so,geoPoly3D)){
		violation = true;
	    }
	    	   	    	    	    	    
	}
	//Keep out Geofence
	else{
	    
	    

	}
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

    
    

