/*
 * GenericObject.h
 *
 *  Created on: Mar 20, 2017
 *      Author: Swee Balachandran
 */

#ifndef GENERICOBJECT_H_
#define GENERICOBJECT_H_

#include "Position.h"
#include "Velocity.h"
#include "string.h"
#include <list>

using namespace larcfm;
using namespace std;


typedef enum{
	_TRAFFIC_,
	_OBSTACLE_,
}genericObjectType_e;

class GenericObject{

public:
	int time;
	int id;
	int type;
	char callsign[25];
	bool callsign_avail;
	Position pos;
	Velocity vel;
	double orientation;
	GenericObject(){};
	GenericObject(double time,genericObjectType_e type_in,int id_in,char *callsign,float lat_in,float lon_in,float altmsl_in,
			        float vx_in, float vy_in, float vz_in);

	bool operator==( const GenericObject& other ) const  {
	    if (id == other.id){
	    	return true;
	    }else{
	    	return false;
	    }
	}

	bool isEqual(GenericObject& obj, bool update);
	static int AddObject(list<GenericObject>& objectList, GenericObject& obj);
	static void RemoveObject(list<GenericObject>& objectList, GenericObject& obj);
};

#endif /* GENERICOBJECT_H_ */
