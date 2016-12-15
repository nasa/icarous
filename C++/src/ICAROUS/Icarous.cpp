/*
 * Icarous.cpp
 *
 *  Created on: Dec 15, 2016
 *      Author: research133
 */

#include "Icarous.h"

Icarous_t::Icarous_t(int argc,char* argv[],Mission_t* task){

	printf("Icarous version %s\n",version);
	GetOptions(argc,argv);

	// Read parameters from file and get the parameter data container
	ifstream ConfigFile;
	SeparatedInput sepInputReader(&ConfigFile);

	ConfigFile.open("params/icarous.txt");
	sepInputReader.readLine();
	paramData = sepInputReader.getParameters();
	mission = task;

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
		  {0,                          0, 0,   0}
	  };

	  int option_index = 0;

	  c = getopt_long (argc, argv, "ab:c:d:e:f:g:h:i:j:k:l:",
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
		  //printf("Connecting to pixhawk at %s\n",px4port);
		  break;

		case 'c':
		  px4baud = atoi(optarg);
		  //printf("pixhawk port baud rate %d\n",px4baud);
		  break;

		case 'd':
		  strcpy(sitlhost,optarg);
		  //printf("Connecting to SITL host: %s\n",sitlhost);
		  break;

		case 'e':
		  sitlin = atoi(optarg);
		  //printf("SITL host input at port: %d\n",sitlin);
		  break;

		case 'f':
		  sitlout = atoi(optarg);
		  //printf("SITL host output at port: %d\n",sitlout);
		  break;

		case 'g':
		  strcpy(gshost,optarg);
		  //printf("Ground station host: %s\n",gshost);
		  break;

		case 'h':
		  gsin = atoi(optarg);
		  //printf("Ground station host input at port:%d\n",gsin);
		  break;

		case 'i':
		  gsout = atoi(optarg);
		  //printf("Ground station host output at port:%d\n",gsout);
		  break;

		case 'j':
		  strcpy(gsradio,optarg);
		  //printf("Connecting to radio on port %s\n",gsradio);
		  break;

		case 'k':
		  radiobaud = atoi(optarg);
		  //printf("Radio baud rate %d\n",radiobaud);
		  break;

		case 'l':
		  strcpy(mode,optarg);
		  //printf("Launching ICAROUS in %s mode\n",mode);
		  break;

		case '?':

		  break;

		default:
		  abort ();
		}
	}
}

void Icarous_t::Run(){

	MAVLinkMessages_t RcvdMessages;
	AircraftData_t FlightData(&RcvdMessages,&paramData);

	Interface_t *AP;
	Interface_t *COM;

	SerialInterface_t apPort,gsPort;
	SocketInterface_t SITL,comSock;

	if(px4baud > 0){
		apPort = SerialInterface_t(px4port,px4baud,0,&RcvdMessages);
		AP     = &apPort;
	}
	else{
		SITL  = SocketInterface_t(sitlhost,sitlin,sitlout,&RcvdMessages);
		AP    = &SITL;
	}

	if(radiobaud > 0){
		gsPort = SerialInterface_t(gsradio,radiobaud,0,&RcvdMessages);
		COM    = &gsPort;
	}
	else{
		comSock = SocketInterface_t(gshost,gsin,gsout,&RcvdMessages);
		COM     = &comSock;
	}

	Communication_t DAQ(AP,COM,&FlightData);
	QuadFMS_t FMS(AP,COM,&FlightData,mission);

	FMS.SendStatusText("Starting ICAROUS");

	std::thread thread1(&Communication_t::GetPixhawkData,&DAQ);
	std::thread thread2(&Communication_t::GetGSData,&DAQ);
	std::thread thread3(&FlightManagementSystem_t::RunFMS,&FMS);

	thread1.join();
	thread2.join();
	thread3.join();

}

