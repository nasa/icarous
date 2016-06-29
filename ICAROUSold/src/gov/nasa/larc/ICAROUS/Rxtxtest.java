/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.ICAROUS;
//
//import gnu.io.CommPort;
//import gnu.io.CommPortIdentifier;
//import gnu.io.SerialPort;
//import gov.nasa.larcfm.Util.f;
//
//import java.io.FileDescriptor;
//import java.io.IOException;
//import java.io.InputStream;
//import java.io.OutputStream;
//
//
//public class Rxtxtest {
//
//    public Rxtxtest()
//    {
//        super();
//    }
//    
//    static void listPorts()
//    {
//        java.util.Enumeration<CommPortIdentifier> portEnum = CommPortIdentifier.getPortIdentifiers();
//        while ( portEnum.hasMoreElements() ) 
//        {
//            CommPortIdentifier portIdentifier = portEnum.nextElement();
//            System.out.println(portIdentifier.getName()  +  " - " +  getPortTypeName(portIdentifier.getPortType()) );
//        }        
//    }
//    
//    static String getPortTypeName ( int portType )
//    {
//        switch ( portType )
//        {
////            case CommPortIdentifier.PORT_I2C:
////                return "I2C";
////            case CommPortIdentifier.PORT_PARALLEL:
////                return "Parallel";
////            case CommPortIdentifier.PORT_RAW:
////                return "Raw";
////            case CommPortIdentifier.PORT_RS485:
////                return "RS485";
//            case CommPortIdentifier.PORT_SERIAL:
//                return "Serial";
//            default:
//                return "unknown type";
//        }
//    }
//
//
//    
//    void connect ( String portName ) throws Exception
//    {
//        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
//        if ( portIdentifier.isCurrentlyOwned() )
//        {
//            System.out.println("Error: Port is currently in use");
//        }
//        else
//        {
//            CommPort commPort = portIdentifier.open(this.getClass().getName(),2000);
//            
//            if ( commPort instanceof SerialPort )
//            {
//                SerialPort serialPort = (SerialPort) commPort;
//                serialPort.setSerialPortParams(57600,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
//                
//                InputStream in = serialPort.getInputStream();
//                OutputStream out = serialPort.getOutputStream();
//                
//                (new Thread(new SerialReader(in))).start();
//                (new Thread(new SerialWriter(out))).start();
//
//            }
//            else
//            {
//                System.out.println("Error: Only serial ports are handled by this example.");
//            }
//        }     
//    }
//    
//    /** */
//    public static class SerialReader implements Runnable 
//    {
//        InputStream in;
//        
//        public SerialReader ( InputStream in )
//        {
//            this.in = in;
//        }
//        
//        public void run ()
//        {
//            byte[] buffer = new byte[1024];
//            int len = -1;
//            try
//            {
//                while ( ( len = this.in.read(buffer)) > -1 )
//                {
//                    System.out.print(new String(buffer,0,len));
//                }
//            }
//            catch ( IOException e )
//            {
//                e.printStackTrace();
//            }            
//        }
//    }
//
//    /** */
//    public static class SerialWriter implements Runnable 
//    {
//        OutputStream out;
//        
//        public SerialWriter ( OutputStream out )
//        {
//            this.out = out;
//        }
//        
//        public void run ()
//        {
//            try
//            {                
//                int c = 0;
//                while ( ( c = System.in.read()) > -1 )
//                {
//                    this.out.write(c);
//                }                
//            }
//            catch ( IOException e )
//            {
//                e.printStackTrace();
//            }            
//        }
//    }
//    
//    public static void main ( String[] args )
//    {
//        try
//        {
//        	f.pln("Serial Ports:");
//        	listPorts();
//        	f.pln("Done.");
////            (new Rxtxtest()).connect("COM3");
//        }
//        catch ( Exception e )
//        {
//            // TODO Auto-generated catch block
//            e.printStackTrace();
//        }
//    }
//	
//}
