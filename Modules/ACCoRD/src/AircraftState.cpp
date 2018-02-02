/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Projection.h"
#include <string>
#include <cmath>
#include <sstream>
#include "format.h"


namespace larcfm {
  using std::string;
  using std::cout;
  using std::endl;
  using std::pair;
  
 
  double AircraftState::minClimbVelocity = Units::from("fpm",150);    // used to determine when a climb/descent occurs

  double AircraftState::MAX_RELATIVE_DIFF = 0.10;

//  const int AircraftState::DEFAULT_BUFFER_SIZE = 10;
  bool AircraftState::projectVelocity = true;

 
    AircraftState::AircraftState(const string& name, int buffer_size) : error("AircraftState") {
      init(name, buffer_size);
    }

    AircraftState::AircraftState() : error("AircraftState") {
    	init("Aircraft", DEFAULT_BUFFER_SIZE);
    }
    
    AircraftState::AircraftState(const string& id) : error("AircraftState") {
    	init(id, DEFAULT_BUFFER_SIZE);
    }
    
    AircraftState::AircraftState(const AircraftState& orig) : error("AircraftState") {
    	init(orig.id, orig.bufferSize);
        for (int i = 0; i < bufferSize; i++) {
          s_list[i] = orig.s_list[i];
          v_list[i] = orig.v_list[i];
          t_list[i] = orig.t_list[i];
        }
        error = orig.error; // this might be a problem -- if so, remove
        sp = orig.sp;
        oldest = orig.oldest;
        sz = orig.sz;
        projection_initialized = orig.projection_initialized;
        projection_done = orig.projection_done;
        regression_done = orig.regression_done;
        if (projection_done) {
          for (int i = 0; i < bufferSize; i++) {
            projS2[i] = orig.projS2[i];
            projH[i] = orig.projH[i];
            projV2[i] = orig.projV2[i];
            projVZ[i] = orig.projVZ[i];
            projT[i] = orig.projT[i];
          }
        }
     }

    AircraftState::~AircraftState() {
    	//fpln("@@ AircraftState Destructor "+id);
    	if (s_list) delete [] s_list;
    	if (v_list) delete [] v_list;
    	if (t_list) delete [] t_list;
    	if (projS2) delete [] projS2;
    	if (projH) delete [] projH;
    	if (projV2) delete [] projV2;
    	if (projVZ) delete [] projVZ;
    	if (projT) delete [] projT;
    }

    void AircraftState::init(string name, int buffer_size) {
      sz = 0;
      bufferSize = buffer_size < 1 ? DEFAULT_BUFFER_SIZE : buffer_size;
      s_list = new Position[bufferSize];
      v_list = new Velocity[bufferSize];
      t_list = new double[bufferSize];
      projS2 = new Vect2[bufferSize];
      projH = new double[bufferSize];
      projV2 = new Vect2[bufferSize];
      projVZ = new double[bufferSize];
      projT = new double[bufferSize];
      // initialize arrays
      for (int i = 0; i < bufferSize; i++){
    	  t_list[i] = 0.0;
    	  projH[i] = 0.0;
    	  projVZ[i] = 0.0;
    	  projT[i] = 0.0;
      }
      sp = Projection::createProjection(0,0,0);
      oldest = 0;
      id = name;
      projection_initialized = false;
      projection_done = false;
      regression_done = false;
 	  ls_t = -1000002.0;
 	  //fpln(" AircraftState::init: name = "+name+" ls_t = "+Fm1(ls_t));
	  lastZeroTrackRateThreshold = Units::from("deg/s",0.1);
     }
    
    AircraftState& AircraftState::operator=(const AircraftState& rhs) {
    	// clear old arrays (size may have changed)
    	if (this == &rhs) return *this;
    	bool bufferchange = (bufferSize != rhs.bufferSize);
    	bufferSize = rhs.bufferSize;
    	id = rhs.id;
        error = rhs.error; // this might be a problem -- if so, remove

    	if (s_list && bufferchange) {
    		delete [] s_list;
    		s_list = new Position[bufferSize];
    	}
    	if (v_list && bufferchange) {
    		delete [] v_list;
    		v_list = new Velocity[bufferSize];
    	}
    	if (t_list && bufferchange) {
    		delete [] t_list;
    		t_list = new double[bufferSize];
    	}
    	if (projS2 && bufferchange) {
    		delete [] projS2;
    	      projS2 = new Vect2[bufferSize];
    	}
    	if (projH && bufferchange) {
    		delete [] projH;
    	      projH = new double[bufferSize];
    	}
    	if (projV2 && bufferchange) {
    		delete [] projV2;
  	      projV2 = new Vect2[bufferSize];
    	}
    	if (projVZ && bufferchange) {
    		delete [] projVZ;
    	      projVZ = new double[bufferSize];
    	}
    	if (projT && bufferchange) {
    		delete [] projT;
  	      projT = new double[bufferSize];
    	}

        for (int i = 0; i < bufferSize; i++) {
          s_list[i] = rhs.s_list[i];
          v_list[i] = rhs.v_list[i];
          t_list[i] = rhs.t_list[i];
        }
        sp = rhs.sp;
        oldest = rhs.oldest;
        sz = rhs.sz;
        projection_initialized = rhs.projection_initialized;
        projection_done = rhs.projection_done;
        regression_done = rhs.regression_done;
        if (projection_done) {
          for (int i = 0; i < bufferSize; i++) {
            projS2[i] = rhs.projS2[i];
            projH[i] = rhs.projH[i];
            projV2[i] = rhs.projV2[i];
            projVZ[i] = rhs.projVZ[i];
            projT[i] = rhs.projT[i];
          }
        }
    	recentInd = rhs.recentInd;
    	horizvelintercept = rhs.horizvelintercept;
    	horizvelslope = rhs.horizvelslope;
    	vertvelintercept = rhs.vertvelintercept;
    	vertvelslope = rhs.vertvelslope;
    	ls_t = rhs.ls_t;
    	ls_trk = rhs.ls_trk;
		lastZeroTrackRateThreshold = rhs.lastZeroTrackRateThreshold;
	   	//fpln("$$$$$$$$$$$$$$$$$$ AircraftState::operator= ls_t = "+Fm1(ls_t));
	   	return *this;
    }

    string AircraftState::name() const {
    	return id;
    }
    
	bool AircraftState::checkLatLon(bool ll) const {
		if (size() == 0) {
			return true;
		}
		return ll == s_list[oldest].isLatLon();
	}


	bool AircraftState::isLatLon() const {
		return checkLatLon(true);
	}


    AircraftState AircraftState::copy() const {
      AircraftState a(id, bufferSize);
      for (int i = 0; i < bufferSize; i++) {
        a.s_list[i] = s_list[i];
        a.v_list[i] = v_list[i];
        a.t_list[i] = t_list[i];
      }
      a.sp = sp;
      a.oldest = oldest;
      a.sz = sz;
      if (projection_done) {
        for (int i = 0; i < bufferSize; i++) {
          a.projS2[i] = projS2[i];
          a.projH[i] = projH[i];
          a.projV2[i] = projV2[i];
          a.projVZ[i] = projVZ[i];
          a.projT[i] = projT[i];
        }
      }
      a.projection_initialized = projection_initialized;
      a.projection_done = projection_done;
      a.regression_done = regression_done;
      a.ls_t = ls_t;  // EXPERIMENTAL
      a.ls_trk = ls_trk;
      //fpln("$$$$$$$$$$$$$$$$$$$$$$$$$$ AircraftState::copy() a.ls_t = "+Fm1(a.ls_t));
      return a;
    }
    
    

    
    int AircraftState::getBufferSize() const {
    	return bufferSize;
    }

    void AircraftState::clear() {
    	sz = 0;
    	oldest = 0;
    	ls_t = -10000001;
    	//fpln(" $$$$$$$$$$$$$$$$$$$$ AircraftState::clear ls_t = -10000001");
    }
    
    int AircraftState::find(double time) const {
    	for (int i = 0; i < sz; i++) {
    		double tm = this->time(i);
    		if (time == tm) {
    			return i;
    		}
    		if (time < tm) {
    			return -i-1;
    		}
    	}
    	return -sz-1;
    }
    
    int AircraftState::size() const {
    	return sz;
    }
    


	void AircraftState::add(const Position& ss, const Velocity& vv, double tm) {
		int i;
		if (sz >= 1 && tm <= timeLast()) {
			i = find(tm);
			if (i >= 0 ) {
				s_list[ext2int(i)] = ss;
				v_list[ext2int(i)] = vv;
				//t_list[ext2int(i)] = tm;  // unneeded, times must be the same.
			} else {
				if (i < -1) { 
					i = -i - 2;
					// invariant: i /= size()
					insertAt(i, ss, vv, tm);
			   	} else if (sz < bufferSize) { // note i == -1 (insert before beginning)
			   		oldest = ext2int(-1);
					s_list[oldest] = ss;
					v_list[oldest] = vv;
					t_list[oldest] = tm;
					sz++;
				} // note on this empty else clause: at this point an attempt is 
				  // being made to insert something before any other time into
				  // a list that is already full, this does nothing, so that
				  // is precisely what I do, nothing.
			}
		} else { // the list is empty or we are adding to the end.
			s_list[ext2int(sz)] = ss;
			v_list[ext2int(sz)] = vv;
			t_list[ext2int(sz)] = tm;
			sz++;
			if (sz > bufferSize) {
				sz = bufferSize;
				oldest = ext2int(1);
			}
		}
		projection_done = false;
		// EXPERIMENTAL
		//fpln(id+" $$$$$$$ AircraftState::add: sz = "+Fm1(sz)+" lastZeroTrackRateThreshold = "+Units::str("deg/s",lastZeroTrackRateThreshold));
		if (sz == 1) {
			ls_t = tm;
			ls_trk = vv.trk();
			//fpln(id+" INIT:at tm = "+Fm1(tm)+" ls_t = "+Fm1(ls_t)+" ls_trk = "+Units::str("deg",ls_trk) );
		} else {
			//fp(id+" $$$$$$$ AircraftState::add: Util::turnDelta(ls_trk,vv.track())/(tm - ls_t) = "+Units::str("deg",Util::turnDelta(ls_trk,vv.track())/(tm - ls_t)));
			if (tm > ls_t && Util::turnDelta(ls_trk,vv.trk())/(tm - ls_t) < lastZeroTrackRateThreshold) {
				ls_t = tm;
				ls_trk = vv.trk();
				//fpln(id+" REVISE: at tm = "+Fm1(tm)+" ls_t = "+Fm1(ls_t)+" ls_trk = "+Units::str("deg",ls_trk) );
			}
			//fpln(">>>>>>>>>>>  ls_t = "+Fm1(ls_t));
		}
	}
	

	StateVector AircraftState::get(int i) {
		if (i >= sz || i < 0) return StateVector(Vect3::ZERO(),Velocity::ZEROV(),0.0);
		updateProjection();
		return StateVector(Vect3(projS2[i], projH[i]), Velocity::mkVxyz(projV2[i].x, projV2[i].y, projVZ[i]), time(i));
	}

	StateVector AircraftState::getLast() {
	    return get(sz - 1);
	}


	// This inserts the given data at point i--no questions asked.  Everything
	// from index 0..i is shifted down one place. i is an external index
	//
	// Assumes:
	// 1. 0 <= i < size
	// 2. i is the correct place to insert the data; time is correctly ordered: t(i) < tm < t(i+1)
	// 3. the projection_done flag is set by somewhere else
	void AircraftState::insertAt(int i, const Position& ss, const Velocity& vv, double tm) {
		if (sz < bufferSize) {
			for(int j = 0; j <= i; j++) {
				int first = ext2int(j);
				int second = ext2int(j-1);
				s_list[second] = s_list[first];
				v_list[second] = v_list[first];
				t_list[second] = t_list[first];
			}
			s_list[ext2int(i)] = ss;
			v_list[ext2int(i)] = vv;
			t_list[ext2int(i)] = tm;
			oldest = ext2int(-1);
			sz++;
		} else {
			for(int j = 1; j <= i; j++) {
				int first = ext2int(j);
				int second = ext2int(j-1);
				s_list[second] = s_list[first];
				v_list[second] = v_list[first];
				t_list[second] = t_list[first];
			}
			s_list[ext2int(i)] = ss;
			v_list[ext2int(i)] = vv;
			t_list[ext2int(i)] = tm;			
		}
	}

	const Position& AircraftState::position(int i) const {
		if (i >= sz || i < 0) return checkLatLon(true) ? Position::ZERO_LL() : Position::ZERO_XYZ();
		return s_list[ext2int(i)];
	}
	
	const Velocity& AircraftState::velocity(int i) const {
		if (i >= sz || i < 0) return Velocity::ZEROV();
		return v_list[ext2int(i)];
	}

	double AircraftState::time(int i) const {
		if (i >= sz || i < 0) return -1.0;
		return t_list[ext2int(i)];
	}

	Position AircraftState::positionLast() const {
		return position(sz-1);
	}

	Velocity AircraftState::velocityLast() const {
		return velocity(sz-1);
	}

	double AircraftState::timeLast() const {
		return time(sz-1);
	}
	
    void AircraftState::remove(int n) {
    	if (n <= 0) return;
    	if (n > sz) n = sz;
    	oldest = ext2int(n);
    	sz = sz - n;
    }

    void AircraftState::removeUpToTime(double time) {
    	while (sz > 0) {
    		if (this->time(0) < time) {
    			remove(1);
    		} else {
    			break;
    		}
    	}
    }


   void AircraftState::removeLast() {
	  if (sz > 0) sz = sz - 1;
   }


    void AircraftState::setProjection(const EuclideanProjection& p) {
    	sp = p;
    	projection_initialized = true;
    	projection_done = false;
    }

//	void AircraftState::updateProjection(const EuclideanProjection sp) {
//		setProjection(sp);
//		updateProjection();
//	}

	void AircraftState::updateProjection() {
		if (projection_done || sz == 0) {
			return; // no need to do any work.
		}
		if (position(0).isLatLon()) {
			if ( ! projection_initialized) {
                error.addWarning("No projection defined in updateProjection()");
				sp = Projection::createProjection(LatLonAlt::ZERO());
			}
			for (int j = 0; j < sz; j++){                     // project all the points for aircraft id
				Position s = position(j);
				projS2[j] = sp.project2(s.lla());
				projH[j] = s.alt();
				if (AircraftState::projectVelocity) {
					Velocity v = sp.projectVelocity(s, velocity(j));
					projV2[j] = v.vect2();
					projVZ[j] = v.z;
				} else {
			  	   Velocity v = velocity(j); // sp.projectVelocity(s, velocity(j));
				   projV2[j] = v.vect2(); // sp.projectVelocity(s, velocity(j));
				   projVZ[j] = v.z;
				}
				projT[j] = time(j);
			}
		} else {
			for (int j = 0; j < sz; j++){                     // Euclidean coordinates, no projection done
				Position s = position(j);
				projS2[j] = s.vect2();
				projH[j] = s.alt();
				Velocity v = velocity(j);
				projV2[j] = v.vect2();
				projVZ[j] = v.z;
				projT[j] = time(j);
			}
		}
        projection_done = true;
        regression_done = false;
	}

	EuclideanProjection AircraftState::getProjection() const {
	  return sp;
	}


	// assumes that all arrays are the same length and have at least "length" elements
	// assumes the arrays are sorted in increasing time order.
	void AircraftState::calc(Vect2 pos2[], double posH[], Vect2 vel2[], double velZ[], double timevar[], int length) {
		if (regression_done) {
			return;
		}
		regression_done = true;
		if (length == 1) {
			horizvelintercept = vel2[0].norm();
			vertvelintercept  = velZ[0];
			horizvelslope     = 0;
			vertvelslope      = 0;
			recentInd		  = 0;
			return;
		}

		double sumv = 0;
		double sumt = 0;
		double sumtsq = 0;
		double sumvt = 0;
		double hsumv = 0;
		double hsumvt = 0;
		double timediff = 0;
		double vnorm = 0;
		double vertvelz = 0;
		double regdenom = 0;

		recentInd = length - 1;
// This search is not needed, currently, may be needed if data is out of order.
//		for (int point = 0; point < length; point++) {
//			if (timevar[point] > timevar[recentInd]) {
//				recentInd = point;
//			}
//		}

		for (int point = 0; point < length; point++) {
			timediff = timevar[point] - timevar[recentInd];
			vnorm = vel2[point].norm();
			vertvelz = velZ[point];

			sumv = sumv + vnorm;
			sumt = sumt + timediff;
			sumtsq = sumtsq + std::pow(timediff, 2.0);
			sumvt = sumvt + vnorm * timediff;
			hsumv = hsumv + vertvelz;
			hsumvt = hsumvt + vertvelz * timediff;
			//f.pln("Inside Regression Loop: "+point+": sumv is "+sumv+" and sumt is "+sumt+" and sumtsq is "
			// +sumtsq+" and sumvt is "+sumvt+" and timediff is "+timediff);
		}
		regdenom = length * sumtsq - sumt * sumt;
		if (regdenom != 0) {
			horizvelintercept = (sumv * sumtsq - sumt * sumvt) / regdenom;
			horizvelslope = (length * sumvt - sumt * sumv) / regdenom;
			vertvelintercept = (hsumv * sumtsq - sumt * hsumvt) / regdenom;
			vertvelslope = (length * hsumvt - sumt * hsumv) / regdenom;
		}
		//f.pln(":: horizvelintercept = "+horizvelintercept);
		//f.pln(":: horizvelslope = "+horizvelslope);
		//f.pln(":: vertvelintercept = "+vertvelintercept);
		//f.pln(":: vertvelslope  = "+vertvelslope);
		return;
	}

	Vect3 AircraftState::predS(double t) const {
		//f.pln("calling predS with time = "+t);
		double trel = t - projT[sz - 1];
		Vect2 predSxy = projS2[recentInd].AddScal(trel * (horizvelintercept + trel * horizvelslope / 2)
								* (1 / projV2[recentInd].norm()),projV2[recentInd]);
		double predAlt = projH[recentInd] + trel * (vertvelintercept + trel * vertvelslope / 2);
		return Vect3(predSxy, predAlt);
	}

	Velocity AircraftState::predV(double t) const {
		//f.pln("Calling PredV with horizvelintercept = "+horizvelintercept+" and horizvelslope = "+horizvelslope);
		double trel = t - projT[recentInd];
		Vect2 predVxy = projV2[recentInd].Scal((horizvelintercept + trel * horizvelslope)
				* (1 / projV2[recentInd].norm()));
		double predVz = vertvelintercept + trel * vertvelslope;
		return Velocity::mkVxyz(predVxy.x, predVxy.y, predVz);
	}


	StateVector AircraftState::pred(double t) {
		updateProjection();
		calc(projS2, projH, projV2, projVZ, projT, sz);
		return StateVector(predS(t), predV(t),t);
	}	

	StateVector AircraftState::predLinear(double t) {
		int i = sz-1;
		while (i > 0 && Util::almost_greater(time(i),t,PRECISION13)) {
			i--;
		}
		double dt = t - time(i);
		if (time(i) > t) {
			error.addWarning("predLinear: no data points before time");
		}
		pair<Vect3,Velocity> vv = get(i).pair();
		return StateVector(vv.first.AddScal(dt, vv.second), vv.second, t);
	}

	Position AircraftState::positionLinear(double t) const {
		int i = sz-1;
		while (i > 0 && Util::almost_greater(time(i),t,PRECISION13)) {
			i--;
		}
		double dt = t - time(i);
		if (time(i) > t) {
			error.addWarning("predLinear: no data points before time");
			return Position::INVALID();
		}
		return position(i).linear(velocity(i), dt);
	}

	Velocity AircraftState::velocityAt(double t) const {
		int i = sz-1;
		while (i > 0 && Util::almost_greater(time(i),t,PRECISION13)) {
			i--;
		}
		if (time(i) > t) {
			error.addWarning("velocity: no data points before time");
			return Velocity::INVALIDV();
		}
		return velocity(i);
	}

	//
	// Methods to check the "integrity" of the data
	//
	
	
	bool AircraftState::inLevelFlight() {
        int point = sz-1;
        Velocity vol,vonl;
        if (point <= 0) {
        	return false;
        } else {
	  	   vol = get(point).v();
        }
        
        if (point == 1) {
        	vonl = vol;
        } else {
		    vonl = get(point-1).v();
        }
		return std::abs(vol.z) < minClimbVelocity && std::abs(vonl.z) < minClimbVelocity;
	}
	
	bool AircraftState::closeEnough(Velocity v1, Velocity v2) {
  	     double delTrk = std::abs(v1.trk() - v2.trk());
		 double delGs = std::abs(v1.gs() - v2.gs()) ;
		 double delVs = std::abs(v1.vs() - v2.vs()) ;
		 bool trkOk = delTrk < Units::from("deg",10);
		 bool gsOk = delGs < Units::from("kn",10);
		 bool vsOk = delVs < Units::from("fpm",10);
         if (!trkOk)
        	 cout << "$$$$$$$$$ delTrk = " << delTrk << endl;
         if (!gsOk)
        	 cout << "$$$$$$$$$ delGs = " << delGs << endl;
         if (!trkOk)
        	 cout << "$$$$$$$$$ delVs = " << delVs << endl;
		 return trkOk && gsOk && vsOk;
	}

	double AircraftState::lastStraightTime() const {
		//fpln(id+" AircraftState::lastStraightTime ls_t = "+Fm1(ls_t)+" ls_trk = "+Units::str("deg",ls_trk));

		return ls_t;
	}

	/** Track Rate calculation
	 * @param i      index (0..size()-1)
	 * @return track rate at ith data location
	 */
	double AircraftState::trackRate(int i) {
		if (i >= sz || i < 0 || sz < 2) return 0.0;
		StateVector svt = get(i);
		StateVector svtm1 = get(i-1);
		//f.pln("$$$ svt = "+svt+" svtm1 = "+svtm1);
		return (svt.v().trk() - svtm1.v().trk())/(svt.t() - svtm1.t());
	}


	double AircraftState::timeLastZeroTrackRate() {
		//f.pln(" $$$ timeLastZeroTrackRate: "+id+" sz = "+sz);
		if (sz < 2) return -1;
        for (int j = sz-1; j > 0; j--) {
        	double trkRate = trackRate(j);
        	//fpln(Fm0(get(j).t())+" $$$ timeLastZeroTrackRate: trkRate = "+Units::str("deg/s",trkRate));
        	//fpln(" lastZeroTrackRateThreshold = "+Units::str("deg/s",lastZeroTrackRateThreshold));
        	if (std::abs(trkRate) < lastZeroTrackRateThreshold) {
        		StateVector svt = get(j);
        		//fpln(Fm0(get(j).t())+" $$$ timeLastZeroTrackRate: svt.t() = "+Units::str("s",svt.t()));
                return svt.t();
        	}
        }
   		StateVector svt = get(0);
        return svt.t();
	}


	double AircraftState::avgTrackRate(int numPtsTrkRateCalc) {
		int n = size();
		if (numPtsTrkRateCalc < 2) numPtsTrkRateCalc = 2;
		int numPts = Util::min(numPtsTrkRateCalc,n);
		double trkLast = 0;
		double tmLast = 0;
		//double maxTrackRate = 0.0;
		//bool turnRight = false;
		double trackRateSum = 0.0;
		//fpln(" >>>> avgTrackRate: numPts = "+Fm2(numPts)+" n= "+Fm0(n));
		for (int i = n - 1; i > n - numPts - 1 && i >= 0; i--){                      // i = 0 is oldest, i = size() -1 is newest
			StateVector svt = get(i);
			double track = svt.v().compassAngle();
			double tmTr = time(i);
			//fpln(" >>>> estimateOmega: i = "+i+" tR= "+Units::str("ft",tR));
			if (i < n-1) {
				double trackRate = Util::signedTurnDelta(trkLast,track)/(tmTr-tmLast);
				//fpln(">>>>  trackRate = "+Units::str("deg",trackRate));
				trackRateSum = trackRateSum + trackRate;
			}
			trkLast = track;
			tmLast = tmTr;
		}
		//return pair<double,bool>(trackRateSum/(numPts-1),turnRight);
		if (numPts < 2) return 0;
		else return trackRateSum/(numPts-1);
	}



	/** EXPERIMENTAL
	 * Estimate rate of change of vertical speed from sequence of velocity vectors stored in this object.
	 * The sign of the vertical speed rate indicates the direction of the acceleration
	 * @param numPtsVsRateCalc   number of data points used in the average, must be at least 2
	 * @return signed vertical speed rate (if insufficient number of points, return 0);
	 */
	double AircraftState::avgVsRate(int numPtsVsRateCalc) {
		int n = size();
		if (numPtsVsRateCalc < 2) numPtsVsRateCalc = 2;
		int numPts = Util::min(numPtsVsRateCalc,n);
		double vsLast = 0;
		double tmLast = 0;
		double vsRateSum = 0.0;
 		for (int i = n - 1; i > n - numPts - 1 && i >= 0; i--){                      // i = 0 is oldest, i = size() -1 is newest
			StateVector svt = get(i);
            //f.pln(" $$$ svt = "+svt);
			double vs = svt.v().vs();
			double tmTr = time(i);
			if (i < n-1) {  // make sure trackLast is defined
				double vsRate = (vs-vsLast)/(tmTr-tmLast);
				vsRateSum = vsRateSum + vsRate;
				//f.pln(" >>>> avgvsRate: i = "+i+" track = "+Units.str("deg",track)+" vsRate = "+Units.str("deg/s",vsRate));
				//		+" vsRateSum = "+Units.str("deg/s",vsRateSum));
			}
			vsLast = vs;
			tmLast = tmTr;
		}
		//f.pln(" >>>> avgvsRate: numPts = "+numPts+" vsRateSum = "+Units.str("deg/s",vsRateSum));
		//return new Pair<Double,Boolean>(vsRateSum/(numPts-1),turnRight);
		if (numPts < 2) return 0;
		else return vsRateSum/(numPts-1);
	}




	void AircraftState::prune() {
		//dump();
		Velocity lastV = Velocity();
		double lastT = 0.0;
		//f.pln("prune: initially, start = "+oldest+" size = " + sz);
		if (sz < 2)
			return;
		double baseAccel = (velocity(sz-1).z - velocity(sz - 2).z) / (time(sz-1) - time(sz - 2));
		//f.pln(" baseAccel = " + baseAccel);
		int largestPruned = -1;  // relative index
		lastV = velocity(0);
		lastT = time(0);
		for (int i = 1; i < sz; i++) {
			//f.pln("!! s = " + s(i) + "  v =" + v(i) + " t = " + t(i));
			double delT = time(i) - lastT;
			double accel = (velocity(i).z - lastV.z) / delT;
			lastV = velocity(i);
			lastT = time(i);
			//f.pln("!! for i = "+i+" accel = " + accel);
			if (std::abs((accel - baseAccel)/baseAccel) > MAX_RELATIVE_DIFF) {
				//f.pln(" prune "+i+" value "+accel);
				largestPruned = i;
			}
		}// for
		//f.pln(" largestPruned = "+largestPruned);
		if (largestPruned > 0) {
		   oldest = ext2int(largestPruned); 
		   sz = sz - largestPruned;
		   cout << "prune: Deleted " << largestPruned << " from aircraft " << id << "; start = " << oldest << " sz = " << sz << endl;
		}
        //dump();
	}

	//
	// Utility methods
	//
	
	int AircraftState::ext2int(int i) const {
		int j = (oldest+i) % bufferSize;
		if (j < 0) {
			return bufferSize + j;
		} else {
			return j;
		}
	}


	string AircraftState::dump() const {
		string rtn = "";
		for (int i = 0; i < sz; i++){
			rtn = rtn + id + "     " +position(i).toStringNP(4)+"    "+velocity(i).toStringNP(4)+"    "+Fm4(time(i))+"\n";
		}
		return rtn;
	}


	string AircraftState::toString() const {
        std::stringstream temp;
		temp << "AircraftState(" << id << "): sz = " << sz << " start = " << oldest << endl;
		for (int i = 0; i < sz; i++){
			temp << "  ---   s = " << position(i).toString() << "  v =" << velocity(i).toString() << " t = " << time(i) << endl;
		}
		return temp.str();
	}
    
    string AircraftState::toOutput() const {
      std::stringstream sb;
      for (int i = 0; i < size(); i++){
        sb << id << " " << position(i).toStringNP(8) << " " << velocity(i).toStringNP(6) << " " << time(i) << endl;
      }
      return sb.str();
    }
	
}
