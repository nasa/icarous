/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

/**
 * Classes implementing this interface communicate with one or more Receiver objects.
 * 
 * Over the communication medium, it should allow three main types of message:
 * - subscription request
 * - data push OR (update status query AND data request)  
 * 
 */
public interface Transmitter {
	/**
	 * Publisher announces it has data of type keyword.
	 * @param address string representing the address to be listened to for subscribers (e.g. port)
	 * @param keyword Keywords should be cumulative for all data types presented
	 */
	public void publish(String address, String keyword);
	
	public void unpublish(String keyword);
	
	/**
	 * Publisher updates data to be transmitted.  Depending on the connection type, this may immediately 
	 * push the data to all subscribers or cache it until requested.
	 * @param keyword which keyword this data is associated with
	 * @param data
	 */
	public void update(String keyword, String data);
	
	/**
	 * Returns true if there is at least one active subscriber associated with this keyword
	 * @param keyword
	 * @return
	 */
	public boolean hasSubscribers(String keyword);
}
