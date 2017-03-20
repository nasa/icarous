/*
 * DAQ.h
 *
 *  Created on: Mar 20, 2017
 *      Author: research133
 */

#ifndef DAQ_H_
#define DAQ_H_

#include <string>
#include "Interface.h"
#include "AircraftData.h"
#include "ErrorLog.h"

#include "icarous/mavlink.h"

class DAQ_t{

private:
	Interface_t* px4Intf;
	Interface_t* gsIntf;

public:
	ErrorLog log;
	DAQ_t():log("COM"){};
	DAQ_t(Interface_t* px4int, Interface_t* gsint);
	void GetPixhawkData();
};

#endif /* DAQ_H_ */
