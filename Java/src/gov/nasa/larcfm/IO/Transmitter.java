/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

/**
 * Classes implementing this interface communicate with one or more specific Receiver objects.  
 * Generally a Transmitter and Receiver are created as a coupled pair.
 * A Transmitter may only communicate with a specific type of Receiver and vice versa, which share a common communication medium and encoding method.
 * It is not necessary for a Transmitter to be able to speak with arbitrary Receivers.
 * 
 * Over the communication medium, it should allow two main types of message:
 * - subscription request: client attempts to request data based on a given keyword.  If it is a recognized keyword, register the client, otherwise close the connection.
 * - data transfer: this data may be automatically pushed out to all subscribers on the server's timeframe, or subscribers may explicitly request data on their timeframes,
 * 
 * The details of how these are accomplished are determined by a particular Transmitter/Receiver matched pair.
 */
public interface Transmitter {
	/**
	 * Publisher announces it has data of type keyword.
	 * @param address string representing the address to be listened to for subscribers (e.g. port)
	 * @param keyword Keywords should be cumulative for all data types presented
	 */
	public void publish(String address, String keyword);
	
	/**
	 * Publisher removes a certain keyword from its publishing list.
	 * @param keyword Keywords to remove
	 */
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
	
	/**
	 * Attempts to cleanly shuts down this transmitter, releasing any associated resources.  This does not "unpublish" the transmitter. 
	 * @return true if shut down successfully.
	 */
	public boolean shutdown();
}
