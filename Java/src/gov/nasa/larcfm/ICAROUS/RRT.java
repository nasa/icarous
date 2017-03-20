package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.ICAROUS.GeoFence.FENCE_TYPE;
import gov.nasa.larcfm.Util.*;
import java.util.*;

public class RRT {

	static public class node_t{
		public int id;
		public Vect3 pos;
		public Vect3 vel;
		public ArrayList<Vect3> trafficPos;
		public ArrayList<Vect3> trafficVel;

		boolean goal;
		double g,h;
		public ArrayList<node_t> children;
		public node_t parent;
		
		public node_t(){
			trafficPos = new ArrayList<Vect3>();
			trafficVel = new ArrayList<Vect3>();
			children   = new ArrayList<node_t>();  
			parent     = null;
		}
	};
	
	public int xmax, xmin;
	public int ymax, ymin;
	public int zmax, zmin;
	public int Tstep;
	public double dT;
	public ArrayList<GeoFence> fenceList;
	public Poly3D boundingBox;
	public ArrayList<Poly3D> obstacleList;
	public ArrayList<node_t> nodeList;
	public EuclideanProjection proj;
	public CDPolycarp geoPolycarp;
	public int nodeCount;
	public node_t root;
	public int trafficSize;
	public double closestDist;
	public double daaLookAhead;
	public node_t closestNode;
	public node_t goalNode;
	public Daidalus DAA;
	public KinematicMultiBands KMB;
	public boolean goalreached;
	public Random RD;
	double startTime;
	double maxD;
	double maxInputNorm;
	
	public RRT(){
		nodeCount = 0;
		trafficSize = 0;
		xmin = 0;
		xmax = 0;
		ymin = 0;
		ymax = 0;
		zmin = 0;
		zmax = 0;
		Tstep = 5;
		dT = 1;
		maxInputNorm = 1;
		RD = new Random();
		closestDist = Double.MAX_VALUE;
		
		DAA = new Daidalus();
		if(!DAA.parameters.loadFromFile("params/DaidalusQuadConfig.txt")){
			System.out.print("error:no file found\n");
		}
		daaLookAhead = DAA.parameters.getLookaheadTime("s");
		obstacleList = new ArrayList<Poly3D>();
		nodeList     = new ArrayList<node_t>();
		
		fenceList    = new ArrayList<GeoFence>();
		geoPolycarp  = new CDPolycarp();
		root         = new node_t();
		goalNode     = new node_t();
		
		RD           = new Random();
		maxD         = DAA.parameters.alertor.getParameters().getValue("D", "m");
	}
	
	public RRT(List<GeoFence> fences,Position initialPos,Velocity initialVel,
			 ArrayList<Position> trafficPos,ArrayList<Velocity> trafficVel,int stepT,double dt,double inputNormMax){

		obstacleList = new ArrayList<Poly3D>();
		nodeList     = new ArrayList<node_t>();
		fenceList    = (ArrayList<GeoFence>) fences;
		geoPolycarp  = new CDPolycarp();
		root         = new node_t();
		goalNode     = new node_t();
		
		RD    = new Random();
		Tstep = stepT;
		dT    = dt;
		
		proj = Projection.createProjection(initialPos.mkAlt(0));

		for(int i=0;i<fenceList.size();++i){
			if(fenceList.get(i).fType == FENCE_TYPE.KEEP_IN){
				boundingBox = fenceList.get(i).geoPolyLLA2.poly3D(proj);
				break;
			}
		}

		xmin = -100;
		xmax = 100;
		ymin = -100;
		ymax = 100;
		zmin = -1;
		zmax = 10;
		maxInputNorm = inputNormMax;

		for(int i = 0;i<fenceList.size();++i){
			GeoFence GF = fenceList.get(i);
			if(GF.fType != FENCE_TYPE.KEEP_IN){
				obstacleList.add((GF.geoPolyLLA2.poly3D(proj)));
			}
		}

		Vect3 initPosR3 = proj.project(initialPos);

		nodeCount = 0;
		
		root.id = nodeCount;
		root.pos = initPosR3;
		root.vel = new Vect3(initialVel.x,initialVel.y,initialVel.z);
		closestNode = root;
		root.parent = null;
		
		for(int i=0;i<trafficPos.size();++i){
			Vect3 tPos = proj.project(trafficPos.get(i));
			root.trafficPos.add(tPos);
		}
		
		for(int i=0;i<trafficVel.size();++i){
			Velocity vel = trafficVel.get(i);
			Vect3 tVel = new Vect3(vel.x,vel.y,vel.z);
			root.trafficVel.add(tVel);
		}
		
		
		trafficSize = trafficPos.size();
		nodeList.add(root);

		closestDist = Double.MAX_VALUE;
		closestNode = root;

		DAA = new Daidalus();
		DAA.parameters.loadFromFile("params/DaidalusQuadConfig.txt");
		daaLookAhead = DAA.parameters.getLookaheadTime("s");
		startTime =  (double)System.nanoTime()/1E9;
		maxD = DAA.parameters.alertor.getLevel(1).getDetector().getParameters().getValue("D", "m");
		maxD = Units.to(Units.m, maxD);
		
	}
	
	public double NodeDistance(node_t A, node_t B){
		return Math.sqrt(Math.pow((A.pos.x - B.pos.x),2) + Math.pow((A.pos.y - B.pos.y),2));
	}
	
	public void GetInput(node_t nn, node_t qn,double U[]){
		double dx, dy, dz;
		double norm;

		dx = qn.pos.x - nn.pos.x;
		dy = qn.pos.y - nn.pos.y;
		dz = qn.pos.z - nn.pos.z;

		norm = Math.sqrt(Math.pow(dx,2) + Math.pow(dy,2) + Math.pow(dz,2));
		if (norm > maxInputNorm){
			U[0] = dx/norm*maxInputNorm;
			U[1] = dy/norm*maxInputNorm;
			U[2] = dz/norm*maxInputNorm;
		}
		else{
			U[0] = dx;
			U[1] = dy;
			U[2] = dz;
		}
	}
	
	public node_t FindNearest(node_t query){
		double minDist = 10000;
		double dist;
		node_t nearest = null;

		if ( nodeList.size() == 0 ){
			nearest = query;
		}
		else{
			for(int i=0;i<nodeList.size();++i){
				node_t nd = nodeList.get(i);
				dist = NodeDistance(nd,query);

				if(dist < minDist){
					minDist = dist;
					nearest = nd;
				}
			}
		}

		return nearest;
	}
	
	public void F(double X[], double U[],double Y[]){

		double Kc = 0.3;

		Y[0] = X[1];
		Y[1] = -Kc*(X[1] - U[0]);
		Y[2] = X[3];
		Y[3] = -Kc*(X[3] - U[1]);
		Y[4] = X[5];
		Y[5] = -Kc*(X[5] - U[2]);

		// Constant velocity for traffic
		for(int i=0;i<trafficSize;++i){
			Y[6+(6*i)+0]   = X[6+(6*i)+1];
			Y[6+(6*i)+1]   = 0;
			Y[6+(6*i)+2]   = X[6+(6*i)+3];
			Y[6+(6*i)+3]   = 0;
			Y[6+(6*i)+4]   = X[6+(6*i)+5];
			Y[6+(6*i)+5]   = 0;
		}
	}
	
	public node_t MotionModel(node_t nearest, double U[]){

		
		Vect3 pos = nearest.pos;
		Vect3 vel = nearest.vel;
		ArrayList<Vect3> trafficPos = nearest.trafficPos;
		ArrayList<Vect3> trafficVel = nearest.trafficVel;
		
		int Xsize = 6+trafficSize*6;

		double X[]   = new double[Xsize];
		double X_p[] = new double[Xsize];
		double Y[]   = new double[Xsize];
		double k1[]  = new double[Xsize];
		double k2[]  = new double[Xsize];
		
		boolean fenceConflict = false;
		boolean trafficConflict = false;
		
		ArrayList<Vect3> newTrafficPos = null;
		
		Vect3 newPos = new Vect3(0,0,0);
		Vect3 newVel = new Vect3(0,0,0);
	
		X[0] = pos.x;
		X[1] = vel.x;
		X[2] = pos.y;
		X[3] = vel.y;
		X[4] = pos.z;
		X[5] = vel.z;

		
		for(int i=0;i<trafficSize;++i){
			X[6+(6*i)+0] =  trafficPos.get(i).x;
			X[6+(6*i)+1] =  trafficVel.get(i).x;
			X[6+(6*i)+2] =  trafficPos.get(i).y;
			X[6+(6*i)+3] =  trafficVel.get(i).y;
			X[6+(6*i)+4] =  trafficPos.get(i).z;
			X[6+(6*i)+5] =  trafficVel.get(i).z;
			
		}

		for(int i=0;i<Tstep;++i){
			F(X,U,Y);

			for(int j=0;j<Xsize;j++){
				k1[j] = Y[j]*dT;
				X_p[j] = X[j] + k1[j];
			}

			F(X_p,U,Y);
			for(int j=0;j<Xsize;j++){
				k2[j] = Y[j]*dT;
			}

			for(int j=0;j<Xsize;j++){
				X[j] = X[j] + 0.5*(k1[j] + k2[j]);
			}

			newPos = new Vect3(X[0],X[2],X[4]);
			newVel = new Vect3(X[1],X[3],X[5]);
			
			newTrafficPos = new ArrayList<Vect3>();
			for(int j=0;j<trafficSize;++j){
				Vect3 newTraffic = new Vect3(X[6+(6*j)+0],X[6+(6*j)+2],X[6+(6*j)+4]);
				newTrafficPos.add(newTraffic);
			}
			
			if(CheckFenceCollision(newPos)){
				fenceConflict = true;
			}
			
			if(trafficSize > 0){
				trafficConflict = CheckTrafficCollision(newPos, newVel, newTrafficPos, trafficVel);
			}
			
			if(fenceConflict || trafficConflict){
				node_t newNode = new node_t();
				newNode.id = -1;
				return newNode;
			}
		}

		// This part is computing bands to detect conflicts
		// This is not necessary if traffic conflicts are detected pointwise in the above loop
		/*
		if(trafficSize > 0){
			boolean checkTurn;
			if(nearest.id > 0){
				checkTurn = true;
			}
			else{
				checkTurn = false;
			}
			if(CheckTrafficCollisionWithBands(checkTurn,newPos,newVel,newTrafficPos,trafficVel,vel)){
				node_t newNode = new node_t();
				newNode.id = -1;
				return newNode;
			}
		}
		*/


		node_t newNode = new node_t();
		nodeCount++;
		newNode.id  = nodeCount;
		newNode.pos = newPos;
		newNode.vel = newVel;
		newNode.trafficPos = newTrafficPos;
		newNode.trafficVel = trafficVel;

		return newNode;
	}
	
	public void RRTStep(int i){

		double X[] = new double[3];
		// Generate random number
		int rangeX = xmax - xmin;
		int rangeY = ymax - ymin;

		X[0] = xmin + RD.nextDouble() * rangeX;
		X[1] = ymin + RD.nextDouble() * rangeY;
		X[2] = root.pos.z;

		//TODO: configure hard coded value for z dimension 
		node_t rd = new node_t();
		rd.pos = new Vect3(X[0],X[1],X[2]);
		
		node_t nearest = FindNearest(rd);
		node_t newNode;

		if(CheckDirectPath2Goal(nearest)){
			newNode = goalNode;
			nodeCount++;
			newNode.id = nodeCount;
		}
		else{
			double U[] = new double[3];
			GetInput(nearest,rd,U);
			newNode = MotionModel(nearest,U);
			
			if(newNode.id < 0){
				return;
			}
		}
		
		nearest.children.add(newNode);
		newNode.parent= nearest;
		nodeList.add(newNode);

	}
	
	public boolean CheckFenceCollision(Vect3 qPos){
		
		for(int i=0;i<obstacleList.size();++i){
			Poly3D GF = obstacleList.get(i);
			if(geoPolycarp.definitelyInside(qPos, GF)){
				return true;
			}
		}

		if(boundingBox != null){
			if(boundingBox.size() > 2){
				if(!geoPolycarp.definitelyInside(qPos,boundingBox)){
					return true;
				}
			}
		}
		return false;
	}
	
	public boolean CheckProjectedFenceConflict(node_t qnode,node_t goal){
		
		for(int ip=0;ip<obstacleList.size();++ip){
			Poly3D poly = obstacleList.get(ip);
			int sizePoly = poly.size();
			for(int i=0;i<sizePoly;i++){
				int j;
				if(i == sizePoly - 1){
					j = 0;
				}
				else{
					j = i+1;
				}

				boolean intCheck = LinePlanIntersection(poly.getVertex(i),poly.getVertex(j),
										  	         poly.getBottom(),poly.getTop(),
													 qnode.pos,goal.pos);
				if(intCheck){
					return true;
				}
			}
		}

		return false;
	}
	
	public boolean CheckTrafficCollision(Vect3 qPos,Vect3 qVel,ArrayList<Vect3> TrafficPos,ArrayList<Vect3> TrafficVel){
		
		double timeElapsed1 = (double)System.nanoTime()/1E9 - startTime;
		
		Position so  = Position.makeXYZ(qPos.x,"m",qPos.y,"m",qPos.z,"m");
		Velocity vo  = Velocity.makeVxyz(qVel.x,qVel.y,"m/s",qVel.z,"m/s");
		
		DAA.setOwnshipState("Ownship",so,vo,timeElapsed1);

		double violationTime;
		double alertTime = DAA.parameters.alertor.getLevel(1).getAlertingTime();
		double trafficDist = Double.MAX_VALUE;
		
		for(int i=0;i<TrafficPos.size();++i){
			Vect3 pos = TrafficPos.get(i);
			Vect3 vel = TrafficVel.get(i);
			Position si = Position.makeXYZ(pos.x,"m",pos.y,"m",pos.z,"m");
			Velocity vi = Velocity.makeVxyz(vel.x,vel.y,"m/s",vel.z,"m/s");
			DAA.addTrafficState("Traffic"+i,si,vi);

			KMB = DAA.getKinematicMultiBands();
			
			if(!KMB.timeIntervalOfViolation(1).isEmpty()){
				violationTime = KMB.timeIntervalOfViolation(1).low;
			}else{
				violationTime = Double.NaN;
			}
			
			double distH = so.distanceH(si);

			if(distH < trafficDist){
				trafficDist = distH;
			}
			
			
			if(Double.isFinite(violationTime) && violationTime < alertTime){
				return true;
			}
		}
		
		if(trafficDist < maxD){
			//System.out.print("In cylinder\n");
			//return true;
		}

		return false;
	}
	
	public boolean CheckTrafficCollisionWithBands(boolean CheckTurn,Vect3 qPos,Vect3 qVel,
		    ArrayList<Vect3> TrafficPos,ArrayList<Vect3> TrafficVel,Vect3 oldVel){

		
		double timeElapsed1 = (double)System.nanoTime()/1E9 - startTime;
		
		Position so  = Position.makeXYZ(qPos.x,"m",qPos.y,"m",qPos.z,"m");
		Velocity vo  = Velocity.makeVxyz(qVel.x,qVel.y,"m/s",qVel.z,"m/s");
		
		DAA.setOwnshipState("Ownship",so,vo,timeElapsed1);

		double trafficDist = Double.MAX_VALUE;

		for(int i=0;i<TrafficPos.size();++i){
			Vect3 pos = TrafficPos.get(i);
			Vect3 vel = TrafficVel.get(i);
			Position si = Position.makeXYZ(pos.x,"m",pos.y,"m",pos.z,"m");
			Velocity vi = Velocity.makeVxyz(vel.x,vel.y,"m/s",vel.z,"m/s");
			DAA.addTrafficState("Traffic"+i,si,vi);

			//printf("Traffic pos:%f,%f\n",itP->x,itP->y);
			//printf("Traffic heading:%f\n",vi.track("degree"));

			double distH = so.distanceH(si);

			if(distH < trafficDist){
				trafficDist = distH;
			}
		}

		if(trafficDist < maxD){
			System.out.print("In cylinder\n");
			return true;
		}

		double qHeading = vo.track("degree");
		
		//printf("curr heading:%f\n",qHeading);
		//printf("old heading:%f\n",oldHeading);

		KMB = DAA.getKinematicMultiBands();
		if(KMB.regionOfTrack(DAA.getOwnshipState().track()).isConflictBand()){
			//Check if current heading is in conflict band
			return true;
		}
		else if(CheckTurn){
			Velocity vo0 = Velocity.makeVxyz(oldVel.x,oldVel.y,"m/s",oldVel.z,"m/s");
			double oldHeading = vo0.track("degree");
			if(KMB.regionOfTrack(vo0.trk()).isConflictBand()){
				// Check if old heading lies in current conflict band
				return true;
			}
			
			// Check if turning crosses a conflict band
			for(int ib=0;ib<KMB.trackLength();++ib){
				if(KMB.trackRegion(ib) != BandsRegion.NONE ){
					Interval ii = KMB.track(ib,"deg");
					if(CheckTurnConflict(ii.low,ii.up,qHeading,oldHeading)){
							//printf("RRT:turn conflict old:%f, new:%f [%f,%f]\n",oldHeading,qHeading,ii.low,ii.up);
							return true;
					}
				}
			}
		}
		
		return false;
	}

	public boolean CheckTurnConflict(double low,double high,double newHeading,double oldHeading){

		
		if(newHeading < 0){
			newHeading = 360 + newHeading;
		}
		
		if(oldHeading < 0){
			oldHeading = 360 + oldHeading;
		}
		
		// Get direction of turn
		double psi   = newHeading - oldHeading;
		double psi_c = 360 - Math.abs(psi);
		boolean leftTurn = false;
		boolean rightTurn = false;
		if(psi > 0){
			if(Math.abs(psi) > Math.abs(psi_c)){
				leftTurn = true;
			}
			else{
				rightTurn = true;
			}
		}else{
			if(Math.abs(psi) > Math.abs(psi_c)){
				rightTurn = true;
			}
			else{
				leftTurn = true;
			}
		}

		double A,B,X,Y,diff;
		if(rightTurn){
			diff = oldHeading;
			A = oldHeading - diff;
			B = newHeading - diff;
			X = low - diff;
			Y = high - diff;

			if(B < 0){
				B = 360 + B;
			}

			if(X < 0){
				X = 360 + X;
			}

			if(Y < 0){
				Y = 360 + Y;
			}

			if(A < X && B > Y){
				return true;
			}
			
		}else{
			diff = 360 - oldHeading;
			A    = oldHeading + diff;
			B    = newHeading + diff;
			X = low + diff;
			Y = high + diff;

			if(B > 360){
				B = B - 360;
			}

			if(X > 360){
				X = X - 360;
			}

			if(Y > 360){
				Y = Y - 360;
			}

			if(A > Y && B < X){
				return true;
			}	
		}

		return false;
	}



	public boolean CheckDirectPath2Goal(node_t qnode){
		//TODO: add check against fences also
		//TODO: add velocity towards goal
		//TODO: change speed to a parameter
		double speed = maxInputNorm;
		Vect3 A = qnode.pos;
		Vect3 B = goalNode.pos;
		Vect3 AB = B.Sub(A);
		AB = AB.mkZ(0);
		double norm = AB.norm();
		if(norm > 0){
			AB = AB.Scal(speed/norm);
		}

		if(CheckProjectedFenceConflict(qnode,goalNode)){
			return false;
		}

		if(trafficSize > 0){
			node_t parent = qnode.parent;
			boolean CheckTurn = false;
			Vect3 vel = new Vect3(0,0,0);
			if(parent != null){
				vel = parent.vel;
			}
			if(CheckTrafficCollisionWithBands(CheckTurn,qnode.pos,AB,qnode.trafficPos,qnode.trafficVel,vel)){
				return false;
			}
			else{
				return true;
			}
		}
		else{
			return true;
		}

	}

	public boolean CheckGoal(){
		node_t lastNode = nodeList.get(nodeList.size()-1);
		Vect3 diff = lastNode.pos.Sub(goalNode.pos);
		double mag = diff.norm();
		if(mag <= closestDist){
			closestDist = mag;
			closestNode = lastNode;
			if(CheckDirectPath2Goal(closestNode)){
				System.out.print("found direct path to goal\n");
				goalreached = true;
				return true;
			}
		}
		if( mag < 3 ){
			System.out.print("found goal\n");
			goalreached = true;
			return true;
		}else{
			goalreached = false;
			return false;
		}
	}

	public void SetGoal(Position goal){
		goalNode.pos = proj.project(goal);
	}

	public void SetGoal(node_t goal){
		goalNode = goal;
	}


	public boolean LinePlanIntersection(Vect2 A,Vect2 B,double floor,double ceiling,Vect3 CurrPos,Vect3 NextWP){

		double x1 = A.x;
		double y1 = A.y;
		double z1 = floor;

		double x2 = B.x;
		double y2 = B.y;
		double z2 = ceiling;

		Vect3 l0 = new Vect3(CurrPos.x, CurrPos.y, CurrPos.z);
		Vect3 p0 = new Vect3(x1, y1, z1);

		Vect3 n = new Vect3(-(z2 - z1) * (y2 - y1), (z2 - z1) * (x2 - x1), 0);
		Vect3 l = new Vect3(NextWP.x - CurrPos.x, NextWP.y - CurrPos.y, NextWP.z - CurrPos.z);

		double d = (p0.Sub(l0).dot(n)) / (l.dot(n));

		Vect3 PntI = l0.Add(l.Scal(d));


		Vect3 OA = new Vect3(x2 - x1, y2 - y1, 0);
		Vect3 OB = new Vect3(0, 0, z2 - z1);
		Vect3 OP = PntI.Sub(p0);
		Vect3 CN = NextWP.Sub(CurrPos);
		Vect3 CP = PntI.Sub(CurrPos);

		double proj1 = OP.dot(OA) / Math.pow(OA.norm(), 2);
		double proj2 = OP.dot(OB) / Math.pow(OB.norm(), 2);
		double proj3 = CP.dot(CN) / Math.pow(CN.norm(), 2);

		if (proj1 >= 0 && proj1 <= 1) {
			if (proj2 >= 0 && proj2 <= 1) {
				if (proj3 >= 0 && proj3 <= 1)
					return true;
			}
		}

		return false;
	}

	public Plan GetPlan(){

		double speed = maxInputNorm;
		node_t node = closestNode;
		node_t parent;
		ArrayList<node_t> path = new ArrayList<node_t>();
		
		if(!goalreached){
			node = goalNode;
			node.parent = closestNode;
		}
		
		while(node != null){
			System.out.format("x,y:%f,%f\n",node.pos.x,node.pos.y);
			path.add(node);
			node = node.parent;
		}

		Plan newRoute = new Plan();
		int count = 0;
		double ETA = 0;
		for(int i=path.size()-1;i>=0;--i){
			Position wp = new Position(proj.inverse(path.get(i).pos));
			if(count == 0){
				ETA = 0;
			}
			else{
				Position prevWP = newRoute.point(count-1).position();
				double distH    = wp.distanceH(prevWP);
				ETA             = ETA + distH/speed;
			}

			NavPoint np = new NavPoint(wp,ETA);
			newRoute.addNavPoint(np);
			count++;
		}

		return newRoute;
	}
	
}
