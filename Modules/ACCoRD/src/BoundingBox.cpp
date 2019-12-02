/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BoundingBox.h"
#include "Vect2.h"
#include "BoundingRectangle.h"
#include "GreatCircle.h"
#include "format.h"


namespace larcfm {
  	
  BoundingBox::BoundingBox() {
    //br = new BoundingRectangle();
		br.clear();
		zMin = MAXDOUBLE;
		zMax = -MAXDOUBLE;
	}	

	BoundingBox::BoundingBox(double zMin, double zMax) {
	  //br = new BoundingRectangle();
		this->zMin = zMin;
		this->zMax = zMax;
	}
	
	BoundingBox::BoundingBox(const BoundingBox& bb) {  //copy
	  //if (bb != NULL) {
			br = BoundingRectangle(bb.br);
			zMin = bb.zMin;
			zMax = bb.zMax;
			//	} else {
			//clear();
			//}
	}

	BoundingBox::BoundingBox(const BoundingRectangle& br) {
		this->br = br;
		zMin = MAXDOUBLE;
		zMax = -MAXDOUBLE;
	}

	BoundingBox::BoundingBox(const BoundingRectangle& br, double zMin, double zMax) {
		this->br = br;
		this->zMin = zMin;
		this->zMax = zMax;
	}


	void BoundingBox::clear() {
		br.clear();
		zMin = MAXDOUBLE;
		zMax = -MAXDOUBLE;
	}
	
	void BoundingBox::add(double x, double y, double z) {
		br.add(x,y);
		zMin = Util::min(z,zMin);
		zMax = Util::max(z,zMax);
	}

	void BoundingBox::add(const Vect3& v) {
	  //if (v == NULL) return;
		add(v.x,v.y,v.z);
		zMin = Util::min(v.z,zMin);
		zMax = Util::max(v.z,zMax);

	}
	
	void BoundingBox::add(const Position& p) {
	  //if (p == NULL) return;
		if (p.isLatLon()) {
			br.add(p.lla()); //p.lon(),Util::to_pi2_cont(p.lat()),p.alt());			
		} else {
			add(p.x(),p.y(),p.z());
		}
		zMin = Util::min(p.z(),zMin);
		zMax = Util::max(p.z(),zMax);

	}
	
	double BoundingBox::getMinX() const {
		return br.getMinX();
	}

	double BoundingBox::getMaxX() const {
		return br.getMaxX();
	}

	double BoundingBox::getMinY() const {
		return br.getMinY();
	}

	double BoundingBox::getMaxY() const {
		return br.getMaxY();
	}

	double BoundingBox::getMinZ() const {
		return zMin;
	}

	double BoundingBox::getMaxZ() const {
		return zMax;
	}

	double BoundingBox::getMinLon() const { return br.getMinLon(); }
	double BoundingBox::getMaxLon() const { return br.getMaxLon(); }
	double BoundingBox::getMinLat() const { return br.getMinLat(); }
	double BoundingBox::getMaxLat() const { return br.getMaxLat(); }

	
	Position BoundingBox::upperRightTop() {
		if (br.isLatLon()) {
			return Position(LatLonAlt::mk(getMaxY(), getMaxX(), getMaxZ()).normalize());
		} else {
		  return Position(Vect3::mkXYZ(getMaxX(), getMaxY(), getMaxZ()));
		}
	}
	
	Position BoundingBox::lowerLeftBottom() {
		if (br.isLatLon()) {
			return Position(LatLonAlt::mk(getMinY(), getMinX(), getMinZ()).normalize());
		} else {
		  return Position(Vect3::mkXYZ(getMinX(), getMinY(), getMinZ()));
		}
	}
	
	double BoundingBox::getMinAlt() { return zMin; }
	double BoundingBox::getMaxAlt() { return zMax; }
	

	BoundingRectangle BoundingBox::getBoundRect() const {	
		return br;
	}

	bool BoundingBox::intersects(const BoundingBox& bb) {
		return br.intersects(bb.getBoundRect()) && bb.getMinZ() <= zMax && bb.getMaxZ() >= zMin;
	}
	
	BoundingBox BoundingBox::linear(const Velocity& v, double dt) {
	  BoundingBox br; // = new BoundingBox();
		Position ur = upperRightTop().linear(v, dt);
		Position ll = lowerLeftBottom().linear(v, dt);
		br.add(ur);
		br.add(ll);
		return br;
	}

	std::string BoundingBox::toString() {
	  return br.toString()+"<z:"+Fm6(zMin)+" to "+Fm6(zMax)+">";
	}
	
}
