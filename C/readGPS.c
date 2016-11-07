#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#define GROUP1_LEN 198
#define GROUP2_LEN 144

#ifdef GROUP2_LEN
  #define HEADER 5 // GROUP (1), GROUP FIELD(s) (2 for each field) 
#else
  #define HEADER 3 // GROUP (1), GROUP FIELD(s) (2 for each field) 
#endif

#define CRC_LEN 2

#define PAYLOAD_LEN GROUP1_LEN + GROUP2_LEN
#define DATA_LEN HEADER + PAYLOAD_LEN + CRC_LEN

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
	  printf("error in tcgetattr 1");
	  return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
		printf("error from tcsetattr 2");
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
		printf("error from tcsetattr 3");
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
	  printf("error from tcsetattr 4");
	
}

struct BinGroup1{
  uint64_t TimeStartup; //8
  uint64_t TimeGps;     //8
  uint64_t TimeSyncIn;  //8
  float YawPitchRoll[3];//12
  float Quaternion[4];  //16
  float AngularRate[3]; //12
  double LLA[3];        //24
  float Velocity[3];    //12
  float Accel[3];       //12
  float Imu[6];         //24
  float MagPres[5];     //20
  float DeltaThetaVel[7]; //28
  uint16_t InsStatus;  //2
  uint32_t SyncInCnt;  //4
  uint64_t TimeGpsPps; //8
};

struct BinGroup2{
  //UTC
  int8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t ms[2];
  
  uint64_t Tow;
  uint16_t Week;
  uint8_t NumSats;
  uint8_t Fix;
  double PosLla[3];
  double PosEcef[3];
  float VelNed[3];
  float VelEcef[3];
  float PosU[3];
  float VelU;
  uint32_t TimeU;
};

uint16_t calculateCRC(unsigned char data[], unsigned int length)
{
 unsigned int i;
 uint16_t crc = 0;
 for(i=0; i<length; i++){
 crc = (unsigned char)(crc >> 8) | (crc << 8);
 crc ^= data[i];
 crc ^= (unsigned char)(crc & 0xff) >> 4;
 crc ^= crc << 12;
 crc ^= (crc & 0x00ff) << 5;
 }

 return crc;
}

void ExtractDataGroup1(uint8_t *payload, struct BinGroup1 *msg){

  uint8_t *p = payload+HEADER;

  // extract group 1
  memcpy(&(msg->TimeStartup),p,8);p = p+8;
  memcpy(&(msg->TimeGps),p,8);p = p+8;
  memcpy(&(msg->TimeSyncIn),p,8);p = p+8;
  memcpy(&(msg->YawPitchRoll),p,12);p = p+12;
  memcpy(&(msg->Quaternion),p,16);p = p+16;
  memcpy(&(msg->AngularRate),p,12);p = p+12;
  memcpy(&(msg->LLA),p,24);p = p+24;
  memcpy(&(msg->Velocity),p,12);p = p+12;
  memcpy(&(msg->Accel),p,12);p = p+12;
  memcpy(&(msg->Imu),p,24);p = p+24;
  memcpy(&(msg->MagPres),p,20);p = p+20;
  memcpy(&(msg->DeltaThetaVel),p,28);p = p+28;
  memcpy(&(msg->InsStatus),p,2);p = p+2;
  memcpy(&(msg->SyncInCnt),p,4);p = p+4;
  memcpy(&(msg->TimeGpsPps),p,8);
}

void ExtractDataGroup2(uint8_t *payload, Struct BinGroup2 *msg){
  uint8_t *p = payload + HEADER+GROUP1_LEN;
  memcpy(&(msg->year),p,1); p = p+1;
  memcpy(&(msg->month),p,1); p = p+1;
  memcpy(&(msg->day),p,1); p = p+1;
  memcpy(&(msg->hour),p,1); p = p+1;
  memcpy(&(msg->min),p,1); p = p+1;
  memcpy(&(msg->sec),p,1); p = p+1;
  memcpy(&(msg->ms),p,2); p = p+2;
  memcpy(&(msg->Tow),p,8); p = p+8;
  memcpy(&(msg->Week),p,2); p = p+2;
  memcpy(&(msg->NumSats),p,1); p = p+1;
  memcpy(&(msg->Fix),p,1); p = p+1;
  memcpy(&(msg->PosLla),p,24); p = p+24;
  memcpy(&(msg->PosEcef),p,24); p = p+24;
  memcpy(&(msg->VelNed),p,12); p = p+12;
  memcpy(&(msg->VelEcef),p,12); p = p+12;
  memcpy(&(msg->PosU),p,12); p = p+12;
  memcpy(&(msg->VelU),p,4); p = p+4;
  memcpy(&(msg->TimeU),p,4); p = p+4;
}

int ProcessGPSMessage(uint8_t c, struct BinGroup1* msg1, struct BinGroup2* msg2){

  static int state = 0;
  static int count = 0;
  static uint16_t fieldSize1 = 0;
  static uint16_t fieldSize2 = 0;
  static uint16_t CRC = 0;  
  static uint8_t payload[PAYLOAD_LEN];
  static int packetsize = 0;
  
  uint8_t data[DATA_LEN];
  uint16_t CRCV;
  int8_t *p;
  
  switch(state){
  case 0:       
    // Checking for header
    count     = 0;
    fieldSize = 0;
    p         = payload;
    memset(payload,0,PAYLOAD_LEN);
    memset(data,0,DATA_LEN);
    if(c == 0xFA){
      packetsize = 0;
      packetsize++;
      state = 1;
      //printf("Received header\n");
    }
    break;

  case 1:
    // Groups active    
    data[packetsize-1] = c;
    packetsize++;
    state = 2;
    count = 0;
    //printf("Group %02x is active\n",c);
    
    break;
    
  case 2:
    // Active fields in each group
    data[packetsize-1] = c;
    packetsize++;
    if(count == 0){
      fieldSize1 = 0;
      fieldSize1 = fieldSize1 | c;
      count++;
    }
    else if(count == 1){
      fieldSize1 = (fieldSize1 << 8) | c;
      count++;
    }
    else if(count == 2){
      fieldSize2 = 0;
      fieldSize2 = fieldSize2 | c;
      count++;
    }
    else if(count == 3){
      fieldSize2 = (fieldSize2 << 8) | c;
      state = 3;
      count = 0;
    }

    break;

  case 3:
    //Payload
    data[packetsize-1] = c;
    packetsize++;
    p = p + 1;
    memcpy(p,&c,1);
    count++;
    if(count == PAYLOAD_LEN){
      //printf("Receive payload, count=%d\n",count);
      state = 4;
      count = 0;
      
    }

    break;
    
  case 4:
    //CRC
    //printf("datasize = %d\n",packetsize);
    data[packetsize-1] = c;
    packetsize++;
    if(count == 0){
      CRC = 0;
      CRC = CRC | c;
      count++;
    }
    else if(count == 1){
      CRC = (CRC << 8) | c;
      state = 5;
      count = 0;
      //printf("Received checksum\n");
    }

    break;

  case 5:
    // Validate data    
    CRCV = calculateCRC(data,DATA_LEN);
    state = 0;
    count = 0;
    if(CRCV == 0x0000){
      ExtractDataGroup1(data,msg1);
      ExtractDataGroup2(data,msg2);
      //printf("Valid data obtained\n");
      return 1;
    }
    else{      
      return 0;
    }
    
    break;
  }//end of switch

  return 0;
}

void InitVecNav(int fd){

  // Pause ASCII output
  char input1[] = "$VNWRG,06,0*XX\r\n";
  write(fd,input1,sizeof(input1));

  // Enable Binary group 1 output on serial port 1

  char input2[] = "$VNWRG,75,1,16,09,FFFF,FFFF*XX\r\n";
  write(fd,input2,sizeof(input2));
  
}

void main(){
  char *portname = "/dev/ttyUSB0";
  
  int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0)
  {
    printf("Error operning port");
    return;
  }
  
  set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
  set_blocking (fd, 1);                    // set no blocking
  
  char buf [1];

  tcflush(fd,TCIOFLUSH);

  InitVecNav(fd);

  struct BinGroup1 msg1;
  struct BinGroup2 msg2;
  memset(&msg1,0,sizeof(msg1));
  memset(&msg2,0,sizeof(msg2));
  
  while(1){
    uint8_t buf = 0;        
    int n = read (fd, &buf, 1);    // read up to 100 characters if ready to read
    //printf("%c",buf);
    int status = ProcessGPSMessage(buf,&msg1,&msg2);

    if(status == 1){            
      printf("Yaw = %f, Pitch = %f, Roll = %f\n",msg1.YawPitchRoll[0],msg1.YawPitchRoll[1],msg1.YawPitchRoll[2]);
    }
  }
  return;
}

