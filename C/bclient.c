/*
 * bclient.c -- joins a multicast group and receives a MAVLink GPS message
 *
 *
 * Swee Warman (07/20/16)
 * Multicast socket code adapted from Courtney and Bastien. The original 
 * socket code was obtained from: http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mavlink/icarous/mavlink.h"


#define HELLO_PORT 5555
#define HELLO_GROUP "230.1.1.1"
#define MSGBUFSIZE 6+255+2

int main(int argc, char *argv[])
{
     struct sockaddr_in addr;
     int fd, nbytes,addrlen;
     struct ip_mreq mreq;
     char msgbuf[MSGBUFSIZE];

     u_int yes=1;           

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }

    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }

     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family      = AF_INET;
     addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
     addr.sin_port        = htons(HELLO_PORT);
     
     /* bind to receive address */
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  perror("bind");
	  exit(1);
     }
     
     /* use setsockopt() to request that the kernel join a multicast group */
     mreq.imr_multiaddr.s_addr = inet_addr(HELLO_GROUP);
     mreq.imr_interface.s_addr = htonl(INADDR_ANY);
     if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
	  perror("setsockopt");
	  exit(1);
     }

     mavlink_message_t msg;
     mavlink_status_t status;

     mavlink_global_position_int_t global_position_int;
     mavlink_heartbeat_icarous_t heartbeat_icarous;
     
     /* Read mavlink messages */
     while (1) {

	  addrlen=sizeof(addr);
	  if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
			       (struct sockaddr *) &addr,&addrlen)) < 0) {
	       perror("recvfrom");
	       exit(1);
	  }
	  printf("Bytes received: %d\n",nbytes);
	  
	  for(int i=0;i<nbytes;i++){
	    if(mavlink_parse_char(MAVLINK_COMM_0, msgbuf[i], &msg, &status)) {
	      // Handle message
	      
	      switch(msg.msgid)
		{
		case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
		  {
		    
		    mavlink_msg_global_position_int_decode(&msg,&global_position_int);
		    printf("GPS lat=%f, lon=%f, alt=%f\n",global_position_int.lat/1E7,
			   global_position_int.lon/1E7,
			   global_position_int.alt/1E3);
		  }
		  break;

		case MAVLINK_MSG_ID_HEARTBEAT_ICAROUS:
		  {
		    mavlink_msg_heartbeat_icarous_decode(&msg,&heartbeat_icarous);
		    printf("Heartbeat obtained. Aircraft state = %d\n",heartbeat_icarous.status);

		  }
		  break;	        

		case MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE:
		  printf("received aircraft state\n");
		  
		default:
		  //Do nothing
		  break;
		}
	    }

	    //printf("msg_received   = %d\n",status.msg_received);
	    //printf("buffer overrun = %d\n",status.buffer_overrun);
	    //printf("parse error    = %d\n",status.parse_error);
	    //printf("parse state    = %d\n",status.parse_state);
	    //printf("packet_idx     = %d\n",status.packet_idx);
	    //printf("current_rx_seq = %d\n",status.current_rx_seq);
	  }

	  
	  
     }
}
