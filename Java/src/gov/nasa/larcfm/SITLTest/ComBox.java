import java.io.*;
import java.net.*;
import com.MAVLink.*;
import com.MAVLink.Messages.*;
import com.MAVLink.icarous.*;


public class ComBox{

    public static void UDPWrite(MAVLinkMessage msg2send,DatagramSocket sock,InetAddress host, int udpSendPort){
	
	MAVLinkPacket raw_packet = msg2send.pack();
	byte[] buffer            = raw_packet.encodePacket();

	try{
	    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
	    sock.send(output);
	    System.out.println("Sent command");
	}
	catch(IOException e){
	    System.err.println(e);
	}

    }

    public static void ParseMessage(byte[] input){
	
	MAVLinkPacket RcvdPacket = null;

	Parser MsgParser = new Parser();
	
	for(int i=0;i<(6+255+2);i++){
	    
	    RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & input[i]);

	    if(RcvdPacket != null){

		RcvdPacket = null;
	    }
	}
    }

    
    public static void MultiUDPRead(MulticastSocket sock){

	byte[] buffer = new byte[6+255+2];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);

	try{
	    sock.receive(input);
	    
	    buffer_data   = input.getData();
		
	    ParseMessage(buffer_data);	    
	    
	}catch(SocketTimeoutException e){
	    
	}catch(IOException e){
	    System.out.println(e);
	}
	
	
    }

    public static msg_command_acknowledgement UDPRead(DatagramSocket sock){

	byte[] buffer = new byte[6+255+2];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);
	
	Parser MsgParser = new Parser();
	
	try{
	    sock.receive(input);
	    
	    buffer_data   = input.getData();
		
	    MAVLinkPacket RcvdPacket = null;
	
	    for(int i=0;i<(6+255+2);i++){
		
		RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer_data[i]);
		
		if(RcvdPacket != null){
		    System.out.println("Received acknowledgement");
		    return (msg_command_acknowledgement) RcvdPacket.unpack();
		    
		}
	    }
	    
	}catch(IOException e){
	    System.out.println(e);
	}
	
	return null;
    }

    
    public static void main(String args[]){

	MAVLinkMessage msg2send;
	MAVLinkPacket raw_packet;
	byte[] receiveData = new byte[6+255+2];

	InetAddress host = null;
	int updSendPort  = 0;

	String udpHost   = args[0];
	int udpSendPort  = Integer.parseInt(args[1]);
	DatagramSocket sock = null;

	DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);

	MulticastSocket multisock = null;
	
	try{
	    multisock = new MulticastSocket(5555);
	    InetAddress mcAddress = InetAddress.getByName("230.1.1.1");
	    multisock.joinGroup(mcAddress);
	}
	catch(IOException e){
	    System.err.println(e);
	    multisock = null;
	}

	

	
	msg_command_acknowledgement ack = new msg_command_acknowledgement();


	
	try{
	    host  = InetAddress.getByName(udpHost);
	    sock  = new DatagramSocket();
	}
	catch(IOException e){
	    System.err.println(e);
	}
       
	

	
	// Assemble all the messages to be sent

	// COMBOX hearbeat
	msg_heartbeat_combox msgHeartBeatCombox  = new msg_heartbeat_combox();
	msgHeartBeatCombox.count             = (long) 1;

	// Flightplan information
	msg_flightplan_info msgFlightPlanInfo = new msg_flightplan_info();
	msgFlightPlanInfo.msgType      = 0;
	msgFlightPlanInfo.numWaypoints = 4;
	msgFlightPlanInfo.maxHorDev    = 2;
	msgFlightPlanInfo.maxVerDev    = 3;
	msgFlightPlanInfo.standoffDist = 1;

	// Send four waypoints
	msg_pointofinterest wp1 = new msg_pointofinterest();
	wp1.id      = 0;
	wp1.index   = 0;
	wp1.subtype = 0;
	wp1.latx    = 37.615759f;
	wp1.lony    = -122.365150f;
	wp1.altz    = 50.0f;
	wp1.heading = 0.0f;

	msg_pointofinterest wp2 = new msg_pointofinterest();
	wp2.id      = 0;
	wp2.index   = 1;
	wp2.subtype = 0;
	wp2.latx    = 37.614369f; 
	wp2.lony    = -122.361927f;
	wp2.altz    = 20.0f;
	wp2.heading = 0.0f;
	
	msg_pointofinterest wp3 = new msg_pointofinterest();
	wp3.id      = 0;
	wp3.index   = 2;
	wp3.subtype = 0;
	wp3.latx    = 37.617655f;
	wp3.lony    = -122.359112f;
	wp3.altz    = 20.0f;
	wp3.heading = 0.0f;

	msg_pointofinterest wp4 = new msg_pointofinterest();
	wp4.id      = 0;
	wp4.index   = 3;
	wp4.subtype = 0;
	wp4.latx    = 37.617624f;
	wp4.lony    = -122.363878f;
	wp4.altz    = 20.0f;
	wp4.heading = 0.0f;

	msg_mission_start_stop msgMissionStart = new msg_mission_start_stop();

	msgMissionStart.missionStart = 1;

	msg_geofence_info msgGeoFenceInfo = new msg_geofence_info();
	msgGeoFenceInfo.msgType   = (short) 0;
	msgGeoFenceInfo.fenceID   = (short) 0;
	msgGeoFenceInfo.fenceType =  (short) 0;
	msgGeoFenceInfo.numVertices = 4;
	msgGeoFenceInfo.fenceFloor = 0;
	msgGeoFenceInfo.fenceCeiling = 1000;

	// Send four waypoints
	msg_pointofinterest gf1 = new msg_pointofinterest();
	gf1.id      = 1;
	gf1.index   = 0;
	gf1.subtype = 0;
	gf1.latx    = 37.609712f;
	gf1.lony    = -122.359299f;
	gf1.altz    = 0.0f;
	gf1.heading = 0.0f;

	msg_pointofinterest gf2 = new msg_pointofinterest();
	gf2.id      = 1;
	gf2.index   = 1;
	gf2.subtype = 0;
	gf2.latx    = 37.614297f; 
	gf2.lony    = -122.355688f;
	gf2.altz    = 0.0f;
	gf2.heading = 0.0f;
	
	msg_pointofinterest gf3 = new msg_pointofinterest();
	gf3.id      = 1;
	gf3.index   = 2;
	gf3.subtype = 0;
	gf3.latx    = 37.618960f;
	gf3.lony    = -122.366671f;
	gf3.altz    = 0.0f;
	gf3.heading = 0.0f;

	msg_pointofinterest gf4 = new msg_pointofinterest();
	gf4.id      = 1;
	gf4.index   = 3;
	gf4.subtype = 0;
	gf4.latx    =  37.614623f;
	gf4.lony    = -122.370068f;
	gf4.altz    = 0.0f;
	gf4.heading = 0.0f;
	

	
	// Send all messages
	try{
	    UDPWrite(msgHeartBeatCombox,sock,host,udpSendPort);	Thread.sleep(5000);

	    UDPWrite(msgFlightPlanInfo,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(wp1,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(wp2,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(wp3,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(wp4,sock,host,udpSendPort); Thread.sleep(100);
	    
	    ack = UDPRead(sock);

	    if(ack.acktype == 0 && ack.value == 1){
		System.out.println("Waypoints sent successfully");
		UDPWrite(msgMissionStart,sock,host,udpSendPort);
	    }
	    else{
		System.out.println("Resend waypoints");
	    }

	    
	    Thread.sleep(1000);

	    UDPWrite(msgGeoFenceInfo,sock,host,udpSendPort);Thread.sleep(100);

	    UDPWrite(gf1,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(gf2,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(gf3,sock,host,udpSendPort); Thread.sleep(100);
	    
	    UDPWrite(gf4,sock,host,udpSendPort); Thread.sleep(100);
	    
	    ack = UDPRead(sock);	
	
	    
	}
	catch(InterruptedException e){
	    System.out.println(e);
	}

	    
	}
    
}
