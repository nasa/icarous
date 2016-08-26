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
 */
package gov.nasa.larcfm.ICAROUS;

import java.io.*;
import java.net.*;
import com.MAVLink.Parser;
import com.MAVLink.Messages.*;
import com.MAVLink.common.*;
import com.MAVLink.MAVLinkPacket;
import jssc.SerialPort;
import jssc.SerialPortException;

public class Interface{

    public static short PX4       = 1;
    public static short COM       = 2;
    public static short SAFEGUARD = 3;
    public static short BROADCAST = 4;

    public static short SOCKET    = 0;
    public static short SERIAL    = 1;
    
    public short interfaceType    = 0;
    private int udpReceivePort    = 0;
    private int udpSendPort       = 0;
    private String udpHost        = null;
    private String serialPortName = null;
    private InetAddress host      = null;
    private DatagramSocket sock   = null;
    private SerialPort serialPort = null;
    private Parser MsgParser      = new Parser();
    private MAVLinkMessages Inbox;
    
    public Interface(int intType,String hostname,int recvPort,int sendPort,AircraftData acData){

	interfaceType   = (short) intType;
	udpReceivePort  = recvPort;
	udpSendPort     = sendPort;
	Inbox           = acData.Inbox;

	if(hostname != null){
	    try{
		host           = InetAddress.getByName(hostname);
	    }catch(UnknownHostException e){
		System.out.println(e);
	    }
	}
	
	InitSocketInterface();
    }

    public Interface(int intType,String portName,AircraftData acData){

	interfaceType  = (short) intType;
	serialPortName = portName;
	Inbox          = acData.Inbox;

	InitSerialInterface();
    }

    public void InitSocketInterface(){
	
	try{
	    if(udpReceivePort != 0){
		sock  = new DatagramSocket(udpReceivePort);
	    }
	    else{
		sock  = new DatagramSocket();
	    }
	}
	catch(IOException e){
	    System.err.println(e);
	}
			
    }

    public void SetTimeout(int timeout){
	try{
	    sock.setSoTimeout(timeout);
	}
	catch(SocketException e){
	    System.out.println(e);
	}
    }
    
    public void InitSerialInterface(){

	serialPort = new SerialPort(serialPortName);

	try {
	    serialPort.openPort();
	    serialPort.setParams(SerialPort.BAUDRATE_115200, 
				 SerialPort.DATABITS_8,
				 SerialPort.STOPBITS_1,
				 SerialPort.PARITY_NONE);
	
	}
	catch(SerialPortException ex){
	    System.out.println(ex);
	}

	System.out.println("Connected to serial port:"+serialPortName);

    }
	
    public void UDPRead(){

	byte[] buffer = new byte[1000];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);
	MAVLinkPacket RcvdPacket = null;
	
	try{
	    sock.receive(input);

	    // Get port number on host to send data
	    if(udpSendPort == 0){
		udpSendPort = input.getPort();		
	    }

	    if(host == null){
		host = input.getAddress();
	    }
	    
	    buffer_data   = input.getData();
	    
	    for(int i=0;i<1000;i++){
		RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer_data[i]);
		Inbox.decode_message(RcvdPacket);	
	    }
	    	    	    
	}catch(SocketTimeoutException e){
	    
	}catch(IOException e){
	    System.out.println(e);
	}	

    }

    public void UDPWrite(MAVLinkMessage msg2send){

	MAVLinkPacket raw_packet = msg2send.pack();

	raw_packet.sysid  = msg2send.sysid;
	raw_packet.compid = msg2send.compid;
	
	byte[] buffer            = raw_packet.encodePacket();

	try{

	    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
	    sock.send(output);
	}
	catch(IOException e){
	    System.err.println(e);
	}

    }   

    public void SerialRead(){

	byte[] buffer = null;
	MAVLinkPacket RcvdPacket = null;
	
	try{
	    buffer = serialPort.readBytes(1);
	}
	catch(SerialPortException e){
	    System.out.println(e);
	}

	for(int i=0;i<(1);i++){
	    
	    RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);	    
	    Inbox.decode_message(RcvdPacket);	    	    	
	}
    }

    public void SerialWrite(MAVLinkMessage msg2send){

	MAVLinkPacket raw_packet = msg2send.pack();	
	raw_packet.sysid  = msg2send.sysid;
	raw_packet.compid = msg2send.compid;

	byte[] buffer            = raw_packet.encodePacket();
	
	try{
	    serialPort.writeBytes(buffer);
	}
	catch(SerialPortException e){
	    System.err.println(e);
	}

	
    }
	
    public synchronized void Read(){
	if(interfaceType == SOCKET){
	    UDPRead();
	}
	else if(interfaceType == SERIAL){
	    SerialRead();
	}
	else{
	    System.out.println("Unknown interface");
	    
	}
    }

    public void Write(MAVLinkMessage msg2send){

	
	    if(interfaceType == SOCKET){
		this.UDPWrite(msg2send);
	    }
	    else if (interfaceType == SERIAL){
		this.SerialWrite(msg2send);
	    }
	
    }

    public MAVLinkPacket ParseMessage(byte[] input){
	
	MAVLinkPacket RcvdPacket = null;
	
	for(int i=0;i<(6+255+2);i++){
	    
	    RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & input[i]);


	    if(RcvdPacket != null){
		return RcvdPacket;
	    }
	
	}
	
	return null;
	    
	
    }
    
        
}
