/**
 * ICAROUS Main Class
 * Contact: Cesar Munoz, George Hagen
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.sql.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Timer;

import gov.nasa.larc.AP.AP;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
//import gov.nasa.larc.serial.SerialPortManager;
import gov.nasa.larcfm.Util.f;

public class Icarous implements Runnable {

  public final static String VERSION = "0.0";
  public final static DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HHmmss_");
  public final static DateFormat timeFormat = new SimpleDateFormat("HH:mm:ss ");
  public final static Date date = new Date(System.currentTimeMillis());
  public final static String dateTimeStr = dateFormat.format(date);
  public final static String logFileNamePrefix = dateTimeStr;

  private String id_; // Identifier
  protected Position s = Position.ZERO_XYZ; // position only, the "t" field is not used
  protected Velocity v = Velocity.ZERO; // velocity
  public Plan trajectory = new Plan();               // Current trajectory of aircraft derived from flight plan
  public NavPoint chasePoint = NavPoint.ZERO_XYZ;
  protected ErrorLog error = new ErrorLog("Aircraft");

  
  //----  Autopilot ----//
  private AP ap_; 

//  //----  Serial communication ----//
//  private IcarousSerialListener serial_comm_;
//
//  //----  Socket I/O communication --//
//  private IcarousSocketListener socket_comm_;

  private String gpsData = ""; // used to store current gps data for pass through (synchronized)
  
  //IcarousCodec codec = new IcarousCodec();
  //public static String GNDport = "COM28";

  public Icarous(String callsign, String ap_port, String serial_port, int socket_port) {
    id_ = callsign; 
    ap_ = new AP(ap_port);
//    serial_comm_ = new IcarousSerialListener(serial_port);
//    socket_comm_ = new IcarousSocketListener(socket_port);
  }

  /**
   * Returns the simulation time of the current instance.
   * Simulation time is the time according to the simulation.  
   * Plans are with respect to this time and simulated aircraft advance based on the simulation time.
   * Simulation times are never negative.
   * @return time (in sec)
   */
  public static double getSimTime() {
    // [CAM] For now it returns computer time [Used in AP.java]
    return System.currentTimeMillis();
  }

  /**
   * Returns the current heartbeat time for this simulation.
   * The heartbeat time is used when the simulation is not actively running to monitor outside events and update displays.
   * This advances even if the simulation does not.  It should, effectively, be unique for each call.
   * @return time (in milliseconds)
   */
  public static long getHBTime() {
    // [CAM] For now it returns computer time [Used in AP.java]
    return System.currentTimeMillis();
  }

  public static int getTypeClass() {
    // [CAM] For now returns 0 [Used in IcarousCodec.java]
    return 0;
  }

  public void run() {
    Thread.currentThread().setName("Icarous");
    Messenger.msg("Icarous::run:Start thread");
    Messenger.msg("Icarous::run:End thread");
  }

  static void printHelpMsg() {
    System.out.println("ICAROUS V-"+VERSION);
    System.out.println("Usage:");
    System.out.println("  icarous [options]");
    System.out.println("  options are:");
    System.out.println("  --help\n\tPrint this message");
    System.out.println("  --config <file>\n\tRead configuration file");
    System.out.println("  --verbose\n\tPrint verbose information");
    System.out.println("  --debug\n\tPrint debug information");
    System.out.println("  --callsign <id>\n\tSet callsign to <id>");
    System.out.println("  --ap <port>\n\tSet autopilot serial port");
    System.out.println("  --sg <port>\n\tSet SafeGuard serial port");
    System.out.println("  --io <port>\n\tSet internal I/O socket port");
    System.exit(0);
  }

  public String getGPSData() {
	  synchronized(gpsData) {
		  return gpsData;
	  }
  }

  public void setGPSData(String data) {
	  synchronized(gpsData) {
		  gpsData = data;
	  }
  }
  
  //*****************************************
  // This is the main entry point for ICAROUS
  //*****************************************
  public static void main(String[] args) {
    int a;
    String config = "";
    String options = "";
    String callsign = "IOUS";
    String ap_port = null; 
    String serial_port = null; 
    int    socket_port = 0;
    int    gps_socket_port = 5555;
    int    hb_socket_port = 4444;
    String gps_serial_port = "COM3"; 
    
    for (a=0;a < args.length && args[a].startsWith("-"); ++a) {
      options += args[a]+" ";
      if (args[a].startsWith("--h") || args[a].startsWith("-h")) {
        printHelpMsg();
      } else if (args[a].startsWith("--conf") || args[a].startsWith("-conf")) {
        config = args[++a];
        options += args[a]+" ";
      } else if (args[a].startsWith("--verb") || args[a].startsWith("-verb")) {
        Messenger.verbose = true;
      } else if (args[a].startsWith("--deb") || args[a].startsWith("-deb")) {
        Messenger.debug = true;
      } else if (args[a].startsWith("--call") || args[a].startsWith("-call")) {
        callsign = args[++a];
        options += args[a]+" ";
      } else if (args[a].equals("--ap") || args[a].equals("-ap")) {
        ap_port = args[++a];
        options += args[a]+" ";
      } else if (args[a].equals("--serial") || args[a].equals("-serial")) {
          serial_port = args[++a];
          options += args[a]+" ";
      } else if (args[a].equals("--socket") || args[a].equals("-socket")) {
          socket_port = Integer.parseInt(args[++a]);
          options += args[a]+" ";
      } else if (args[a].equals("--gps_socket") || args[a].equals("-gps_socket")) {
          gps_socket_port = Integer.parseInt(args[++a]);
          options += args[a]+" ";
      } else if (args[a].equals("--hb_socket") || args[a].equals("-hb_socket")) {
          hb_socket_port = Integer.parseInt(args[++a]);
          options += args[a]+" ";
      } else if (args[a].equals("--gps_serial") || args[a].equals("-gps_serial")) {
          gps_serial_port = args[++a];
          options += args[a]+" ";
      } else if (args[a].startsWith("-")) {
        System.out.println("Invalid option: "+args[a]);
        System.exit(0);
      }
    }

    System.out.println("**");
    System.out.println("ICAROUS V-"+VERSION);
    System.out.println("Options: "+options);
    System.out.println("Callsign: "+callsign);
    System.out.println("AP Serial Port: "+ap_port);
    System.out.println("I/O Serial Port: "+serial_port);
    System.out.println("I/O Socket Port: "+socket_port);
    System.out.println("GPS Serial Port: "+gps_serial_port);
    System.out.println("GPS Socket Port: "+gps_socket_port);
    System.out.println("HB Socket Port: "+hb_socket_port);
    System.out.println("**");

  //  SerialPortManager.init();   

    Icarous icarous = new Icarous(callsign,ap_port,serial_port,socket_port);

//    if (icarous.ap_.serial.portOpenStat && icarous.serial_comm_.portOpenStat && icarous.socket_comm_.portOpenStat) { 
//      Thread thread = new Thread(icarous);
//      // start execution only if all ports are available.
//      thread.start();
//    } else {
//      if (!icarous.ap_.serial.portOpenStat) {
//        Messenger.err("Icarous::main: AP Serial Port ["+ ap_port +"] failed to open. Check used by another process?");
//      }
//      if (!icarous.serial_comm_.portOpenStat) {
//        Messenger.err("Icarous::main: I/O Serial Port ["+ serial_port +"] failed to open. Check used by another process?");
//      }
//      if (!icarous.socket_comm_.portOpenStat) {
//        Messenger.err("Icarous::main: I/O Socket Port ["+ socket_port +"] failed to open. Check used by another process?");
//      }
//      Messenger.err("Icarous::main:TERMINATED");
//    }

    IcarousComms.startCommPassThroughThread(gps_socket_port, icarous);
    IcarousComms.startCommHBThread(hb_socket_port);
    IcarousComms.startSerialClientThread(gps_serial_port, icarous, 1000);
    
  
  }
}	// End Icarous Class
