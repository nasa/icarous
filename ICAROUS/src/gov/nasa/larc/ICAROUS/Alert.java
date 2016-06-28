/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
///**
// * 
// */
//package gov.nasa.larc.ICAROUS;
//
///**
// * @author cquach
// *
// */
//public class Alert {
//
////=====================================================================================//
//// DATA Section
////=====================================================================================//
//	static public enum AlertEnum { None, AC_Traffic, AC_BHM, AC_Com, AC_SW, AC_MotorTemp, AC_MotorCurrent, AC_GeofenceViolation, AC_NoGPS};
//	
//	/*----*/
//	public AlertEnum aType = AlertEnum.None;
//	public String typeName = null;
//	public byte aByte = (byte)0x00;
//	public String text = null;
//	public int criticalityLevel = 0;
//	public double reactionTimeLimit = (double) 0.0;
//	public double timeRaised = (double) 0.0;
//	public double timeResolved = (double) 0.0;
//	
////=====================================================================================//
//// METHOD Section
////=====================================================================================//
//	
//	//=====================================================================================//
//	/**
//	 * Default Constructor for testing
//	 * @param 
//	 * @return 
//	 */
//	//=====================================================================================//
//	public Alert (AlertEnum a, String aText) 
//	{
//		aType = a;
//		text = aText;
//		switch (aType)
//		{
//		case None:  			
//			aByte =  (byte)0x00;
//			typeName = "None";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_Traffic:		
//			aByte =  (byte)0x01;
//			typeName = "None";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_BHM:			
//			aByte =  (byte)0x02;
//			typeName = "Traffic Conflict";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_Com: 			
//			aByte =  (byte)0x03;
//			typeName = "Communication Error";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_SW:				
//			aByte =  (byte)0x04;
//			typeName = "Software";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_MotorTemp: 		
//			aByte =  (byte)0x05;
//			typeName = "Motor Temperature above threshold";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_MotorCurrent: 		
//			aByte =  (byte)0x06;
//			typeName = "Motor Current above threshold";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_GeofenceViolation: 	
//			aByte =  (byte)0x07;
//			typeName = "Geofence violation";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		case AC_NoGPS: 			
//			aByte =  (byte)0x08;
//			typeName = "No GPS";
//			criticalityLevel = 0;
//			reactionTimeLimit = (double) 0.0;
//			break;
//		}
//	}
//		
//	//=====================================================================================//
//	/**
//	 * Returns the enum alert type byte for sending over serial port.
//	 * static public enum AlertEnum { AC_None, AC_Traffic, AC_BHM, AC_Com, AC_SW, AC_MotorTemp, AC_MotorCurrent, AC_GeofenceViolation, AC_NoGPS};
//	 * 
//	 * @param 
//	 * @return 
//	 */
//	//=====================================================================================//
//	public byte getByte() 
//	{
//		return aByte;
//	}
//		
//	public static byte toByte(AlertEnum ae) 
//	{
//		Alert a = new Alert(ae, null);  // this is clumsy but keeps the encoding in one place. 
//		return a.getByte();
//	}
//		
//	//=====================================================================================//
//	/**
//	 * Returns the enum alert type byte for sending over serial port.
//	 * static public enum AlertEnum { AC_None, AC_Traffic, AC_BHM, AC_Com, AC_SW, AC_MotorTemp, AC_MotorCurrent, AC_GeofenceViolation, AC_NoGPS};
//	 * 
//	 * @param 
//	 * @return 
//	 */
//	//=====================================================================================//
//	public String toString() 
//	{
//		return typeName+" "+text;
//	}
//	
//	
//	//=====================================================================================//
//		/**
//		 * 
//		 * @param resolutionType
//		 * @return String representation of the type of resolution being performed
//		 */
//		//=====================================================================================//
//	public static String resolutionToText (byte a)
//	{
//				
//		String o = "";
//		switch (a)
//		{
//		case 0x00:   o = "Heading";    		break;
//		case 0x01:   o = "Speed";    	break;
//		case 0x02:   o = "Altitude";    	break;
//		case 0x03:	 o = "No Resolution!";	break;
//	
//		
//		}
//		return o;
//	}
//	public static String toText (byte a)
//	{
//		//{ None, AC_Traffic, AC_BHM, AC_Com, AC_SW, AC_MotorTemp, AC_MotorCurrent, AC_GeofenceViolation, AC_NoGPS};
//		
//		
//		String o = "NONE";
//		switch (a)
//		{
//		case 0x00:   o = "None";    		break;
//		case 0x01:   o = "AC_Traffic";    	break;
//		case 0x02:   o = "AC_BHM";    	break;
//		case 0x03:   o = "AC_Com";    	break;
//		case 0x04:   o = "AC_SW";    	break;
//		case 0x05:   o = "AC_MotorTemp";    break;
//		case 0x06:   o = "AC_MotorCurrent";    		break;
//		case 0x07:   o = "AC_GeofenceViolation";    break;
//		case 0x08:   o = "AC_NoGPS";    	break;
//		
//		}
//		return o;
//	}
//	//=====================================================================================//
//	/**
//	 * @param args
//	 */
//	//=====================================================================================//
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//	}
//
//}
