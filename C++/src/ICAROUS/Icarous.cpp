/**
 * ICAROUS
 *
 * Icarous definitions
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 *
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.
 *  All rights reserved.
 *
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED,
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT,
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

#include <Icarous_msg.h>
#include "Icarous.h"
#include "Constants.h"
#include "QuadFMS.h"
#include "Interface.h"

std::string Icarous_t::VERSION = "1.2.2";

std::string Icarous_t::release() {
	return "ICAROUS++ V-"+VERSION+
			"-FormalATM-"+Constants::version+" (July-28-2017)";
}

Icarous_t::Icarous_t():FlightData(&paramData){
    usePlexil = false;
    FMS = new QuadFMS_t(&FlightData);
}

Icarous_t::Icarous_t(int argc,char* argv[]):FlightData(&paramData){

	cout << "ICAROUS Release: " << release() << std::endl;
    usePlexil = false;
	GetOptions(argc, argv);
    FMS = new QuadFMS_t(&FlightData);
}

void Icarous_t::InputParamTable(ParameterData* pData){
	paramData = *pData;
}

void Icarous_t::GetOptions(int argc,char* argv[]){

	while (1)
	{
	  int c;
	  static struct option long_options[] =
	  {
		  {"verbose",      no_argument,   0, 'a'},
		  {"px4",      required_argument, 0, 'b'},
		  {"px4baud",  required_argument, 0, 'c'},
		  {"sitlhost", required_argument, 0, 'd'},
		  {"sitlin",   required_argument, 0, 'e'},
		  {"sitlout",  required_argument, 0, 'f'},
		  {"gshost",   required_argument, 0, 'g'},
		  {"gsin",     required_argument, 0, 'h'},
		  {"gsout",    required_argument, 0, 'i'},
		  {"radio",    required_argument, 0, 'j'},
		  {"radiobaud",required_argument, 0, 'k'},
		  {"mode",     required_argument, 0, 'l'},
		  {"config",   required_argument, 0, 'm'},
		  {"debug",          no_argument, 0, 'n'},
          {"plexil",          no_argument, 0, 'o'},
		  {0,                          0, 0,   0}
	  };

	  int option_index = 0;

	  c = getopt_long (argc, argv, "ab:c:d:e:f:g:h:i:j:k:l:m:n:o",
					   long_options, &option_index);
	  
	  if (c == -1)
		break;

	  switch (c)
		{
		case 'a':
		  verbose = true;
		  break;

		case 'b':
		  //printf ("option -c with value `%s'\n", optarg);
		  strcpy(px4port,optarg);
		  printf("Connecting to pixhawk at %s\n",px4port);
		  break;

		case 'c':
		  px4baud = atoi(optarg);
		  printf("pixhawk port baud rate %d\n",px4baud);
		  break;

		case 'd':
		  strcpy(sitlhost,optarg);
		  printf("Connecting to SITL host: %s\n",sitlhost);
		  break;

		case 'e':
		  sitlin = atoi(optarg);
		  printf("SITL host input at port: %d\n",sitlin);
		  break;

		case 'f':
		  sitlout = atoi(optarg);
		  printf("SITL host output at port: %d\n",sitlout);
		  break;

		case 'g':
		  strcpy(gshost,optarg);
		  printf("Ground station host: %s\n",gshost);
		  break;

		case 'h':
		  gsin = atoi(optarg);
		  printf("Ground station host input at port:%d\n",gsin);
		  break;

		case 'i':
		  gsout = atoi(optarg);
		  printf("Ground station host output at port:%d\n",gsout);
		  break;

		case 'j':
		  strcpy(gsradio,optarg);
		  printf("Connecting to radio on port %s\n",gsradio);
		  break;

		case 'k':
		  radiobaud = atoi(optarg);
		  printf("Radio baud rate %d\n",radiobaud);
		  break;

		case 'l':
		  strcpy(mode,optarg);
		  printf("Launching ICAROUS in %s mode\n",mode);
		  break;

		case 'n':
		  printf("debug mode enabled\n");
		  debug = true;
		  break;

		case 'm': {
			strcpy(config, optarg);
			printf("config file %s\n", config);

			ifstream ConfigFile;
			SeparatedInput sepInputReader(&ConfigFile);

			ConfigFile.open(config);
			sepInputReader.readLine();
			paramData = sepInputReader.getParameters();

			break;
		}

        case 'o':{
            usePlexil = true;
        }

		case '?':
		  break;

		default:
		  abort ();
		}
	}
}

void Icarous_t::Run(Interface_t* AP,Interface_t* GS){
    while(true) {
        if (!usePlexil) {
            FMS->RunFMS();
        }
        OutputToAP(AP);
        OutputToGS(GS);
    }
}

void Icarous_t::OutputToAP(Interface_t *iface) {
    int n=1;
    while(n>=0){
        ArgsCmd_t cmd;
        n = OutputCommand(&cmd);
        if(n >= 0){
            iface->SendData(MSG_ID_CMD,(void*)&cmd);
        }
    }
}

void Icarous_t::OutputToGS(Interface_t *iface) {
    int n=0;
    visbands_t visband;
    n = OutputKinematicBands(&visband);
    if (n>0){
        iface->SendData(MSG_ID_BANDS,(void*)&visband);
    }
}


void Icarous_t::Initialize() {
    FMS->Initialize();
}

void Icarous_t::InputFlightPlanData(waypoint_t* wp){
	FlightData.AddMissionItem(wp);
}

void Icarous_t::InputClearFlightPlan() {
	FlightData.listMissionItem.clear();
}

void Icarous_t::InputResetIcarous() {
	FlightData.Reset();
}

void Icarous_t::InputGeofenceData(geofence_t* gf){

	if(gf->vertexIndex == 0){
		tempVertices.clear();
	}
	tempVertices.push_back(*gf);
    std::cout<<"received fence: "<<gf->vertexIndex<<"/"<<gf->totalvertices<<std::endl;
	if(gf->vertexIndex+1 == gf->totalvertices){
		Geofence_t fence((int)gf->index,(FENCE_TYPE)gf->type,(int)gf->totalvertices,gf->floor,gf->ceiling,FlightData.paramData);
		for(geofence_t sgf: tempVertices){
			fence.AddVertex(sgf.vertexIndex,sgf.latitude,sgf.longitude);
		}

		if(FlightData.fenceList.size() <= gf->index){
			FlightData.fenceList.push_back(fence);
			std::cout << "Received fence: "<<gf->index <<std::endl;
		}
		else{
			std::list<Geofence_t>::iterator it;
			for(it = FlightData.fenceList.begin(); it != FlightData.fenceList.end(); ++it){
				if(it->GetID() == fence.GetID()){
					it = FlightData.fenceList.erase(it);
					FlightData.fenceList.insert(it,fence);
					break;
				}
			}
		}
	}
}

void Icarous_t::InputStartMission(int param1){
	FlightData.SetStartMissionFlag(param1);
}

int Icarous_t::OutputCommand(ArgsCmd_t* cmd){
	if(FlightData.outputList.size() > 0){
		ArgsCmd_t icCmd;
		icCmd = (ArgsCmd_t)FlightData.outputList.front();
		cmd->name = icCmd.name;
		cmd->param1 = icCmd.param1;
		cmd->param2 = icCmd.param2;
		cmd->param3 = icCmd.param3;
		cmd->param4 = icCmd.param4;
		cmd->param5 = icCmd.param5;
		cmd->param6 = icCmd.param6;
		cmd->param7 = icCmd.param7;
		cmd->param8 = icCmd.param8;
		FlightData.outputList.pop_front();
		return FlightData.outputList.size();
	}else{
		return -1;
	}
}

void Icarous_t::InputAck(CmdAck_t* ack){
    FlightData.InputAck(ack);
}

void Icarous_t::InputPosition(position_t* pos){
	larcfm::Position currentPos = Position::makeLatLonAlt(pos->latitude,"degree",pos->longitude,"degree",pos->altitude_rel,"m");
	larcfm::Velocity currentVel = Velocity::makeVxyz(pos->vy,pos->vx,"m/s",pos->vz,"m/s");

	FlightData.acState.add(currentPos,currentVel,pos->time_gps);
	FlightData.acTime = pos->time_gps;
}

void Icarous_t::InputAttitude(attitude_t* att){
	double roll, pitch, yaw, heading;

	heading = FlightData.acState.velocityLast().track("degree");
	if(heading < 0){
		heading = 360 + heading;
	}

	FlightData.roll = att->roll;
	FlightData.pitch = att->pitch;
	FlightData.yaw = att->yaw;
	FlightData.heading = heading;
}

void Icarous_t::InputMissionItemReached(missionItemReached_t* msnItem){
	FlightData.nextMissionWP++;
}

void Icarous_t::InputTraffic(object_t* traffic){
	FlightData.AddTraffic(traffic->index,traffic->latitude,traffic->longitude,traffic->altiude,
						  traffic->vx,traffic->vy,traffic->vz);
}

int Icarous_t::OutputKinematicBands(visbands_t *bands) {
    if(FlightData.visBands.numBands > 0){
        bands->numBands = FlightData.visBands.numBands;
        bands->type1 = FlightData.visBands.type1;
        bands->type2 = FlightData.visBands.type2;
        bands->type3 = FlightData.visBands.type3;
        bands->type4 = FlightData.visBands.type4;
        bands->type5 = FlightData.visBands.type5;
        bands->min1  = FlightData.visBands.min1;
        bands->min2  = FlightData.visBands.min2;
        bands->min3  = FlightData.visBands.min3;
        bands->min4  = FlightData.visBands.min4;
        bands->min5  = FlightData.visBands.min5;
        bands->max1  = FlightData.visBands.max1;
        bands->max2  = FlightData.visBands.max2;
        bands->max3  = FlightData.visBands.max3;
        bands->max4  = FlightData.visBands.max4;
        bands->max5  = FlightData.visBands.max5;
        return FlightData.visBands.numBands;
    }
    return 0;
}