/*
 * Mission.h
 *
 *  Created on: Dec 15, 2016
 *      Author: research133
 */

#ifndef MISSION_H_
#define MISSION_H_

class FlightManagementSystem_t;

class Mission_t{

public:
	virtual void Execute(FlightManagementSystem_t* fms){};
	virtual ~Mission_t(){};
};


#endif /* MISSION_H_ */
