#ifndef INC_NMEA_GPRMC_H
#define INC_NMEA_GPRMC_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
Example:

$PFLAA,0,-1234,1234,220,2,DD8F12,180,,30,-1.4,1*

There is a glider in the south-east direction, 1.7km away (1.2km south, 1.2km
east), 220m higher flying on south track with a ground speed of 30m/s in a slight
left turn with 4.5°/s turning rate, sinking with 1.4m/s. Its ID is a static FLARM-ID
“DD8F12”. There is no danger.

*/
typedef struct {
	int alarmLevel;
	int relNorth;
	int relEast;
	int relVertical;
	int idType;
	char ID[6];
	int track;
	int groundSpeed;
	float climbRate;
	char acftType[1];
	int noTrack;
} nmea_pflaa_s;

/* Value indexes */
#define NMEA_PFLAA_ALARMLEVEL		0
#define NMEA_PFLAA_RELATIVE_NORTH		1
#define NMEA_PFLAA_RELATIVE_EAST		2
#define NMEA_PFLAA_RELATIVE_VERTICAL		3
#define NMEA_PFLAA_IDTYPE		4
#define NMEA_PFLAA_ID		5
#define NMEA_PFLAA_TRACK		6
//#define NMEA_PFLAA_TURN_RATE		7
#define NMEA_PFLAA_GROUND_SPEED		7
#define NMEA_PFLAA_CLIMB_RATE		8
#define NMEA_PFLAA_ACFTTYPE		9
#define NMEA_PFLAA_NO_TRACK		10

#endif  /* INC_NMEA_PFLAA_H */
