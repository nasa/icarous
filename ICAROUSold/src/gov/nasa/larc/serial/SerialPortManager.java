/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.serial;
//
//import java.util.HashMap;
//import java.util.Enumeration;
//import java.util.LinkedList;
//
//import gnu.io.*;
//import gov.nasa.larc.ICAROUS.Messenger;
//
//public class SerialPortManager
//{
//  // Map of port name -> port identifier; primary method of accessing serial
//  // ports
//  private static HashMap<String, CommPortIdentifier> portIds = new HashMap<String, CommPortIdentifier>();
//  private static HashMap<String, TxRx> openIds = new HashMap<String, TxRx>();
//  private static HashMap<TxRx, LinkedList<TxRx>> tunnelMap = new HashMap<TxRx, LinkedList<TxRx>>();
//  // private static HashMap<String, SerialRx> rxIds = new HashMap<String,
//  // SerialRx>();
//  // private static HashMap<String, SerialTx> txIds = new HashMap<String,
//  // SerialTx>();
//  // private static HashMap<SerialRx, LinkedList<SerialTx>> tunnels = new
//  // HashMap<SerialRx, LinkedList<SerialTx>>();
//
//  private static int bitrate = 57600;
//  public static int time_out = 500; //[CAM]
//
//
//  /**
//   * Populate port list at program start
//   */
//  public static void init()
//  {
//    Messenger.msg("SerialPortManager::init:Searching for ports...");
//
//    // retrieve list of port identifiers to iterate through
//    @SuppressWarnings("unchecked")
//    Enumeration<CommPortIdentifier> portEnum = CommPortIdentifier.getPortIdentifiers();
//
//    int numPorts = 0;
//
//    // loop through available ports
//    while (portEnum.hasMoreElements())
//    {
//      // get port identifier, name, type; skip if not serial
//      CommPortIdentifier portIdentifier = portEnum.nextElement();
//
//      String portName = portIdentifier.getName();
//      int portType = portIdentifier.getPortType();
//      if (portType != CommPortIdentifier.PORT_SERIAL)
//      {
//        Messenger.msg("SerialPortManager::init:Found non-serial port " + portName);
//        continue;
//      }
//      // if serial: count, store, name
//      numPorts++;
//      portIds.put(portName, portIdentifier);
//      Messenger.msg("SerialPortManager::init:Registered serial port " + portName);
//    }
//    // exit if no ports
//    if (numPorts == 0)
//    {
//      Messenger.err("SerialPortManager::init:No ports found! Closing SerialTR.");
//      System.exit(1);
//    } else
//    {
//      Messenger.msg("SerialPortManager::init:"
//          + numPorts 
//          + " serial ports available."
//          );
//    }
//
//    // add closing hook for closing ports
//    Thread portCloser = new Thread()
//    {
//      @Override
//      public void run()
//      {
//        closeAllPorts();
//      }
//    };
//    Runtime.getRuntime().addShutdownHook(portCloser);
//  }
//
//
//  public static boolean openPort(String portName)
//  {
//    // try and make txrx object on port
//    TxRx txrx = TxRx.openPort(portName);
//    // abort if it fails
//    if (txrx == null)
//    {
//      Messenger.err("SerialPortManager::openPort:Could not open port " + portName);
//      return false;
//    }
//    // otherwise store it and return true
//    openIds.put(portName, txrx);
//    Messenger.msg("SerialPortManager::openPort:Successfully opened port " + portName);
//    return true;
//  }
//
//  public static boolean write(String portName, byte[] msg)
//  {
//    try
//    {
//      openIds.get(portName).write(msg);
//      return true;
//    } catch (NullPointerException e)
//    {
//      Messenger.err("SerialPortManager::write:Cannot write to port " + portName
//          + " because it does not exist or hasn't been opened!");
//      return false;
//    }
//  }
//
//  public static boolean addSerialReceiver(String portName, SerialReceiver rx)
//  {
//
//    boolean portOpen = hasOpenPort(portName) || openPort(portName);
//    // if hasPortOpen returns false, call openPort which will attempt
//    // to open the port and return true if it was successful
//
//    if (portOpen)
//    {
//      return openIds.get(portName).addReceiver(rx);
//    } else
//    {
//      Messenger.err("SerialPortManager::addSerialReceiver:Cannot add serial receiver to port " + portName
//          + " because the port doesn't exist or couldn't be opened!");
//      return false;
//    }
//  }
//
//  /**
//   * Retrieve port by name
//   * 
//   * @param portName
//   * @return
//   */
//  public static CommPortIdentifier getPort(String portName)
//  {
//    return portIds.get(portName);
//  }
//
//  /**
//   * Check if specified port exists
//   * 
//   * @param portName
//   * @return
//   */
//  public static boolean hasPort(String portName)
//  {
//    return portIds.containsKey(portName);
//  }
//
//  /**
//   * Check if specified port is available for read/write
//   * 
//   * @param portName
//   * @return
//   */
//  public static boolean hasOpenPort(String portName)
//  {
//    return openIds.containsKey(portName);
//  }
//
//  
//  /**
//   * Add tunnel from port1 to port2
//   * 
//   * @param port1
//   * @param port2
//   * @return true if successful
//   */
//  public static boolean addTunnel(String port1, String port2)
//  {
//    TxRx entry = openIds.get(port1);
//    TxRx exit = openIds.get(port2);
//    if (entry == null)
//    {
//      Messenger.err("SerialPortManager::addTunnel:Could not create tunnel from " + port1 + " to " + port2
//          + ";");
//      Messenger.err("SerialPortManager::addTunnel:Port " + port1 + " not found.");
//      return false;
//    }
//    if (exit == null)
//    {
//      Messenger.err("SerialPortManager::addTunnel:Could not create tunnel from " + port1 + " to " + port2
//          + ";");
//      Messenger.err("SerialPortManager::addTunnel:Port " + port2 + " not found.");
//      return false;
//    }
//
//    if (!tunnelMap.containsKey(entry))
//    {
//      LinkedList<TxRx> exitList = new LinkedList<TxRx>();
//      exitList.add(exit);
//      tunnelMap.put(entry, exitList);
//    } else
//    {
//      tunnelMap.get(entry).add(exit);
//    }
//
//    Messenger.msg("SerialPortManager::addTunnel:Successfully created tunnel from " + port1 + " to " + port2);
//    return true;
//  }
//
//  /**
//   * Returns whether a tunnel exists from specified port
//   * 
//   * @param entry
//   * @return
//   */
//  public static boolean hasTunnelEntry(TxRx rx)
//  {
//    return tunnelMap.containsKey(rx);
//  }
//
//  /**
//   * Get all port exit writers in a tunnel group
//   * 
//   * @param entry
//   * @return
//   */
//  public static LinkedList<TxRx> getTunnelExits(TxRx rx)
//  {
//    return tunnelMap.get(rx);
//  }
//
//  /**
//   * Close all open ports
//   */
//  public static void closeAllPorts()
//  {
//    if (openIds.keySet().size() == 0)
//      return;
//    Messenger.msg("SerialPortManager::closeAllPorts: Closing all ports...");
//    for (String key : openIds.keySet())
//    {
//      Messenger.msg("SerialPortManager::closeAllPorts: Closing port "+key);
//      openIds.get(key).closePort();
//    }
//    Messenger.msg("SerialPortManager::closeAllPorts: All ports successfully closed.");
//  }
//
//
//  public static int getBitrate()
//  {
//    return bitrate;
//  }
//
//
//  public static void setBitrate(int bitratex)
//  {
//    bitrate = bitratex;
//  }
//
//}
