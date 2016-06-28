/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.serial;
//
//import gnu.io.*;
//import gov.nasa.larc.AP.APPacket;
////import gov.nasa.larc.ICAROUS.Codec;
//
//import gov.nasa.larc.ICAROUS.Messenger;
//
//import java.io.*;
//import java.util.Arrays;
//import java.util.HashSet;
//import java.util.TooManyListenersException;
//
//
//public class TxRx implements SerialPortEventListener
//{
//  private String portName;
//  private CommPortIdentifier portId;
//  private SerialPort port;
//  private InputStream istream;
//  private OutputStream ostream;
//  private SendBuffer buffer;
//  private HashSet<SerialReceiver> receivers;
//
//  public static Boolean lockObject = new Boolean(true); 
//
//  /**
//   * Create an instance of a TxRx on specified port
//   * @param name
//   * @return
//   */
//  public static TxRx openPort(String name)
//  {
//
//    // make sure specified port exists
//    if (!SerialPortManager.hasPort(name))
//      return null;
//    // if it does, make a new reader
//    TxRx serial = new TxRx(name);
//    boolean success = serial.init();
//    // return the reader if that works, otherwise return null
//    if (!success)
//      serial = null;
//    return serial;
//  }
//
//  /**
//   * Constructor, private because we use the create() method externally
//   * 
//   * @param name
//   * @param id
//   */
//  private TxRx(String name) 
//  {
//    portName = name;
//    portId = SerialPortManager.getPort(name);
//    receivers = new HashSet<SerialReceiver>();
//
//  }
//
//  /**
//   * Initialize method to open port, only called internally
//   * 
//   * @return
//   */
//  private boolean init()
//  {
//    // first of all open up the dern port
//    try
//    {
//      port = (SerialPort) portId.open(portName,SerialPortManager.time_out);
//      istream = port.getInputStream();
//      buffer = new SendBuffer(port);
//      buffer.start();
//      ostream = port.getOutputStream();
//      port.addEventListener(this);
//      port.notifyOnDataAvailable(true);
//
//      //			if(portName.equals("COM22"))
//      //			{
//      //				System.err.println("Setting Baud Rate for AP Port to 57600");
//      //				port.setSerialPortParams(57600, SerialPort.DATABITS_8,
//      //				        SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
//      //			}
//      //			else
//      //			{
//      //						
//      port.setSerialPortParams(SerialPortManager.getBitrate(), SerialPort.DATABITS_8,
//          SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
//      //			}
//      return true;
//    } catch (PortInUseException e)
//    {
//      Messenger.err("TxRx::init:Port " + portName + " in use, unable to read");
//      return false;
//    } catch (IOException e)
//    {
//      Messenger.err("TxRx::init:I/O error getting input stream from port " + portName);
//      return false;
//    } catch (TooManyListenersException e)
//    {
//      Messenger.err("TxRx::init:Too many listeners on port " + portName
//          + " failed to add new listener");
//      return false;
//    } catch (UnsupportedCommOperationException e)
//    {
//      Messenger.err("TxRx::init:Error configuring read port " + portName);
//      return false;
//    }
//
//  }
//
//  /**
//   * Method to actually write bytes to the port
//   * @param bytes
//   * @return
//   */
//  public boolean write(byte[] bytes) {
//
//    try{
//      //			System.err.println("Edge-->TxRx::write: bytes written to outputstream  "+Codec.bytesToInt(bytes));
//
//      //			if(bytes.length>6 && packet.isValidHeader(bytes))
//      //			{
//      if(bytes[0] == 85 && APPacket.enoughForHeader(bytes) && APPacket.isValidHeader(bytes)){
//        //System.err.println("Edge-->TxRx::write: bytes written directly to outputstream MAV "+Codec.bytesToInt(bytes));
//        synchronized(lockObject)
//        {
//          ostream.write(bytes);
//        }
//
//      }
//      else
//      {
//        //System.err.println("Edge-->TxRx::write: bytes written to SendBuffer "+Codec.bytesToInt(bytes));
//
//        buffer.send(bytes);
//      }
//
//
//
//
//      //			available.acquire();
//      //			ostream.write(bytes);
//      //			available.release();
//
//      //			System.err.println("Edge-->TxRx::write: bytes written to outputstream  "+Codec.bytesToInt(bytes));
//      //
//      //			ostream.write(bytes);
//
//
//
//      return true;
//    }
//    catch (IOException e)
//    {
//      Messenger.err("TxRx::write:I/O Exception " + portName);
//      return false;
//    }
//
//  }
//
//  /**
//   * What to do when we get data from port
//   */
//  @Override
//  public void serialEvent(SerialPortEvent event)
//  {
//    switch (event.getEventType())
//    {
//
//    case SerialPortEvent.BI:
//
//    case SerialPortEvent.OE:
//
//    case SerialPortEvent.FE:
//
//    case SerialPortEvent.PE:
//
//    case SerialPortEvent.CD:
//
//    case SerialPortEvent.CTS:
//
//    case SerialPortEvent.DSR:
//
//    case SerialPortEvent.RI:
//
//    case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
//      break;
//
//    case SerialPortEvent.DATA_AVAILABLE:
//
//      // TODO debug crap
//      // System.out.println("TXRX RECEIVING SERIAL DATA");
//      // System.out.println("# of SerialReceivers for" + portName + ": " + receivers.size());
//
//      try
//      {
//        // we got some data
//        while ( istream.available() > 0 ) {
//          // store data in buffer and get length
//          byte[] buffer = new byte[288];
//          int msglen = istream.read(buffer);
//          // get message at proper length
//          byte[] msg = Arrays.copyOfRange(buffer, 0, msglen);
//          //System.err.println("Edge-->TxRx::serialEvent: message received in buffer: "+ Codec.bytesToInt(msg));
//          if ( SerialPortManager.hasTunnelEntry(this) )
//          {
//            // pass through to tunnels
//            for ( TxRx tx : SerialPortManager.getTunnelExits(this) )
//            {
//              tx.write(msg);
//            }
//          }
//          // pass through to receiver objects
//          for ( SerialReceiver rx : receivers )
//          {
//            //if(portName == "COM28")System.err.println("Serial-->TxRx::serialEvent: msg sent to serialDataReceived : "+Codec.bytesToInt(msg)+" from the thread "+Thread.currentThread().getName());
//            rx.serialDataReceived(msg);
//          }
//          String test = "";
//          for ( int i = 0; i < msglen; i ++ )
//          {
//            byte b = buffer[i];
//            //System.out.print(b + " ");
//            test += String.valueOf((char)b);
//          }
//          Messenger.dbg("TxRx::serialEvent:"+test);
//        }
//
//
//      } catch (Exception e)
//      {
//
//        Messenger.err("TxRx::serialEvent:Error while reading data from port " + portName
//            + ": " + e.getClass());
//
//        for (Object o : e.getStackTrace())
//          Messenger.err("TxRx::serialEvent:"+o.toString());
//
//      }
//
//      break;
//    }
//
//  }
//
//  public boolean addReceiver(SerialReceiver rx)
//  {
//    return receivers.add(rx);
//  }
//
//  public void closePort()
//  {
//    buffer.running.countDown();
//    try {
//      buffer.join();
//      port.close();
//    } catch (InterruptedException e) {
//    }
//  }
//  
//}
