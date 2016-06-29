/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/**
 * 
 */
package gov.nasa.larc.AP;

//import gov.nasa.larc.ICAROUS.IcarousCodec;
import gov.nasa.larc.ICAROUS.Messenger;
//import gov.nasa.larc.serial.SerialPortManager;
import gov.nasa.larc.serial.SerialReceiver;

/**
 * @author cquach
 *
 */
public class APListener implements SerialReceiver 
{
  String portName = null;
  public boolean portOpenStat;
  private long sendTime;
  private byte[] msgToSend;

  //====================================================================================================================//
  // FUNCTION Section
  //====================================================================================================================//

  /**
   * Constructor to open serial port.
   * @param pn   Serial port logical name.
   */
  public APListener (String pn)
  {

    if (pn != null) {
      portName = pn;
      //Thread.currentThread().setPriority(4);
      //Thread.currentThread().setName("Watch");
      Messenger.msg("APListener:"
          + "Opening serial port & set listener at ["+ portName +"] "
          + "with thread priority ["+Thread.currentThread().getPriority()+"]..."
          );        
//      portOpenStat = SerialPortManager.addSerialReceiver(portName,this); // opens port and add listener
      //SerialPortManager.
      if (portOpenStat) {
        Messenger.msg("APListener:SUCCESS Opening serial port & set listener at ["+ portName +"].");                       
      } else {
        Messenger.err("APListener:FAILED Opening serial port & set listener at ["+ portName +"].");                                        
      }
    }
  }

  //=====================================================================================//
  // Override interface method.
  //=====================================================================================//
  public void serialDataReceived(byte[] data)
  {

    //		System.out.println (Thread.currentThread().getName()
    //							+"--> AP::serialDataReceived: "
    //							+" New ["+data+"] bytes from AP serial port["
    //							+ByteQueue.bytesToInt(data, 0, data.length)
    //							+"]"
    //							);
    try {
      APPacket.enque(data);

    } catch (Exception e)	{
      //STR.err("AP:serialDataReceived: Exception during parsing!");
      System.err.println("AP:serialDataReceived: Exception during parsing!");
    }
  }	// End SerialDataReceived() function

  /**
   * Write a byte array into a serial port.
   * @param msg The byte array to output
   * @return
   */
  public boolean send(byte[] msg) {
    sendTime = System.currentTimeMillis();
    msgToSend = msg;
    //System.out.println(Thread.currentThread().getName()+"--> SerialPort::send: Sending ["+ WatchCodec.bytesToInt(msg)+"]");
//    return SerialPortManager.write(portName, msg);  // write function returns boolean status
    return false;
  }	

  /**
   * Write a byte array into a serial port.
   * @param msg The byte array to output
   * @return
   */
  public boolean resendLastMsg() {
    sendTime = System.currentTimeMillis();
    //msgToSend = msg;
//    System.out.println(Thread.currentThread().getName()+"--> SerialPort::send: Sending ["+ IcarousCodec.bytesToInt(msgToSend)+"]");
//    return SerialPortManager.write(portName, msgToSend);  // write function returns boolean status
    return false;
  }	

  /**
   * Return the last time we sent a message.
   * @param msg The byte array to output
   * @return
   */
  public long lastSendTime() {
    return sendTime;
  }

  /**
   * Entry point to test AP communication and data storage.
   * @param pn   Serial port logical name.
   */
  public static void main(String[] args) {
    // TODO Auto-generated method stub

  }

}
