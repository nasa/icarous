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
import gov.nasa.larcfm.Util.Poly2D;

import java.util.*;
import java.lang.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.enums.*;


public class GeoFence{

    public int Type;
    public int ID;
    public int numVertices;
    public double floor;
    public double ceiling;
    public static double hthreshold = 0.5;
    public static double vthreshold = 0.5;
    public static double hstepback  = 50;
    public static double vstepback  = 50;
    
    
    LatLonAlt origin      = null;
    Position SafetyPoint = null;
    boolean violation     = false;
    boolean hconflict     = false;
    boolean vconflict     = false;
    boolean isconvex      = false;
    List<Position> Vertices_LLA;

    EuclideanProjection proj;
    Poly2D geoPolygon; 
    
    public GeoFence(int IDIn,int TypeIn,int numVerticesIn,double floorIn,double ceilingIn){
	Vertices_LLA = new ArrayList<Position>();
	geoPolygon   = new Poly2D();
	Type         = TypeIn;
	ID           = IDIn;
	numVertices  = numVerticesIn;
	floor        = floorIn;
	ceiling      = ceilingIn;
	SafetyPoint  = new Position();
    }

    public void AddVertex(int index,float lat,float lon){
	Position pos = new Position(lat,lon,0);
	Vertices_LLA.add(index,pos);
	
	if(origin == null){
	    origin = LatLonAlt.make((double)lat,(double)lon,0);
	    proj   = Projection.createProjection(origin);
	    LatLonAlt p = LatLonAlt.make((double)lat,(double)lon,0);
	    geoPolygon.addVertex(proj.project(p).vect2());	    	    
	}
	else{
	    LatLonAlt p = LatLonAlt.make((double)lat,(double)lon,0);
	    geoPolygon.addVertex(proj.project(p).vect2());	    	    
	}

	if(geoPolygon.size() == numVertices){
	    isconvex = geoPolygon.isConvex();
	}
				    
    }
    
    public double VerticalProximity(Position pos){

	double d1  = pos.alt_msl - floor;
	double d2  = ceiling  - pos.alt_msl;

	if(Type == 0){
	    if(d1 < 0 || d2 < 0){
		return -1;
	    }
	    else{
		return Math.min(d1,d2);
	    }	    
	}
	else{
	    if(d1 < 0 || d2 < 0){
		return Math.min(d1,d2);
	    }
	    else{
		return -1;
	    }
	}
    }
    
    public double HorizontalProximity(Position pos){
	
	double min = Double.MAX_VALUE;
	double dist = 0;
	double x1 = 0.0;
	double y1 = 0.0;
	double x2 = 0.0;
	double y2 = 0.0;
	double x3 = 0.0;
	double y3 = 0.0;
	double x0 = 0.0;
	double y0 = 0.0;
	
	double m  = 0;
	double a  = 0;
	double b  = 0;
	double c  = 0;
	int vert_i = 0;
	int vert_j = 0;

	boolean insegment;
	
	LatLonAlt p = LatLonAlt.make((double)pos.lat,(double)pos.lon,0);
	Vect2 xy     = proj.project(p).vect2();
	x3          = xy.x();
	y3          = xy.y();

			
	// Check if perpendicular intersection lies within line segment
	for(int i=0;i<geoPolygon.size();i++){
	    vert_i = i;

	    if(i == geoPolygon.size() - 1)
		vert_j = 0;
	    else
		vert_j = i + 1;

	    x1 = geoPolygon.getVertex(vert_i).x();
	    y1 = geoPolygon.getVertex(vert_i).y();

	    x2 = geoPolygon.getVertex(vert_j).x();
	    y2 = geoPolygon.getVertex(vert_j).y();
	    
	    m  = (y2 - y1)/(x2 - x1);

	    a  = m;
	    b  = -1;
	    c  = (y1-m*x1);

	    x0 = (b*(b*x3 - a*y3) - a*c)/(Math.pow(a,2)+Math.pow(b,2));
	    y0 = (a*(-b*x3 + a*y3) - a*c)/(Math.pow(a,2)+Math.pow(b,2));

	    insegment = false;

	    if( (x0 > x1) && (x0 < x2) ){
		if( ( y0 > y1) && (y0 < y2) )
		    insegment = true;
	        else if( ( y0 < y1) && (y0 > y2) )
		    insegment = true;
	    }
	    else if( (x0 < x1) && (x0 > x2) ){
		if( ( y0 > y1) && (y0 < y2) )
		    insegment = true;
	        else if( ( y0 < y1) && (y0 > y2) )
		    insegment = true;
	    }

	    if(insegment){
		dist = Math.abs(a*x3 + b*y3 + c)/Math.sqrt(Math.pow(a,2) + Math.pow(b,2));
	    }
	    else{
		double d1 = Math.sqrt( Math.pow(x3-x1,2) + Math.pow(y3-y1,2) );
		double d2 = Math.sqrt( Math.pow(x3-x2,2) + Math.pow(y3-y2,2) );
		dist = Math.min(d1,d2);
	    }

	    if(dist < min){
		min = dist;
	    }

	}
	

	return min;
	
    }

    public Position CheckViolation(Position pos){

	double hdist;
	double vdist;


	SafetyPoint.lat     = pos.lat;
	SafetyPoint.lon     = pos.lon;
	SafetyPoint.alt_msl = pos.alt_msl;

	// Keep in geofence
	if(Type == 0){
	    
	    hdist = HorizontalProximity(pos);
	    vdist = VerticalProximity(pos);


	    if(hdist <= hthreshold){
		hconflict = true;
	    }
	    else{
		hconflict = false;
	    }

	    if(hconflict){
		Vect2 Ctd   = geoPolygon.centroid();
		LatLonAlt p = LatLonAlt.make((double)pos.lat,(double)pos.lon,0);
		Vect2 xy    = proj.project(p).vect2();
		violation   = geoPolygon.contains(xy);

		Vect2 inv_slope = xy.Sub(Ctd);
		double norm_slope = inv_slope.norm();
		Vect2 step  = inv_slope.Scal(1/norm_slope*hstepback);

		Vect2 sf;
		
		if (xy.y() > Ctd.y()){
		    sf = xy.Sub(step);
		}
		else if( xy.y() < Ctd.y() ){
		    sf = xy.Add(step);
		}
		else{
		    if(xy.x() < Ctd.x())
			sf = xy.Add(step);
		    else
			sf = xy.Sub(step);
		}

		LatLonAlt lla = proj.inverse(sf,pos.alt_msl);

		SafetyPoint.lat = (float) lla.latitude();
		SafetyPoint.lon = (float) lla.longitude();
		    
	    }

	    vconflict = false;
	    if( (ceiling - pos.alt_msl) <= vthreshold){
		SafetyPoint.alt_msl = (float) (ceiling - vstepback);
		vconflict = true;
	    }
	    else if( (pos.alt_msl - floor) <= vthreshold){
		SafetyPoint.alt_msl = (float) (floor + vstepback);
		vconflict = true;
	    }

	    
	    
	    
	}
	//Keep out Geofence
	else{
	    
	    

	}

	return SafetyPoint;
	
    }

    public void print(){
	System.out.println("Type: "+Type);
	System.out.println("ID:" + ID);
	System.out.println("Num vertices:"+numVertices);
	System.out.println("Floor:"+floor);
	System.out.println("Ceiling:"+ceiling);
	System.out.println("Vertices information");
	for(int i=0;i<numVertices;i++){
	    Position vertex = Vertices_LLA.get(i);
	    System.out.println("Lat :"+vertex.lat);
	    System.out.println("Lon :"+vertex.lon);
	}

    }
    
}

class GEOFENCES{

    enum FENCESTATE{
	IDLE,INFO,VERTICES,ACK_FAIL,ACK_SUCCESS,UPDATE,REMOVE
    }
    
    public List<GeoFence> fenceList;
    FENCESTATE state;
    int numFences;
    
    public GEOFENCES(){
	fenceList  = new ArrayList<GeoFence>();
	state = FENCESTATE.IDLE;
	numFences = 0;
    }

    public GeoFence GetGeoFence(int index){

	int size = fenceList.size();
	
	if(index > 0){
	    return (GeoFence) fenceList.get(index);
	}
	else{
	    return (GeoFence) fenceList.get(size-1);
	}
    }


    public void GetNewGeoFence(ICAROUS_Interface Intf){

	GeoFence fence1 = null;
	int count = 0;
	boolean getfence = true;

	MAVLinkMessages RcvdMessages = Intf.SharedData.RcvdMessages;
	
	msg_geofence_info msg1 = RcvdMessages.msgGeofenceInfo;;

	if(msg1.msgType == 0){
	    state = FENCESTATE.INFO;
	    System.out.println("Adding fence");
	}
	else{
	    state = FENCESTATE.REMOVE;
	    System.out.println("Removing fence");
	}
	
	while(getfence){    

	    switch(state){
	    
	    case INFO:
	    
		fence1 = new GeoFence(msg1.fenceID,msg1.fenceType,msg1.numVertices,msg1.fenceFloor,msg1.fenceCeiling);
		System.out.println("Received geofence information: "+msg1.fenceCeiling);
		state = FENCESTATE.VERTICES;
		break;

	    case VERTICES:

		Intf.SetTimeout(500);
		Intf.Read();
		Intf.SetTimeout(0);
		
		if(RcvdMessages.RcvdVertex == 1){
		    msg_pointofinterest msg2 = RcvdMessages.msgPointofinterest;
		    RcvdMessages.RcvdVertex = 0;
		
		    if(msg2.id == 1 && msg2.index != count){
		      
			state  =  FENCESTATE.ACK_FAIL;
			break;
		    }

		    System.out.println("Adding vertex :"+count);
		    fence1.AddVertex(msg2.index,msg2.latx,msg2.lony);
		    count++;

		    
		    if(count == fence1.numVertices){
			state = FENCESTATE.UPDATE;
			break;
		    }
		}

		break;

	    case UPDATE:

		fenceList.add(fence1);
		GeoFence gf = this.GetGeoFence(-1);
		gf.print();
		System.out.println("Updated fence list");
		numFences = fenceList.size();
		System.out.println("Total fences in ICAROUS:"+numFences);
		state = FENCESTATE.ACK_SUCCESS;
		
		break;

	    case REMOVE:

		Iterator Itr = fenceList.iterator();

		while(Itr.hasNext()){
		    GeoFence f1 = (GeoFence) Itr.next();

		    if(f1.ID == msg1.fenceID){
			Itr.remove();
			numFences = fenceList.size();
			System.out.println("Total fences in ICAROUS:"+numFences);
			break;   
		    }
		}

		getfence = false;

		break;

	       
	    case ACK_FAIL:

		msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		msg4.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg4.value = 0;
		
		Intf.Write(msg4);
		
		break;

	    case ACK_SUCCESS:

		msg_command_acknowledgement msg5 = new msg_command_acknowledgement();

		msg5.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg5.value = 1;
		
		Intf.Write(msg5);

		state = FENCESTATE.UPDATE;
		
		break;
		
	    }//end of switch
	}//end of while
	    
    }//end of function
    
    
    
    
}
