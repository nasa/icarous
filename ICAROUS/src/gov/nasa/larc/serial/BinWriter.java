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
//import java.io.BufferedOutputStream;
//import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
//import java.io.IOException;
//import java.io.OutputStream;
//
///**
// * @author cquach
// *
// */
//public class BinWriter {
//
////====================================================================================================================//
//// DATA Section
////====================================================================================================================//
//	//==== Default file names 
//	private static final String DEFAULT_FILE_NAME = "out.blog";
//
//	static String fileName = DEFAULT_FILE_NAME;
//	OutputStream outStream = null;
//
////====================================================================================================================//
//// FUNCTION Section
////====================================================================================================================//
//			
//	//=====================================================================================//
//	/**
//	 * Default Constructor
//	 * @param  bytes to add
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public BinWriter() {
//		this(fileName);
//	}
//
//	
//	//=====================================================================================//
//	/**
//	 * Construct BinWriter with given filename.
//	 * @param  bytes to add
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public BinWriter(String fn) {
//		open (fn);
//	}
//
//
//	//=====================================================================================//
//	/**
//	 * Open a binary file for output.  File stays persistent unti closed.  Write errors 
//	 * will trigger file close.  
//	 * @param  outStream file name
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public boolean open (String fn)
//	{
//		boolean fileOpenResult = false;
//		fileName = fn;
//		if  (outStream != null)  { 
//			System.err.println(Thread.currentThread().getName()
//					+"-->BinWriter::open: "
//					+"Call to open file did nothing because [" + fileName + "] is already opened for write. "
//				  );
//			return true;			
//		}
//		System.out.println(Thread.currentThread().getName()
//						+"-->BinWriter::open: "
//						+"Trying to open [" + fileName + "]  "
//					  );
//	
//		try {
//			
////				try {
//					outStream = new BufferedOutputStream(new FileOutputStream(fileName));
//					fileOpenResult = true;
//					System.out.println(Thread.currentThread().getName()+"-->BinWriter::open: "
//							+"Success opening [" + fileName + "] !!  "
//						  );
////				} 
////				finally {
////					System.err.println(Thread.currentThread().getName()+"-->BinWriter::open: "
////							+"Closing [" + fileName + "] on finally clause of open try statement. "
////						  );
////				    outStream.close();
////				}
//			}  
//		
//		catch(FileNotFoundException ex)   {
//			System.err.println(Thread.currentThread().getName()
//					+"-->BinWriter::open: "
//					+"Encountered FileNotFoundException trying to open [" + fileName + "]  "
//				  );
//			System.err.println(ex);
//			
//		}
//		    
//		//catch(IOException ex)		{
//		//	System.err.println(Thread.currentThread().getName()
//		//			+"-->BinWriter::open: "
//		//			+"Encountered IOException trying to open [" + fileName + "]  "
//		//		  );
//		//	System.err.println(ex);
//		//}
//		return fileOpenResult;
//	}	// END open() function
//
//	//=====================================================================================//
//	/**
//	 * Close the binary file 
//	 */
//	//=====================================================================================//
//	public void close ()
//	{
//	    try {
//			outStream.close();
//		} catch (IOException e) {
//			//e.printStackTrace();
//			System.err.println(Thread.currentThread().getName()+"-->BinWriter::close: "
//					+"Encountered IOException trying to close [" + fileName + "]  "
//				  );
//		}
//	}
//	
//	//=====================================================================================//
//	/**
//	 * Write a byte array to the given file.  Writing binary data is significantly simpler 
//	 * than reading it.
//	 * @param  bytes to write
//	 * @param  Output filename
//	 * 
//	 */
//	//=====================================================================================//
//	 public boolean write(byte[] inData)
//	 {
//	
//		boolean writeResult = false;
//		 
////		System.out.println(Thread.currentThread().getName()+"-->BinWriter::write: "
////				+"write to[" + fileName + "]  "
////				+"data[" + inData + "]  "
////			  );
//		if  (outStream == null)  {
//			// if file not open, then open before write
//			open (fileName);
//		}
//		try {
//		    outStream.write(inData);
//		    writeResult = true;
//		}  
//		
//		catch(FileNotFoundException ex)   {
//			System.err.println(Thread.currentThread().getName()+"-->BinWriter::write: "
//					+"Encountered FileNotFoundException writing to [" + fileName + "]  "
//				  );
//			System.err.println(ex);
//			
//		}
//		    
//		catch(IOException ex)		{
//			System.err.println(Thread.currentThread().getName()+"-->BinWriter::write: "
//					+"Encountered IOException writing to [" + fileName + "]  "
//				  );
//			System.err.println(ex);
//		}
//		return writeResult;
//	 }	// End write() function
//
//	
//	//=====================================================================================//
//	/**
//	 * Write a byte array to the given file.  Writing binary data is significantly simpler 
//	 * than reading it.
//	 * @param  bytes to write
//	 * @param  Output filename
//	 * @return 
//	 * 
//	 */
//	//=====================================================================================//
//	 public boolean writeFile(byte[] inData, String fn)
//	 {
//	
//		boolean fileWriteResult = false;
//		fileName = fn;
//		System.out.println(Thread.currentThread().getName()+"-->BinWriter::writeFile: "
//				+"write to[" + fileName + "]  "
//				+"data[" + inData + "]  "
//			  );
//		try {
//			OutputStream output = null;
//			try {
//				output = new BufferedOutputStream(new FileOutputStream(fileName));
//			    output.write(inData);
//			    fileWriteResult = false;
//			} finally {
//			    output.close();
//			}
//		}  
//		
//		catch(FileNotFoundException ex)   {
//			System.err.println(Thread.currentThread().getName()+"-->BinWriter::writeFile: "
//					+"Encountered FileNotFoundException writing to [" + fileName + "]  "
//				  );
//			System.err.println(ex);
//			
//		}
//		    
//		catch(IOException ex)		{
//			System.err.println(Thread.currentThread().getName()+"-->BinWriter::writeFile: "
//					+"Encountered IOException writing to [" + fileName + "]  "
//				  );
//			System.err.println(ex);
//		}
//		
//		return fileWriteResult;
//	 }	// End write() function
//	 
//
//	
////====================================================================================================================//
//// TEST ENTRY POINT
////====================================================================================================================//
//
//	/**
//	 * @param args
//	 */
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//	}	// end Main
//	
//	
//}		// END BinWriter Class
//
//
//	
