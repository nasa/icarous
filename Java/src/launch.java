/**
 * Example file to run ICAROUS with a sample mission
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

import java.io.*;
import java.util.*;
import gov.nasa.larcfm.ICAROUS.*;
import gov.nasa.larcfm.MISSION.*;
import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.IO.*;
import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ICAROUS.RRT.node_t;


public class launch{
    
    public static void main(String args[]){
	
    
	Demo test = new Demo();
	Icarous IcarousMain = new Icarous(args,test);
	IcarousMain.run();
	
	
		/*
    	RRT rrt = new RRT();

    	// create bounding box

    	Poly2D box = new Poly2D();
    	box.addVertex(-25,-30);
    	box.addVertex(24,-26);
    	box.addVertex(19,13);
    	box.addVertex(-30,9);

    	Poly3D bbox = new Poly3D(box,-10,50);
    	rrt.boundingBox = bbox;

    	// obstacles
    	Poly2D obs2D = new Poly2D();
    	obs2D.addVertex(3,4);
    	obs2D.addVertex(1.8,-3.8);
    	obs2D.addVertex(11.3,-2.12);
    	//obs2D.addVertex(30,60);
    	Poly3D obs1 = new Poly3D(obs2D,-100,100);

    	//Poly2D obs2D_2;
    	//obs2D_2.addVertex(30,0);
    	//obs2D_2.addVertex(60,0);
    	//obs2D_2.addVertex(60,30);
    	//obs2D_2.addVertex(30,30);
    	//Poly3D obs2(obs2D_2,-100,100);

    	//RRT.obstacleList.push_back(obs1);
    	//RRT.obstacleList.push_back(obs2);
    	
    	rrt.obstacleList.add(obs1);

    	int Nsteps = 500;

    	Vect3 pos = new Vect3(0,0,0);
    	Vect3 vel = new Vect3(1,0,0);
    	
    	
    	rrt.root.pos = pos;
    	rrt.root.vel = vel;
    	rrt.root.id  = 0;
    	rrt.nodeList.add(rrt.root);
    	
    	ArrayList<Vect3> TrafficPos = new ArrayList<Vect3>();
    	ArrayList<Vect3> TrafficVel = new ArrayList<Vect3>();

    	Vect3 gpos = new Vect3(8.3,0.6,0);
    	node_t goal = new RRT.node_t();
    	goal.pos = gpos;


    	rrt.goalNode = goal;

    	rrt.xmin = -100;
    	rrt.xmax = 100;
    	rrt.ymin = -100;
    	rrt.ymax = 100;
    	rrt.zmin = -10;
    	rrt.zmax = 100;

    	rrt.dT = 1;


    	int i=0;
    	for(i=0;i<Nsteps;i++){
    		rrt.RRTStep(i);

    		if(rrt.CheckGoal()){
    			System.out.println("Goal reached\n");
    			break;
    		}
    	}

    	System.out.println(i);
    	System.out.println(rrt.nodeCount);
    	node_t node = rrt.closestNode;
    	node_t parent;
    	while(node.parent != null){
    		
    		System.out.format("%f,%f,",node.pos.x,node.pos.y);
    		System.out.println(rrt.CheckDirectPath2Goal(node));
    		parent = node.parent;
    		node = parent;
    	}*/
	
    }// end of main    
}// end of class
