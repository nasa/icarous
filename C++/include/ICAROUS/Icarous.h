/*
 * Icarous.h
 *
 *  Created on: Dec 15, 2016
 *      Author: research133
 */

#ifndef ICAROUS_H_
#define ICAROUS_H_

#include <stdio.h>
#include <thread>
#include <fstream>
#include <string.h>
#include <getopt.h>

#include "Communication.h"
#include "Interface.h"
#include "QuadFMS.h"
#include "ParameterData.h"
#include "SeparatedInput.h"

using namespace std;
using namespace larcfm;

class Icarous_t{

private:
	bool verbose = false;
	char px4port[100];
	char sitlhost[100];
	char gshost[100];
	char gsradio[100];
	char mode[100];
	int  px4baud = 0,radiobaud = 0;
	int sitlin =0,sitlout =0;
	int gsin =0,gsout = 0;
	ParameterData paramData;


public:
	Icarous_t(int argc,char* argv[]);
	void GetOptions(int argc,char* argv[]);
	void Run();

};



#endif /* ICAROUS_H_ */
