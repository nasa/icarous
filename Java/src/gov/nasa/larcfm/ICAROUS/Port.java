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
package gov.nasa.larcfm.ICAROUS;

import java.io.*;
import java.net.*;
import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class Port{
	
	public class PortType{
		public static final short SOCKET = 0;
		public static final short SERIAL = 1;
	}
	
	private short pType;
	private int udpReceivePort    = 0;
	private int udpSendPort       = 0;
	private String serialPortName = null;
	private InetAddress host      = null;
	private DatagramSocket sock   = null;
	private SerialPort serialPort = null;
	public byte[] buffer_data     = null;

	public Port(short pt,String hostname,int recvPort,int sendPort){
		pType           = pt;
		udpReceivePort  = recvPort;
		udpSendPort     = sendPort;

		if(hostname != null){
			try{
				host           = InetAddress.getByName(hostname);
			}catch(UnknownHostException e){
				System.out.println(e);
			}
		}

		InitSocketInterface();
		SetTimeout(1);
	}

	public Port(short pt,String portName,int BAUDRATE){
		pType          = pt;
		serialPortName = portName;

		InitSerialInterface(BAUDRATE);
		SetTimeout(1);
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
		if(pType == PortType.SOCKET){
			try{
				sock.setSoTimeout(timeout);
			}
			catch(SocketException e){
				System.out.println(e);
			}
		}
	}

	public void InitSerialInterface(int BAUDRATE){
		serialPort = new SerialPort(serialPortName);
		try {
			serialPort.openPort();
			serialPort.setParams(BAUDRATE, 
					SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);			
			//serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT);			

		}
		catch(SerialPortException ex){
			System.out.println(ex);
		}

		System.out.println("Connected to serial port:"+serialPortName);
	}
	
	public void EnableFlowControl(){
		if(pType == PortType.SOCKET){
			return;
		}else{
			try{
				serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT);
			}catch(SerialPortException ex){
				System.out.println(ex);
			}
		}
		
	}

	public byte[] UDPRead(){
		byte[] buffer = new byte[6+255+2];
		buffer_data = null;	    
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

		}catch(SocketTimeoutException e){

		}catch(IOException e){
			System.out.println(e);
		}	
		return buffer_data;
	}

	public void UDPWrite(byte[] buffer){	
		try{	    

			DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
			sock.send(output);
		}
		catch(IOException e){
			System.err.println(e);
		}
	}   

	public byte[] SerialRead(){
		buffer_data = null;		
		try{
			buffer_data = serialPort.readBytes();
		}
		catch(SerialPortException e){
			System.out.println(e);
		}
		
		return buffer_data;
	}

	public void SerialWrite(byte[] buffer){
		try{
			serialPort.writeBytes(buffer);
		}
		catch(SerialPortException e){
			System.err.println(e);
		}	
	}

	public synchronized byte[] Read(){
	
		byte[] buffer = null;
		if(pType == PortType.SOCKET){
			buffer = UDPRead();
		}
		else if(pType == PortType.SERIAL){
			buffer = SerialRead();
		}
		else{
			System.out.println("Unknown interface");	    
		}

		return buffer;
	}

	public synchronized void Write(byte[] buffer){			    	   
			if(pType == PortType.SOCKET){
				this.UDPWrite(buffer);
			}
			else if (pType == PortType.SERIAL){
				this.SerialWrite(buffer);
			}	
	}
}
