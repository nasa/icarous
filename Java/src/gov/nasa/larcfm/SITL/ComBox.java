import java.io.*;
import java.net.*;
import java.util.*;
import com.MAVLink.*;
import com.MAVLink.Messages.*;
import com.MAVLink.icarous.*;

import gov.nasa.larcfm.Util.SeparatedInput;
import gov.nasa.larcfm.Util.ParameterData;

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

    
    public static void main(String args[]) throws IOException{

	List<msg_pointofinterest> Waypoints = new ArrayList<msg_pointofinterest>();
	List<msg_pointofinterest> Geofence  = new ArrayList<msg_pointofinterest>();
	
	MAVLinkMessage msg2send;
	MAVLinkPacket raw_packet;
	byte[] receiveData = new byte[6+255+2];

	InetAddress host = null;
	int updSendPort  = 0;

	DatagramSocket sock = null;
	
	FileReader in = new FileReader(args[0]);
	SeparatedInput reader = new SeparatedInput(in);
	
	reader.readLine();
	ParameterData parameters = reader.getParametersRef();

	String udpHost   = parameters.getString("Host");
	int udpSendPort  = parameters.getInt("Port");
	
	
	DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);

	MulticastSocket multisock = null;
	
	try{
	    multisock = new MulticastSocket(parameters.getInt("mcPort"));
	    InetAddress mcAddress = InetAddress.getByName(parameters.getString("mcAddress"));
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
	msgHeartBeatCombox.count                 = (long) 1;

	// Flightplan information
	msg_flightplan_info msgFlightPlanInfo = new msg_flightplan_info();
	msgFlightPlanInfo.msgType      = 0;
	msgFlightPlanInfo.numWaypoints = parameters.getInt("NumWP");
	msgFlightPlanInfo.maxHorDev    = parameters.getInt("maxHDev");
	msgFlightPlanInfo.maxVerDev    = parameters.getInt("maxVDev");
	msgFlightPlanInfo.standoffDist = parameters.getInt("standoff");

	msg_geofence_info msgGeoFenceInfo = new msg_geofence_info();
	msgGeoFenceInfo.msgType   = 0;
	msgGeoFenceInfo.fenceID   = 1;
	msgGeoFenceInfo.fenceType =  0;
	msgGeoFenceInfo.numVertices = parameters.getInt("NumVertices");
	msgGeoFenceInfo.fenceFloor = parameters.getInt("Floor");
	msgGeoFenceInfo.fenceCeiling = parameters.getInt("Ceiling");
	

	for(int i=0;i<msgFlightPlanInfo.numWaypoints;i++){
	    msg_pointofinterest wp = new msg_pointofinterest();
	    wp.id      = (byte)reader.getColumn(0);
	    wp.index   = (byte)reader.getColumn(1);
	    wp.subtype = (byte)reader.getColumn(2);
	    wp.latx    = (float)reader.getColumn(3);
	    wp.lony    = (float)reader.getColumn(4);
	    wp.altz    = (float)reader.getColumn(5);
	    wp.heading = (float)reader.getColumn(6);

	    Waypoints.add(wp);
	    reader.readLine();
	}

	for(int i=0;i<msgGeoFenceInfo.numVertices;i++){
	    msg_pointofinterest gf = new msg_pointofinterest();
	    gf.id      = (byte)reader.getColumn(0);
	    gf.index   = (byte)reader.getColumn(1);
	    gf.subtype = (byte)reader.getColumn(2);
	    gf.latx    = (float)reader.getColumn(3);
	    gf.lony    = (float)reader.getColumn(4);
	    gf.altz    = (float)reader.getColumn(5);
	    gf.heading = (float)reader.getColumn(6);

	    Geofence.add(gf);
	    reader.readLine();
	}


	msg_mission_start_stop msgMissionStart = new msg_mission_start_stop();

	msgMissionStart.missionStart = 1;
	
	// Send all messages
	try{
	    UDPWrite(msgHeartBeatCombox,sock,host,udpSendPort);	Thread.sleep(5000);

	    UDPWrite(msgFlightPlanInfo,sock,host,udpSendPort); Thread.sleep(100);

	    for(int i=0;i<Waypoints.size();i++){
		msg_pointofinterest wp = (msg_pointofinterest) Waypoints.get(i);
		UDPWrite(wp,sock,host,udpSendPort); Thread.sleep(100);
	    }
	    	    
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

	    for(int i=0;i<Geofence.size();i++){
		msg_pointofinterest gf = (msg_pointofinterest) Geofence.get(i);
		UDPWrite(gf,sock,host,udpSendPort); Thread.sleep(100);
	    }	    
	    ack = UDPRead(sock);	

	    if(ack.acktype == 1 && ack.value == 1){
		System.out.println("Geofence sent successfully");
	    }
	    else{
		System.out.println("Resend geofence");
	    }
	    
	}
	catch(InterruptedException e){
	    System.out.println(e);
	}

	double CurrentTime, ElapsedTime, StartTime;

	
	CurrentTime = (double) System.nanoTime()/1E9;
	StartTime   = CurrentTime;
	
	boolean ObjectDetected = false;

	while(!ObjectDetected){
	    CurrentTime = (double) System.nanoTime()/1E9; 
	    ElapsedTime = CurrentTime - StartTime;

	    if(ElapsedTime > 40){
		ObjectDetected = true;
	    }
	}

	msg_pointofinterest obj = new msg_pointofinterest();

	obj.id      = 4;
	obj.index   = 0;
	obj.subtype = 0;

	UDPWrite(obj,sock,host,udpSendPort);

	ack = UDPRead(sock);

	if(ack.acktype == 4 && ack.value == 1){
	    System.out.println("Object sent successfully");	 
	}
	else{
	    System.out.println("Resend object");
	}
	
	
    }
    
}
