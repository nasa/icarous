package InterfaceTest;

import java.io.*;
import java.net.*;
import com.MAVLink.Parser;
import com.MAVLink.Messages.*;
import com.MAVLink.common.*;
import com.MAVLink.MAVLinkPacket;


public class ICAROUS_Interface{

    public static short PX4       = 0;
    public static short SITL      = 1;
    public static short COMBOX    = 2;
    public static short SAFEGUARD = 3;
    
    private DatagramSocket sock   = null;
    private int udpReceivePort    = 0;
    private int udpSendPort       = 0;
    private String udpHost        = null;
    private String serialPort     = null;
    private InetAddress host      = null;
    private Parser MsgParser      = new Parser();
    public short interfaceType    = 0;
    MAVLinkMessages RcvdMessages  = null;

    private msg_command_long CommandLong  = new msg_command_long();

    public ICAROUS_Interface(String hostName,int receivePort,MAVLinkMessages msgs){

	interfaceType   = SITL;
	udpHost         = hostName;
	udpReceivePort  = receivePort;
	RcvdMessages    = msgs;
	InitSocketInterface();
	
    }

    public ICAROUS_Interface(String portName, MAVLinkMessages msgs){

	interfaceType  = PX4;
	RcvdMessages   = msgs;
    }


    public void InitSocketInterface(){
	
	try{
	    host  = InetAddress.getByName(udpHost);
	    sock  = new DatagramSocket(udpReceivePort);
	}
	catch(IOException e){
	    System.err.println(e);
	}
	
	byte[] buffer = new byte[65536];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);

	// Initial read to get port number to send messages
	System.out.println("Waiting for input from port:"+udpReceivePort);
	try{
	    sock.receive(input);
	    udpSendPort = input.getPort();
	}catch(IOException e){
	    System.out.println(e);
	}

	CheckHeartBeat();

    }

    public void CheckHeartBeat(){
	System.out.println("Waiting for hearbeat...");
	while(true){
	    this.UDPRead();
	    if(RcvdMessages.msgHeartbeat != null){
		System.out.println("Got heart beat");
		break;
	    }
	}
	
    }

    
    public void InitSerialInterface(){

	//TODO:
    }

    public void UDPRead(){

	byte[] buffer = new byte[65536];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);

	try{
	    sock.receive(input);
		
	    buffer_data   = input.getData();
		
	    this.ParseMessage(buffer_data);
	    
	}catch(IOException e){
	    System.out.println(e);
	}
	
	
    }

    public void UDPWrite(){

	MAVLinkPacket raw_packet = CommandLong.pack();
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

    public void SerialRead(){
	//TODO;
    }

    public void SerialWrite(){
	//TODO;
    }
	
    public void AP_Read(){
	if(interfaceType == SITL){
	    this.UDPRead();
	}
	else{
	    this.SerialRead();
	}
    }

    public void AP_Write(){
	if(interfaceType == SITL){
	    this.UDPWrite();
	}
	else{
	    this.SerialWrite();
	}
    }

    public void ParseMessage(buffer[] input){
	
	MAVLinkPacket RcvdPacket = null;
	
	for(int i=0;i<(6+255+2);i++){
	    
	    RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer_data[i]);
	    
	    if(RcvdPacket != null){
		RcvdMessages.decode_message(RcvdPacket);
	    }
	}
	

    }
    
    public int SendCommand( int target_system,
			    int target_component,
			    int confirmation,
			    int command,
			    float param1,
			    float param2,
			    float param3,
			    float param4,
			    float param5,
			    float param6,
			    float param7){

	
	CommandLong.target_system     = (short) target_system;
	CommandLong.target_component  = (short) target_component;
	CommandLong.command           = command;
	CommandLong.confirmation      = (short) confirmation;
	CommandLong.param1            = param1;
	CommandLong.param2            = param2;
	CommandLong.param3            = param3;
	CommandLong.param4            = param4;
	CommandLong.param5            = param5;
	CommandLong.param6            = param6;
	CommandLong.param7            = param7;
	
	this.AP_Write();

	try{
	    Thread.sleep(100);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	synchronized(RcvdMessages){
	    if(RcvdMessages.msgCommandAck.command == CommandLong.command &&
	       RcvdMessages.msgCommandAck.result == 0 ){
		return 1;
	    }
	    else{
		System.out.println("Command not accepted\n");
		return 0;
	    }
	}
    }
    
    
    
    
}
