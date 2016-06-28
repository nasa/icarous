/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.serial;
//
////import gov.nasa.larc.AP.AP;
////import gov.nasa.larc.ICAROUS.Alert;
////import gov.nasa.larc.ICAROUS.Codec;
////import gov.nasa.larc.ICAROUS.WatchCodec;
////import java.io.BufferedOutputStream;
////import java.io.FileNotFoundException;
////import java.io.FileOutputStream;
////import java.io.IOException;
////import java.io.OutputStream;
////import java.text.DecimalFormat;
////import java.util.ArrayList;
//
//public class ByteQueue {
//
//	
//	
//	//====================================================================================================================//
//	// DATA Section
//	//====================================================================================================================//
//
//	static int defaultSize = 10000;
//	static int testSize = 2000;
//
//	int capacity = defaultSize;
//	byte[] bque = null;
//	int head = -1;	// index of first VALID OCCUPIED byte space
//	int tail = -1;	// index of last VALID OCCUPIED byte space
//	int fill = 0;	// the number of occupied bytes
//	int count = 0;
//	int test = 0;
//
//	
//	// optional logging
//	String logFN = null;
//	BinWriter mylog = null;
//	
//	//====================================================================================================================//
//	// FUNCTION Section
//	//====================================================================================================================//
//
//
//	//=====================================================================================//
//	/**
//	 * Default constructor creates byte buffer with default capacity (500 bytes)..
//	 * @param logFileName 
//	 * @param i 
//	 */
//	//=====================================================================================//
//	public ByteQueue ()
//	{
//		this(defaultSize, null);
//		
//	}
//	
//
//	//=====================================================================================//
//	/**
//	 * Create byte queue with given capacity.  if < 1 given, then creates default capacity (500).
//	 * @param Desired buffer capacity
//	 * @return Capacity created
//	 */
//	//=====================================================================================//
//	public  ByteQueue (int cap, String fn)
//	{
//		if (cap < 1) cap = defaultSize;
//		capacity = cap;
//		bque = new byte[cap];		// this is the storage array for the que
//		//int head = -1;
//		//int tail = -1;
//		fill = 0;
//
//		if (fn != null) {
//			mylog = new BinWriter(fn);
//		}
//		
//		//return capacity;
//		
//	}
//	
//	//=====================================================================================//
//	/**
//	 * synchronized add.  Assumes the byte array passed in is filled with no null terminators.
//	 * That is, newbytes.length reflects all valid bytes to add to the byte buffer. 
//	 * @param  bytes to add
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public synchronized int enque (byte[] newbytes)
//	{
//		//boolean found32 = false;
//		
//		if (mylog != null) {
//			mylog.write(newbytes);
//		}
//		
//		// check size
//		int spaceAvail = capacity - fill;
//		if (spaceAvail < newbytes.length)
//		{
//			growSpace (   (int)  ((fill+newbytes.length)*1.2)   );
//		}
//		
//		fill += newbytes.length;	// assume for now all bytes in newbytes are important - including null bytes.
//		// start adding newbytes from tail  
//		int j = tail;		// tail is -1 for empty bque[]
//		//System.err.println("Edge-->ByteQueue::enque: newbytes to be added to queue: "+ Codec.bytesToInt(newbytes));
//		for (int i=0; i < newbytes.length; i++)
//		{
//			j = (++j) % capacity;
////			if (newbytes[i] == (byte)32) {
////				found32 = true;
////			}
//			bque[j] = newbytes[i];
//			
//		}
////		if (found32) {		// possibly a position message
////			System.err.println( Thread.currentThread().getName()
////					+"--> ByteQueue::enque: Found a '32' in the"
////					+" incomming bytes["+bytesToInt(newbytes, 0, newbytes.length)+"]"
////					+"returning [null]"
////				  );
////
////		}
//		if (head == -1) head = 0;	// set head if this is the first enque
//		tail = j;
//
//	
//		return fill;
//		
//	}	// end enque() function
//
//
//
//	//=====================================================================================//
//	/**
//	 * Copy bytes from offset to offset+len.  Also remove them from the queue.  
//	 * That is, newbytes.length reflects all valid bytes to add to the byte buffer. 
//	 * @param  sidx is index of message marker
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public byte[] deque (int offset, int len)
//	{
//		// sanity check on size to deque
//		if (len > fill)  {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::deque: "
//					+"requested len ["+len+"]"
//					+" > available bytes in queue ["+fill+"]..."
//					+"returning [null]"
//				  );
//			return null;
//		}
//	
//		if (offset < 0) {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::deque: "
//					+"Invalid offset ["+offset+"]"
//					+" offset must be non negative..."
//					+"returning [null]"
//				  );
//			return null;
//		}
//		if (offset+len > fill) {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::deque: "
//					+" offset["+offset+"]+len ["+len+"] > fill["+fill+"]"
//					+"returning [null]"
//				  );
//			return null;
//		}
//		byte[] copy = new byte[len];
//		//System.err.println("Edge-->ByteQueue::deque(): bque[] :  "+ Codec.bytesToInt(bque));
//
//		int eidx = (head+offset) % capacity;
//		for (int i = 0; i < len; i++) {
//			copy[i] = bque[eidx];
//			eidx = (eidx + 1) % capacity;
//		}
//		fill -= (offset+len);
//		if (fill <= 0) {
//			clear();  // clear head tail index to empty condition
//		} else {
//			//head = (head + offset + len) % capacity;
//			head = eidx;
//		}		
//		
//		
//		return copy;
//				
//	}	// end deque() function
//	
//
//	//=====================================================================================//
//	/**
//	 * Copy bytes from queue without removing them from the queue.  
//	 * 
//	 * @param  sidx is index of message marker
//	 * @return occupied bytes in buffer.
//	 */
//	//=====================================================================================//
//	public byte[] copy (int offset, int len)
//	{
//		// sanity check on size to deque
//		if (len > fill)  {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::copy: "
//					+"requested len ["+len+"]"
//					+" > available bytes in queue ["+fill+"]..."
//					+"returning [null]"
//				  );
//			return null;
//		}
//		if (offset < 0) {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::copy: "
//					+"Invalid offset ["+offset+"]"
//					+" offset must be non negative..."
//					+"returning [null]"
//				  );
//			return null;
//		}
//		if (offset+len > fill) {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::copy: "
//					+" offset["+offset+"]+len ["+len+"] > fill["+fill+"]"
//					+"returning [null]"
//				  );
//			return null;
//		}
//		byte[] copy = new byte[len];
//		int eidx = (head+offset) % capacity;
//		for (int i = 0; i < len; i++) {
//			copy[i] = bque[eidx];
//			eidx = (eidx + 1) % capacity;
//		}
//		return copy;
//	}	// end copy() function
//	
//	//=====================================================================================//
//	/**
//	 * returns true if testIdx is between head and tail index in the buffer.
//	 * @return true if testIdx is in valid range.
//	 */
//	//=====================================================================================//
//	public boolean isValid (int testIdx)
//	{
//		if (head <= tail)  {
//			if ( (testIdx >= head) && (testIdx <= tail) )
//				return true;
//			else
//				return false;
//		} else {	// (tail < head)
//			if ( (testIdx >= head) && (testIdx <= (capacity-1)) )
//				return true;
//			else if ( (testIdx >=0 ) && (testIdx <= tail))
//				return true;
//			else 
//				return false;
//		}
//		
//	}	// end of isValid()
//
//	//=====================================================================================//
//	/**
//	 * synchronized expand byte que capacity.
//	 * @param newSize 
//	 * @return new capacity.
//	 */
//	//=====================================================================================//
//	public int growSpace (int newSize)
//	{	
//		count++;
//		
//		System.err.println ("Thread "+ Thread.currentThread().getName()+" =================================  ByteQueue::growSpace:got call "+ count+" times ============================= ");
//		System.err.println ("Serial-->ByteQueue::growSpace: Head = : "+head +" Tail = "+ tail);
//		clear();
//		//Example of resizing byte array
//		//http://ostermiller.org/utils/src/CircularByteBuffer.java.html
//		return 0;// return the new space available.
//	}
//	
//
//	//=====================================================================================//
//	/**
//	 * 
//	 * @return counts to go from head pointer to the given index.  if idx is outside valid 
//	 * range, then return -1.
//	 */
//	//=====================================================================================//
//	public int capacity()
//	{
//		return capacity;
//	}
//	
//	//=====================================================================================//
//	/**
//	 * 
//	 * @return counts to go from head pointer to the given index inclusive of given index.  if idx is outside valid 
//	 * range, then return -1.
//	 */
//	//=====================================================================================//
////	public int distanceTo (int testIdx)
////	{
////		int distance = 0;
////		if (head <= tail)  {
////			if ( (testIdx >= head) && (testIdx <= tail) )	// valid index
////				distance =  testIdx - head + 1;
////			else	// not valid idx
////				distance =  -1;
////		} else {	// (tail < head)
////			if ( (testIdx >= head) && (testIdx < (capacity)) )
////				distance = testIdx - head + 1;
////			else if ( (testIdx >=0 ) && (testIdx <= tail))
////				distance = ( (capacity - head) + testIdx );
////			else 
////				distance = -1;
////		}
////		
////		if ((distance <= 0) || (distance > capacity))  {
////			System.err.println( Thread.currentThread().getName()
////								+"--> ByteQueue::distanceTo: distance compute error "
////								+"head["+head+"]"
////								+"tail["+tail+"]"
////								+"testIdx["+testIdx+"]"
////								+"distance["+distance+"]"
////							  );
////		}
////		
////		return distance;
////	}
//	
//
//	//=====================================================================================//
//	/**
//	 * 
//	 * @return size of valid data in queue.
//	 */
//	//=====================================================================================//
//	public int size ()
//	{
//		return fill;
//	}
//	
//	//=====================================================================================//
//		/**
//		 * 
//		 * @return head of queue.
//		 */
//		//=====================================================================================//
//	public  int getHead()
//	{
//		return head;
//	}
//	//=====================================================================================//
//	/**
//	 * 
//	 * @return tail of queue.
//	 */
//	//=====================================================================================//
//	public  int getTail()
//	{
//		return tail;
//	}
//	//=====================================================================================//
//	//=====================================================================================//
//		/**
//		 * Shifts head by value given.
//		 * @param integer value representing how far to shift, integer value 0-1 indicating the direction of shift
//		 * @return size of shift.
//		 */
//		//=====================================================================================//
//	public  void shiftHead(int shift,int direction)
//		{
//			System.err.println("Serial-->ByteQueue::shiftHead: Shifted head " + shift + " bytes after packet lost");
//			if(direction == 0){ //if direction == 0 we are shifting left
//			int val = head - shift;
//			if( val < 0	) //if the value is less than 0 we are wrapping around the circular queue
//			{
//				head = capacity-(shift-head);// move the head to the correct index shift-head 
//			}
//			else
//			{
//			head = val;
//			}
//			fill += shift;
//			}
//			else
//			{
//			
//				head = (head+shift)%capacity;
//				fill -= shift;
//			}
//		
////			int nextMarker = this.indexOf(Codec.msgMarker, 0);
////			head += nextMarker;
////			fill -= nextMarker;
////			System.err.println("Head After Shift "+ head);
//
//			//System.err.println("Serial-->ByteQueue::shiftHead: Bytes after head is shifted: "+ Codec.bytesToInt(bque,head,12));
//
//
//			
//		}
//		//=====================================================================================//
//		
//
//	//=====================================================================================//
//	public void clear()
//	{
//		head = tail = -1;
//		fill = 0;
//	}
//	
//
//	//=====================================================================================//
//	/**
//	 * Discard contents up to one past the given index.  Moves head pointer down.
//	 * @return size of valid data in queue.
//	 */
//	//=====================================================================================//
//	public void clear(int offset) {
//
//		int aidx = (head + offset +1) % capacity;
//		if (isValid(aidx)) {
//			head = aidx;
//			fill -= offset+1;
//		} else {
//			System.err.println( Thread.currentThread().getName()
//					+"--> ByteQueue::clear: invalid index to clear to : "
//					+"head["+head+"]"
//					+"tail["+tail+"]"
//					+" clear to idx["+offset+"]"
//					+"fill["+fill+"]"
//					+"Head indx moved over by one."
//				  );
//			head = (head + 1) % capacity;
//			fill -= 1;
//		}
//		
//	}
//
//	
//	//=====================================================================================//
//	/**
//	 * Find a given byte in the byte buffer and return its index.
//	 * @return index of first found byte or -1 if not available.
//	 */
//	//=====================================================================================//
//	public int indexOf (byte marker)
//	{
//		
//	//	head = head+shift;
//		
//		if ( (head == -1) && (tail == -1) ) return -1;	// take care of empty buffer case
//		int midx = head;				// start from the head index and search toward tail index
////		boolean found = false;
//		int offset = 0;
//		while (    ( bque[midx] != marker )
//			    && ( midx != (tail) )
//			  )
//		{ 
//			offset++;
//			midx = (midx+1) % capacity;
//		}
//		test = 0;
////		if ( (bque[midx] == marker) || (midx !=  tail) )
//		if ( (bque[midx] == marker) )  {
////			System.err.println( Thread.currentThread().getName()
////					+"--> ByteQueue::indexOf: found header byte : ["+marker+"]"
////					+ String.format(" head[%4d]", head)
////					+ String.format(" tail[%4d]", tail)
////					+ String.format(" offset[%4d]", offset)
////					+ String.format(" fill[%4d]", fill)
////					+"Head bytes [" + bytesToInt(bque, head+offset, 6) + "]"
////					);
//			return offset;
//		} else 
////			System.err.println( Thread.currentThread().getName()
////					+"--> ByteQueue::indexOf: NOT header byte : ["+marker+"]"
////					+ String.format(" head[%4d]", head)
////					+ String.format(" tail[%4d]", tail)
////					+ String.format(" offset[%4d]", offset)
////					+ String.format(" fill[%4d]", fill)
////					+" buffer contents [" + bytesToInt(bque, head, fill) + "]"
////					);
//			return -1;
//		
//	}
//	
//	//=====================================================================================//
//	/**
//	 * Return the byte at given index.
//	 * @return index of first found byte or -1 if not available.
//	 */
//	//=====================================================================================//
//	public byte get (int offset)
//	{
//		int idx = (head + offset) % capacity;
//		idx = idx % capacity;
//		return bque[idx];
//	}
//	
//	//=====================================================================================//
//	/**
//	 * convert a byte array into a comma  string
//	 * */
//	//=====================================================================================//
//	public static String bytesToInt (byte[] bytes, int start, int len) 
//	{     
//		String out = new String();
//		out += String.format( "%4d", (int)bytes[start] );
//		for ( int j = start+1; j < start+len; j++ ) 
//		{         
//			out += ","+ String.format( "%4d", (int)bytes[j] );         
//		}     
//		return out; 
//	} 
//	
//
//	//=====================================================================================//
//	/**
//	 * Find a given byte in the byteque and return its index.
//	 * @return index of found byte of -1 if not available.
//	 */
//	//=====================================================================================//
//	public static void main ()
//	{
//		//int a = -1 % 3;
//
//	}
//
//
//	
//}	// END ByteQueue class
