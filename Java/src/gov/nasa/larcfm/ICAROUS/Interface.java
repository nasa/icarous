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
    
    
    public Interface(int intType,String hostname,int recvPort,int sendPort){

	interfaceType   = (short) intType;
	componentType   = (short) compType;
	udpReceivePort  = recvPort;
	udpSendPort     = sendPort;
	SharedData      = msgs;

	if(hostname != null){
	    try{
		host           = InetAddress.getByName(hostname);
	    }catch(UnknownHostException e){
		System.out.println(e);
	    }
	}
	
	InitSocketInterface();
    }

    public Interface(int intType,String portName){

	interfaceType  = (short) intType;
	componentType  = (short) compType;
	SharedData     = msgs;
	serialPortName = portName;

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
	    serialPort.setParams(SerialPort.BAUDRATE_57600, 
				 SerialPort.DATABITS_8,
				 SerialPort.STOPBITS_1,
				 SerialPort.PARITY_NONE);
	
	}
	catch(SerialPortException ex){
	    System.out.println(ex);
	}

	System.out.println("Connected to serial port:"+serialPortName);

    }
	
    public MAVLinkPacket UDPRead(){

	byte[] buffer = new byte[6+255+2];
	byte[] buffer_data;
	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);

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
		
	    return ParseMessage(buffer_data);	    
	    
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
	}
	catch(IOException e){
	    System.err.println(e);
	}

    }

    public MAVLinkPacket SerialRead(){

	byte[] buffer = null;
	
	try{
	    buffer = serialPort.readBytes(6+255+2);
	}
	catch(SerialPortException e){
	    System.out.println(e);
	}

	return ParseMessage(buffer);
    }

    public void SerialWrite(MAVLinkMessage msg2send){

	MAVLinkPacket raw_packet = msg2send.pack();
	byte[] buffer            = raw_packet.encodePacket();

	try{
	    serialPort.writeBytes(buffer);
	    System.err.println("Sent command:");
	}
	catch(SerialPortException e){
	    System.err.println(e);
	}

	
    }
	
    public synchronized MAVLinkPacket Read(){
	if(interfaceType == SOCKET){
	    return UDPRead();
	}
	else if(interfaceType == SERIAL){
	    return SerialRead();
	}
	else{
	    System.out.println("Unknown interface");
	    return null;
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
