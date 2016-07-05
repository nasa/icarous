/**
 * ICAROUS Interface
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import java.io.*;
import java.net.*;
import com.MAVLink.Parser;
import com.MAVLink.Messages.*;
import com.MAVLink.common.*;
import com.MAVLink.MAVLinkPacket;
import jssc.SerialPort;
import jssc.SerialPortException;

public class ICAROUS_Interface{

    public static short PX4       = 0;
    public static short SITL      = 1;
    public static short COMBOX    = 2;
    public static short SAFEGUARD = 3;
    
    private DatagramSocket sock   = null;
    private int udpReceivePort    = 0;
    private int udpSendPort       = 0;
    private String udpHost        = null;
    private String serialPortName = null;
    private InetAddress host      = null;
    private Parser MsgParser      = new Parser();
    public short interfaceType    = 0;
    AircraftData SharedData       = null;
    SerialPort serialPort         = null;


    public ICAROUS_Interface(int intType,String hostName,int receivePort,int timeout,AircraftData msgs){

	interfaceType   = (short) intType;
	udpHost         = hostName;
	udpReceivePort  = receivePort;
	SharedData      = msgs;
	InitSocketInterface(timeout);
	
    }

    public ICAROUS_Interface(int intType,String portName, AircraftData msgs){

	interfaceType  = (short) intType;
	SharedData     = msgs;
	serialPortName = portName;
	InitSerialInterface();
    }


    public void InitSocketInterface(int timeout){
	
	try{
	    host  = InetAddress.getByName(udpHost);
	    sock  = new DatagramSocket(udpReceivePort);
	}
	catch(IOException e){
	    System.err.println(e);
	}
		
	if(interfaceType == SITL){
	    CheckAPHeartBeat();
	}
	else if(interfaceType == COMBOX){
	    CheckCOMHeartBeat();
	}

	try{
	    sock.setSoTimeout(timeout);
	}
	catch(SocketException e){
	    System.out.println(e);
	}

    }

    public void CheckAPHeartBeat(){
	System.out.println("Waiting for heartbeat from autopilot...");
	while(true){
	    this.Read();
	    if(SharedData.RcvdMessages.msgHeartbeat != null){
		System.out.println("Got heart beat");
		break;
	    }
	}
	
    }

    public void CheckCOMHeartBeat(){
	System.out.println("Waiting for heartbeat from combox...");
	while(true){
	    this.Read();
	    if(SharedData.RcvdMessages.msgComboxPulse != null){
		System.out.println("Got heart beat from combox");
		break;
	    }
	}
	
    }

    
    public void InitSerialInterface(){

	serialPort = new SerialPort(serialPortName);

	try {
	    serialPort.openPort();
	    serialPort.setParams(SerialPort.BAUDRATE_57600, 
				 SerialPort.DATABITS_8,
				 SerialPort.STOPBITS_1,
				 SerialPort.PARITY_NONE);
	
	}
	catch(SerialPortException ex){
	    System.out.println(ex);
	}

	System.out.println("Connected to serial port:"+serialPortName);

	if(interfaceType == PX4){
	    CheckAPHeartBeat();
	}
    }
	
    public void UDPRead(){

	byte[] buffer = new byte[6+255+2];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);

	try{
	    sock.receive(input);

	    // Get port number on host to send data
	    if(udpSendPort == 0){
		udpSendPort = input.getPort();
	    }
	    
	    buffer_data   = input.getData();
		
	    this.ParseMessage(buffer_data);	    
	    
	}catch(SocketTimeoutException e){
	    
	}catch(IOException e){
	    System.out.println(e);
	}
	
	
    }

    public void UDPWrite(MAVLinkMessage msg2send){

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

    public void SerialRead(){

	byte[] buffer = null;
	
	try{
	    buffer = serialPort.readBytes(6+255+2);
	}
	catch(SerialPortException e){
	    System.out.println(e);
	}

	ParseMessage(buffer);
    }

    public void SerialWrite(MAVLinkMessage msg2send){

	MAVLinkPacket raw_packet = msg2send.pack();
	byte[] buffer            = raw_packet.encodePacket();

	try{
	    serialPort.writeBytes(buffer);
	    System.out.println("Sent command");
	}
	catch(SerialPortException e){
	    System.err.println(e);
	}

	
    }
	
    public void Read(){
	if(interfaceType == SITL || interfaceType == COMBOX){
	    this.UDPRead();
	}
	else{
	    this.SerialRead();
	}
    }

    public void Write(MAVLinkMessage msg2send){

	
	    if(interfaceType == SITL || interfaceType == COMBOX){
		this.UDPWrite(msg2send);
	    }
	    else{
		this.SerialWrite(msg2send);
	    }
	
    }

    public void ParseMessage(byte[] input){
	
	MAVLinkPacket RcvdPacket = null;
	
	for(int i=0;i<(6+255+2);i++){
	    
	    RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & input[i]);

	    synchronized(SharedData){
		if(RcvdPacket != null){
		    SharedData.RcvdMessages.decode_message(RcvdPacket);
		    RcvdPacket = null;
		}
	    }
	}
	

    }
    
    
    
    
}
