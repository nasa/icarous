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
	msg_combox_pulse msgComboxPulse  = new msg_combox_pulse();
	msgComboxPulse.count             = (long) 1;

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
	wp1.lat     = 37.611865f;
	wp1.lon     = -122.3754350f;
	wp1.alt     = 20.0f;
	wp1.heading = 0.0f;

	msg_pointofinterest wp2 = new msg_pointofinterest();
	wp2.id      = 0;
	wp2.index   = 1;
	wp1.subtype = 0;
	wp2.lat     = 37.615267f; 
	wp2.lon     = -122.373179f;
	wp2.alt     = 20.0f;
	wp2.heading = 0.0f;
	
	msg_pointofinterest wp3 = new msg_pointofinterest();
	wp3.id      = 0;
	wp3.index   = 2;
	wp1.subtype = 0;
	wp3.lat     = 37.616911f;
	wp3.lon     = -122.377167f;
	wp3.alt     = 20.0f;
	wp3.heading = 0.0f;

	msg_pointofinterest wp4 = new msg_pointofinterest();
	wp4.id      = 0;
	wp4.index   = 3;
	wp1.subtype = 0;
	wp4.lat     = 37.612451f;
	wp4.lon     = -122.380198f;
	wp4.alt     = 20.0f;
	wp4.heading = 0.0f;

	msg_mission_start_stop msgMissionStart = new msg_mission_start_stop();

	msgMissionStart.missionStart = 1;
	
	// Send all messages
	try{
	UDPWrite(msgComboxPulse,sock,host,udpSendPort);	Thread.sleep(5000);

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
	}
	catch(InterruptedException e){
	    System.out.println(e);
	}

	
    }

    
    

    
}
