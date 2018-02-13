/*
 * GenericObject.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: Swee Balachandran
 */

#include "GenericObject.h"

GenericObject::GenericObject(int type_in,int id_in,float lat_in,float lon_in,float altmsl_in,
		                          float vx_in,float vy_in,float vz_in){

	id = id_in;
	type = type_in;
	pos = Position::makeLatLonAlt(lat_in,"degree",lon_in,"degree",altmsl_in,"m");
	vel = Velocity::makeVxyz(vy_in,vx_in,"m/s",vz_in,"m/s");
	orientation = vel.trk();
}

bool GenericObject::isEqual(GenericObject& obj, bool update){
	if(id == obj.id){
		if(update){
			pos = obj.pos;
			vel = obj.vel;
			orientation = obj.orientation;
		}
		return true;
	}else{
		return false;
	}
}

void GenericObject::AddObject(list<GenericObject>& objectList,GenericObject& obj){

		bool inList = false;
		std::list<GenericObject>::iterator it;
		for(it = objectList.begin();it != objectList.end();it++){
			if( it->isEqual(obj,true)){
				inList = true;
				break;
			}
		}

		if(!inList){
			objectList.push_back(obj);
			printf("Added object\n");
		}
}

void GenericObject::RemoveObject(list<GenericObject>& objectList, GenericObject& obj){
		objectList.remove(obj);
}

