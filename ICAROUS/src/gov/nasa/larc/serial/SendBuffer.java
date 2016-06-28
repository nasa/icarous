/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
///**
// * 
// */
//package gov.nasa.larc.serial;
//
//import gnu.io.*;
//import gov.nasa.larc.ICAROUS.Messenger;
//
//import java.io.*;
//import java.util.concurrent.CountDownLatch;
//import java.util.concurrent.TimeUnit;
//
//public class SendBuffer extends Thread
//{
//
//  //====================================================================================================================//
//  // DATA Section
//  //====================================================================================================================//
//
//  public ByteQueue queue = new ByteQueue();
//  private String portname;
//  private OutputStream ostream;
//  private int len = 71;
//  private long sleepTics = 50;	
//  public volatile CountDownLatch running = new CountDownLatch(1);
//
//  //====================================================================================================================//
//  // METHOD Section
//  //====================================================================================================================//
//
//  /**
//   * Constructor to open serial port.
//   * @param port   Serial port to get output stream from
//   */
//  public SendBuffer(SerialPort serialPort) 
//  {
//    portname = serialPort.getName();
//    try
//    {
//      ostream = serialPort.getOutputStream();
//    }
//    catch (IOException e)
//    {
//      Messenger.err("SendBuffer:I/O error getting output stream from port "+serialPort.getName());
//    } 
//  }
//
//  /** 
//   *  Write a value to a ByteBuffer for sending 
//   */
//  public synchronized void send (byte[] output)
//  {
//
//    //System.err.println("serial-->SendBuffer::send(): bytes written to buffer for sending to output stream  "+Codec.bytesToInt(output));
//    queue.enque(output);//write all values into one manageable ByteBuffer
//
//  }
//
//  /** 
//   *  Run Thread. Constantly dequeue data packets of length 72 from ByteBuffer and write to the output stream
//   */
//  public void run() 
//  {
//
//    Thread.currentThread().setName("SendBuffer["+portname+"]");
//    Messenger.msg("SendBuffer::run:Start Thread");	
//
//    while(running.getCount() > 0) 
//    {
//      if(queue.fill > len+1)//if we have enough data in our queue 
//      {
//
//        try
//        {
//          //System.err.println("serial::SendBuffer:run(): Head: "+ queue.head +" Tail: "+ queue.tail);
//          byte[] data = queue.deque(0, len);
//          synchronized(TxRx.lockObject)
//          {
//            ostream.write(data);//write 72 bytes at a time to the output stream
//          }
//
//
//          //System.err.println("serial-->SendBuffer::run(): bytes  deque from buffer sent to outputstream:  "+Codec.bytesToInt(data));
//
//        }
//        catch(IOException e)
//        {
//          Messenger.err("SendBuffer::run:Unable to write to output stream");
//        }
//      }
//      try { 
//        running.await(sleepTics,TimeUnit.MILLISECONDS);
//      } catch (InterruptedException e) {
//        running.countDown();
//      }
//    }
//    Messenger.msg("SendBuffer::run:End Thread");  
//  }
//}
//
//
