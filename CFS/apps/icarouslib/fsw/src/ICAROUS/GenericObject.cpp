/*
 * GenericObject.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: research133
 */

#include "GenericObject.h"

GenericObject_t::GenericObject_t(int type_in,int id_in,float lat_in,float lon_in,float altmsl_in,
		                          float vx_in,float vy_in,float vz_in){

	id = id_in;
	type = type_in;
	pos = Position::makeLatLonAlt(lat_in,"degree",lon_in,"degree",altmsl_in,"m");
	vel = Velocity::makeVxyz(vy_in,vz_in,"m/s",vz_in,"m/s");
	orientation = vel.trk();
}

bool GenericObject_t::isEqual(GenericObject_t& obj, bool update){
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

void GenericObject_t::AddObject(list<GenericObject_t>& objectList,GenericObject_t& obj){

		bool inList = false;
		std::list<GenericObject_t>::iterator it;
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

void GenericObject_t::RemoveObject(list<GenericObject_t>& objectList, GenericObject_t& obj){
		objectList.remove(obj);
}

