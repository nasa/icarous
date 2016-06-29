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
//import java.io.BufferedInputStream;
//import java.io.ByteArrayOutputStream;
//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileNotFoundException;
//import java.io.IOException;
//import java.io.InputStream;
//
//
///**
// * @author cquach
// *
// */
//public class BinReader {
//
//  //====================================================================================================================//
//  // DATA Section
//  //====================================================================================================================//
//  //==== Default file names 
//  //private static final String INPUT_FILE_NAME = "C:\\TEMP\\cottage.jpg";
//
//  String fileName = null;
//  InputStream inStream = null;
//
//  //====================================================================================================================//
//  // FUNCTION Section
//  //====================================================================================================================//
//
//  //=====================================================================================//
//  /**
//   * Default Constructor
//   * @param  bytes to add
//   * @return occupied bytes in buffer.
//   */
//  //=====================================================================================//
//  public BinReader() {
//    // TODO Auto-generated constructor stub
//  }
//
//  //=====================================================================================//
//  /**
//   * Read the given binary file, and return its contents as a byte array.
//   * @param  bytes to add
//   * @return occupied bytes in buffer.
//   */
//  //=====================================================================================//
//  public byte[] readFile(String fn)
//  {
//    fileName = fn;
//    File file = new File(fileName);
//    int fileSize = (int)file.length();
//    System.out.println(Thread.currentThread().getName()+"-->BinReader::readFile: "
//        +"Trying to read finary file named [" + fileName + "]  "
//        +"File size: [" + fileSize + "]  "
//        );
//
//    byte[] result = new byte[fileSize];
//    try {
//      InputStream input = null;
//      try {
//        int totalBytesRead = 0;
//        input = new BufferedInputStream(new FileInputStream(file));
//        while(totalBytesRead < result.length)
//        {
//          int bytesRemaining = result.length - totalBytesRead;
//          //input.read() returns -1, 0, or more :
//          int bytesRead = input.read(result, totalBytesRead, bytesRemaining); 
//          if (bytesRead > 0){
//            totalBytesRead = totalBytesRead + bytesRead;
//          }
//        }
//        /*
//			         the above style is a bit tricky: it places bytes into the 'result' array; 
//			         'result' is an output parameter;
//			         the while loop usually has a single iteration only.
//         */
//        System.out.println(Thread.currentThread().getName()+"-->BinReader::readFile: "
//            + "Num bytes read [" + totalBytesRead + "]  "
//            );
//      }
//      finally {
//        System.out.println(Thread.currentThread().getName()+"-->BinReader::readFile: "
//            + "Closing input stream."
//            );
//        input.close();
//      }
//    }
//
//    catch(FileNotFoundException ex)   {
//      System.err.println(Thread.currentThread().getName()+"-->BinWritter::writeFile: "
//          +"Encountered FileNotFoundException writing to [" + fileName + "]  "
//          );
//      System.err.println(ex);
//
//    }
//
//    catch(IOException ex)		{
//      System.err.println(Thread.currentThread().getName()+"-->BinWritter::writeFile: "
//          +"Encountered IOException writing to [" + fileName + "]  "
//          );
//      System.err.println(ex);
//    }
//
//    return result;
//  }	// end read() function 
//
//
//  //=====================================================================================//
//  /**
//   * Read the given binary file, and return its contents as a byte array.
//   * @param  bytes to write
//   * @param  Output filename
//   * 
//   */
//  //=====================================================================================//
//  public byte[] readAlternateImpl(String fn)
//  {
//    fileName = fn;
//    File file = new File(fileName);
//    int fileSize = (int)file.length();
//    System.out.println(Thread.currentThread().getName()+"-->BinReader::readAlternateImpl: "
//        +"Trying to read finary file named [" + fileName + "]  "
//        +"File size: [" + fileSize + "]  "
//        );
//
//    byte[] result = null;
//
//    try {
//      InputStream input =  new BufferedInputStream(new FileInputStream(file));
//      result = readAndClose(input);
//    }
//    catch(FileNotFoundException ex)   {
//      System.err.println(Thread.currentThread().getName()+"-->BinWritter::readAlternateImpl: "
//          +"Encountered FileNotFoundException writing to [" + fileName + "]  "
//          );
//      System.err.println(ex);
//
//    }
//    return result;
//  }
//
//  //=====================================================================================//
//  /**
//   * Read an input stream, and return it as a byte array.  Closes aInput after it's read.  
//   * The source of bytes is an input stream instead of a file. 
//   * @param  aInput  Input stream
//   * 
//   */
//  //=====================================================================================//
//  public byte[] readAndClose(InputStream aInput)
//  {
//    //carries the data from input to output :    
//    byte[] bucket = new byte[32*1024]; 
//    ByteArrayOutputStream result = null; 
//    try  {
//      try {
//        //Use buffering? No. Buffering avoids costly access to disk or network;
//        //buffering to an in-memory stream makes no sense.
//        result = new ByteArrayOutputStream(bucket.length);
//        int bytesRead = 0;
//        while(bytesRead != -1)
//        {
//          //aInput.read() returns -1, 0, or more :
//          bytesRead = aInput.read(bucket);
//          if  (bytesRead > 0) 
//          {
//            result.write(bucket, 0, bytesRead);
//          }
//        }
//      }  finally {
//        aInput.close();
//        //result.close(); this is a no-operation for ByteArrayOutputStream
//      }
//    }  	
//
//    catch(IOException ex)		{
//      System.err.println(Thread.currentThread().getName()+"-->BinWritter::writeFile: "
//          +"Encountered IOException writing to [" + fileName + "]  "
//          );
//      System.err.println(ex);
//    }
//
//
//    return result.toByteArray();
//
//  }		// end readAndClose() function	
//
//  //====================================================================================================================//
//  // TEST ENTRY POINT
//  //====================================================================================================================//
//
//  /**
//   * @param args
//   */
//  public static void main(String[] args) {
//    // TODO Auto-generated method stub
//
//  }
//}	// END BinReader Class
//
