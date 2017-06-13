/*
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

import java.io.*;
import java.nio.file.*;
import java.net.*;
import com.MAVLink.*;
import com.MAVLink.common.msg_command_int;
import com.MAVLink.common.msg_command_long;
import com.MAVLink.enums.MAV_CMD;
import com.MAVLink.icarous.*;

public class Jetson_SafeguardReader{

   public static void main(String args[]){

	Path GPIO_export        = Paths.get("/sys/class/gpio/export");
	Path GPIO_unexport      = Paths.get("sys/class/gpio/unexport");

	String port1            = args[0];
	String port2            = args[1];
	
	Path GPIO_direction1    = Paths.get("/sys/class/gpio/gpio"+port1+"/direction");
	Path GPIO_value1        = Paths.get("/sys/class/gpio/gpio"+port1+"/value");

	Path GPIO_direction2    = Paths.get("/sys/class/gpio/gpio"+port2+"/direction");
	Path GPIO_value2        = Paths.get("/sys/class/gpio/gpio"+port2+"/value");
	
	InetAddress host        = null;

	try{
	    host           = InetAddress.getByName("localhost");
	}catch(UnknownHostException e){
	    System.out.println(e);
	}

	DatagramSocket sock = null;
	try{
	    sock     = new DatagramSocket();
	}catch(SocketException e){
	    System.out.println(e);
	}
	
	int udpSendPort         = Integer.parseInt(args[2]);
	    
	try{
	    // Activate port
	    Files.write(GPIO_export,port1.getBytes(),StandardOpenOption.WRITE);
	    Files.write(GPIO_export,port2.getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){	    
	    System.out.println(e);
	}

	// Set port direction
	try{
	    Files.write(GPIO_direction1,"in".getBytes(),StandardOpenOption.WRITE);
	    Files.write(GPIO_direction2,"in".getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){
	    System.out.println(e);
	}

	double timeStart = (double) System.nanoTime()/1E9;
	
	while(true){

	    double timeNow = (double) System.nanoTime()/1E9;
	    byte[] input1,input2, buffer;
	    try{
		// Read input from port
		input1  = Files.readAllBytes(GPIO_value1) ;
		input2  = Files.readAllBytes(GPIO_value2) ;
				
		// Constuct MAVLink message
		msg_command_long msgSafeguard = new msg_command_long();
		msgSafeguard.command = MAV_CMD.MAV_CMD_USER_1;
		msgSafeguard.param1 = 10;
		if((int) input1[0] == 48 ){

		    if((int) input2[0] == 48 ){
			msgSafeguard.param2         = 0;
		    }
		    else{
			msgSafeguard.param2         = 1;
		    }
		    
		}
		else{

		    if((int) input2[0] == 48 ){
			msgSafeguard.param2        = 2;
		    }
		    else{
			msgSafeguard.param2        = 3;
		    }
		}
								
		MAVLinkPacket raw_packet = msgSafeguard.pack();
		buffer            = raw_packet.encodePacket();
		
		// Send MAVLink message via socket to ICAROUS
		if(timeNow - timeStart > 0.5){
		    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
		    sock.send(output);
		    timeStart = timeNow;
		}
		
	    }
	    catch(IOException e){
		System.out.println(e);
	    }
	}
	

	

   }
}
