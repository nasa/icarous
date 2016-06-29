/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import gov.nasa.larcfm.Util.f;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

public class IcarousComms {

	public static void startCommPassThroughThread(int port, Icarous i) {
		CommPassThroughThread pt = new CommPassThroughThread(port, i);
		pt.start();
	}

	public static void startCommHBThread(int port) {
		CommHBThread hb = new CommHBThread(port);
		hb.start();
	}

	public static void startSerialClientThread(String gps_serial_port, Icarous icarous, int timeout) {
		SerialClientThread gps = new SerialClientThread(gps_serial_port, icarous, timeout, false);
		Timer timer = new Timer();
		timer.schedule(gps, 0, 1000); //try to read gps every second
	}
}




class CommPassThroughThread extends Thread {
	private ServerSocket ssocket;
	private int inport;
	private Icarous icky;

	public CommPassThroughThread(int port, Icarous i) {
		super("CommPassThroughThread");
		inport = port;
		icky = i;
		ssocket = null;
		try {
			ssocket = new ServerSocket(inport);
		} catch (IOException e) {
			f.pln("Could not open passthrough socket on port "+inport);
			e.printStackTrace();
		}
	}

	public void run() {
		try (   Socket socket = ssocket.accept();
				PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
				BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				) {
			f.pln("ICAROUS Starting PassThrough Socket Server");
			while (true) {
				String data = icky.getGPSData();
				f.pln("ICAROUS Sending GPS data: "+data);
				out.println(data);
				Thread.sleep(1000);
			}
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}



class CommHBThread extends Thread {
	private ServerSocket ssocket;
	private int inport;

	public CommHBThread(int port) {
		super("CommHBThread");
		ssocket = null;
		try {
			inport = port;
			ssocket = new ServerSocket(inport);
		} catch (IOException e) {
			f.pln("Could not open heartbeat socket on port "+inport);
			e.printStackTrace();
		}
	}

	public void run() {
		try (   Socket socket = ssocket.accept();
				BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				) {
			String inputLine;
			f.pln("ICAROUS Starting HB Socket Listener");
			while ((inputLine = in.readLine()) != null) {
				f.pln("ICAROUS HB RCV: "+inputLine);
			}			
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
}

// timeout is in milliseconds
class SerialClientThread extends TimerTask implements SerialPortEventListener {
	private SerialPort sp;
	private Icarous icky;
	private boolean portOk;
	private String lineval;
	private int timeout;
	private boolean async;

	public SerialClientThread(String com, Icarous ic, int timeout, boolean setAsync) {
		String[] portNames = SerialPortList.getPortNames();
		sp = new SerialPort(com);
		icky = ic;
		portOk = false;
		this.timeout = timeout;
		try {
			sp.openPort();
			sp.setParams(115200, 8, 1, 0);
			portOk = true;
		} catch (SerialPortException e) {
			f.pln("Could not open port "+com);
			f.pln("Visible Serial Ports: "+portNames.length);
			for (int i = 0; i < portNames.length; i++) {
				f.pln(i+" :: "+portNames[i]);
			}
			e.printStackTrace();
		}
		async = setAsync;
	}

	//assumes command starts with a $
	static String packet(String cmd) {
		int crc = calcChecksum8(cmd.substring(1).getBytes());
		return cmd+"*"+Integer.toHexString(crc)+"\r\n";
	}

	public static int calcChecksum8(byte[] data) {
		int crc = 0;
		for (int i = 0; i < data.length; i++) {
			crc ^= data[i];
		}
		return crc;
	}

	public static int calcChecksum16(byte[] data) {
		int crc = 0;
		for (int i = 0; i < data.length; i++) {
			crc = (crc >> 8) | (crc << 8);
			crc ^= data[i];
			crc ^= (crc & 0xff) >> 4;
			crc ^= crc << 12;
			crc ^= (crc & 0x00ff) << 5;
		}
		return crc;
	}

	public void sendCommand(String cmd) {
		String val = new String();
		if (portOk) {
			try {
				String pkt = packet(cmd);
				byte[] buffer = pkt.getBytes();
				sp.writeBytes(buffer);
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
		}
	}


	public String readAsync() {
		StringBuffer val = new StringBuffer();
		sendCommand("$VNWRG,6,20");
		Pattern pat = Pattern.compile("\\*[0-9A-F][0-9A-F]");
		while (true) {
			// read current line as a string:
			String line;
			try {
				line = sp.readString();
				if (line != null) {
					val = val.append(line); // add line to buffer
					int idx1 = val.indexOf("$VN"); // check for message start (assume replies start with $VN)
					Matcher mat = pat.matcher(val);
					int idx2 = -1;
					if (mat.find()) { // check for message end (*XX, where X=hex digit)
						idx2 = mat.start();
					}
					// if buffer has both start and end marks, perform checksum
					if (idx2 > idx1 && val.length() >= idx2+3) {
						String num = val.substring(idx2+1,idx2+3); // checksum string (2 chars)
						int checksum = Integer.decode("0x"+num); // checksum val
						String tmp = val.substring(idx1,idx2+3); // $ + msg + checksum
						String msg = val.substring(idx1+1, idx2); // just the message 
						if (checksum == calcChecksum8(msg.getBytes())) {
							// checksum ok, return the base message (minus checksum)
							val = val.delete(0, idx2+3);
						} else {
							// checksum fail, search for next possible message
							f.pln("SerialClientThread CHECKSUM ERR "+val);
							val = val.delete(0, idx1+3);
						}
					}
				}
				Thread.sleep(1);
			} catch (SerialPortException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	/**
	 * Send a command string and return the result string
	 * @param cmd Command string to send
	 * @return Result string, or empty string if error (timeout or checksum failure)
	 */
	public String sendCommandWithResponse(String cmd) {
		String val = new String();
		if (portOk) {
			try {
				long stopTime = System.currentTimeMillis()+timeout;
				String pkt = packet(cmd);
				byte[] buffer = pkt.getBytes();
				sp.writeBytes(buffer);
				boolean done = false;
				Pattern pat = Pattern.compile("\\*[0-9A-F][0-9A-F]");
				while (!done && System.currentTimeMillis() < stopTime) {
					// read current line as a string:
					String line = sp.readString();
					if (line != null) {
						val = val+line; // add line to buffer
						int idx1 = val.indexOf("$VN"); // check for message start (assume replies start with $VN)
						Matcher mat = pat.matcher(val);
						int idx2 = -1;
						if (mat.find()) { // check for message end (*XX, where X=hex digit)
							idx2 = mat.start();
						}
						// if buffer has both start and end marks, perform checksum
						if (idx2 > idx1 && val.length() >= idx2+3) {
							String num = val.substring(idx2+1,idx2+3); // checksum string (2 chars)
							int checksum = Integer.decode("0x"+num); // checksum val
							String tmp = val.substring(idx1,idx2+3); // $ + msg + checksum
							String msg = val.substring(idx1+1, idx2); // just the message 
							if (checksum == calcChecksum8(msg.getBytes())) {
								// checksum ok, return the base message (minus checksum)
								val = tmp;
								done = true;
							} else {
								// checksum fail, search for next possible message
								f.pln("SerialClientThread CHECKSUM ERR "+val);
								val = val.substring(idx1+3); //strip out $VN from buffer and try again
								sp.writeBytes(buffer);
							}
							done = true;
						}
					}
					Thread.sleep(1);
				}
				if (!done) {
					f.pln("SerialClientThread TIMEOUT");
				}
			} catch (SerialPortException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return val;
	}


	public void parseVn200Message(String line) {
		String[] list = line.split(",");
		if (list.length > 1) {
			if (list[0].equals("$VNRRG")) {
				int register = Integer.parseInt(list[1]);
				switch (register) {
				case 59: // ECEF gps (all positions in meters)
				case 58: // lla gps (north east down plane)
					double sec = Double.parseDouble(list[2]); // gps seconds into week
					int week = Integer.parseInt(list[3]); // gps week
					int gpsfix = Integer.parseInt(list[4]); //gps fix type 0=none, 1=time, 2=2d, 3=3d
					int numsats = Integer.parseInt(list[5]); // number of visible sats
					double lat = Double.parseDouble(list[6]); // deg
					double lon = Double.parseDouble(list[7]); // deg
					double alt = Double.parseDouble(list[8]); // m above WGS84
					double vx = Double.parseDouble(list[9]); // vx m/s (north)
					double vy = Double.parseDouble(list[10]); // vy m/s (east)
					double vz = Double.parseDouble(list[11]); // vz m/s (down)
					double dx = Double.parseDouble(list[12]); // accuracy x m (north)
					double dy = Double.parseDouble(list[13]); // accuracy y m (east)
					double dz = Double.parseDouble(list[14]); // accuracy z m (down)
					double dv = Double.parseDouble(list[15]); // speed accuracy m/s
					double dt = Double.parseDouble(list[16]); // time accuracy s
					break;
				default:
				}

			}
		}
	}

	public void run() {
		if (async) {

		} else {
			String val = sendCommandWithResponse("$VNRRG,63"); // get current data set
			if (val != null) {
				icky.setGPSData(val);
				f.pln("ICAROUS read gps:"+val);
			} else {
				icky.setGPSData("ERROR");
			}
		}
	}

	@Override
	public void serialEvent(SerialPortEvent serialPortEvent) {
		if(serialPortEvent.isRXCHAR()) {
			int sz = serialPortEvent.getEventValue();
			if (sz > 0) {
				try {
					lineval = sp.readString(sz);
				}
				catch (SerialPortException ex) {
					ex.printStackTrace();
				}
			}
		}
		// TODO Auto-generated method stub

	}
}


