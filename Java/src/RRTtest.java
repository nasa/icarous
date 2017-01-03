import java.io.*;
import java.util.*;
import gov.nasa.larcfm.ICAROUS.*;
import gov.nasa.larcfm.ICAROUS.RRT.node_t;
import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.Util.*;

public class RRTtest {
	public static void main(String args[]){
		RRT rrt = new RRT();

		// create an obstacles
		Poly2D obs2D = new Poly2D();
		obs2D.addVertex(3,3);
		obs2D.addVertex(6,3);
		obs2D.addVertex(6,6);
		obs2D.addVertex(3,6);
		Poly3D obs1 = new Poly3D(obs2D,-100,100);
		rrt.obstacleList.add(obs1);


		Vect3 pos = new Vect3(0,0,0); // Initial position
		Vect3 vel = new Vect3(1,0,0); // Initial velocity

		// Create root note (start node)
		rrt.root.pos = pos;
		rrt.root.vel = vel;
		rrt.root.id  = 0;
		rrt.nodeList.add(rrt.root);

		ArrayList<Vect3> TrafficPos = new ArrayList<Vect3>();
		ArrayList<Vect3> TrafficVel = new ArrayList<Vect3>();

		//Create a traffic
		Vect3 post = new Vect3(0,10,0); // traffic position
		Vect3 velt = new Vect3(-0.5,0.5,0); // traffic velocity
		TrafficPos.add(post);
		TrafficVel.add(velt);
		
		
		// Goal node
		Vect3 gpos = new Vect3(10,10,0);
		node_t goal = new node_t();
		goal.pos = gpos;
		rrt.goalNode = goal;

		// Set max, min bounds
		rrt.xmin = -100;
		rrt.xmax = 100;
		rrt.ymin = -100;
		rrt.ymax = 100;
		rrt.zmin = -10;
		rrt.zmax = 100;

		// RRT iterations
		int Nsteps = 500;
		int i=0;
		for(i=0;i<Nsteps;i++){
			rrt.RRTStep(i);

			if(rrt.CheckGoal()){
				System.out.println("Goal reached\n");
				break;
			}
		}

		System.out.println("Number of iterations:"+i);
		System.out.println("Total nodes explored:"+rrt.nodeCount);
		node_t node = rrt.closestNode;
		node_t parent;
		while(node.parent != null){
			System.out.format("%f,%f\n",node.pos.x,node.pos.y);
			parent = node.parent;
			node = parent;
		}
	}
}
