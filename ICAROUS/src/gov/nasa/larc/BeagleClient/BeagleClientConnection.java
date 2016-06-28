/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.BeagleClient;
//
////import gov.nasa.larc.serial.*;
//import java.io.IOException;
//import java.io.PrintWriter;
//import java.net.InetAddress;
//import java.net.Socket;
//import java.net.UnknownHostException;
//
//public class BeagleClientConnection  {
//
//	static String hostIP = "localhost";
//
//	public BeagleClientConnection (){
////	public static void main(String[] args) {
//
////		STR.init(args);
//
//		Socket socket = null;
//		PrintWriter out = null;
//		// int count = 0;
//
//		try {
//			socket = new Socket(hostIP, 4444);
//			out = new PrintWriter(socket.getOutputStream(), true);
//		} catch (UnknownHostException e) {
//			System.err.println("Can't find the specified host.");
//			System.exit(1);
//		} catch (IOException e) {
//			System.err.println("Couldn't get I/O for the connection to host.");
//			System.exit(1);
//		}
//
//		
//		try {
//			while (true) {
//				String TimeStamp = new java.util.Date().toString();
//				String outputString = "Message from client "
//						+ InetAddress.getLocalHost() + " : " + " at "
//						+ TimeStamp;
//				out.println(outputString);
//				try {
//					Thread.sleep(5);
//				} catch (InterruptedException e) {
//					System.out.println("Exception " + e);
//				}
//				// count++;
//			}
//		} catch (UnknownHostException uhe) {
//			// no op
//		}
//	}
//}
