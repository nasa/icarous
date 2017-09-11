/**
 * Interface
 * 
 * This is a class that will enable establishing a socket or
 * a serial interface between ICAROUS and any component that can
 * stream MAVLink messages
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

#include "Port.h"


Port_t::Port_t(){
    pthread_mutex_init(&locktx, NULL);
    pthread_mutex_init(&lockrx, NULL);
}


void Port_t::PipeThrough(Port_t* intf,int32_t len){
  intf->WriteData(recvbuffer,len);
}

SerialPort_t::SerialPort_t(char name[],int brate,int pbit)
{

  portname = name;
  baudrate = brate;
  parity   = pbit;
  
  fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0)
  {
    printf("Error operning port");
    return;
  }
  
  set_interface_attribs ();            // set baudrate, 8n1 (no parity)
  set_blocking (0);                    // set no blocking

}

int SerialPort_t::set_interface_attribs()
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        printf("error in tcgetattr 1");
        return -1;
    }

    cfsetospeed (&tty, baudrate);
    cfsetispeed (&tty, baudrate);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag |= CRTSCTS;
    //tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        printf("error from tcsetattr 2");
        return -1;
    }
    return 0;
}

void SerialPort_t::set_blocking (int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        printf("error from tcsetattr 3");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 1;                      // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
        printf("error from tcsetattr 4");
	
}

int SerialPort_t::ReadData(){
    
    char buf;
    int n = 0;
    pthread_mutex_lock(&lockrx);
    n = read (fd, &buf, 1);
    recvbuffer[0] = buf;
    //n = read (fd, recvbuffer, 256);
    pthread_mutex_unlock(&lockrx);

    return n;
}

void SerialPort_t::WriteData(uint8_t buffer[],uint16_t len){

  pthread_mutex_lock(&locktx);
  for(int i=0;i<len;i++){
    char c = buffer[i];
    write(fd,&c,1);
  }
  pthread_mutex_unlock(&locktx);  
}


// Socet interface class definition
SocketPort_t::SocketPort_t(char targetip[], int inportno, int outportno)
{

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    recvlen = 0;
    bzero((char *) &locAddr, sizeof(locAddr));
    locAddr.sin_family      = AF_INET;
    locAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    locAddr.sin_port        = htons((unsigned short)inportno);
    
    /* Bind the socket to input */
    if (bind(sock,(struct sockaddr *)&locAddr, sizeof(locAddr)) == -1){
      printf("error: bind failed");
      close(sock);
      exit(EXIT_FAILURE);
    } 
    

    if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0){
      printf("error setting nonblocking\n");
      close(sock);
      exit(EXIT_FAILURE);
    }
    
    bzero((char *) &targetAddr, sizeof(targetAddr));
    targetAddr.sin_family      = AF_INET;
    targetAddr.sin_addr.s_addr = inet_addr(targetip);
    targetAddr.sin_port        = htons((unsigned short)outportno);
    
}

int SocketPort_t::ReadData(){

    int n = 0;
    pthread_mutex_lock(&lockrx);
    memset(recvbuffer, 0, BUFFER_LENGTH);
    n = recvfrom(sock, (void *)recvbuffer, BUFFER_LENGTH, 0, (struct sockaddr *)&targetAddr, &recvlen);
    pthread_mutex_unlock(&lockrx);
    return n;
}

void SocketPort_t::WriteData(uint8_t buffer[],uint16_t len){
    pthread_mutex_lock(&locktx);
    sendto(sock, buffer, len, 0, (struct sockaddr*)&targetAddr, sizeof (struct sockaddr_in));
    pthread_mutex_unlock(&locktx);
}
