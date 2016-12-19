/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

/**
 * Classes implementing this interface communicate with a Publisher object.  A consumer will have one Receiver per Publisher.
 */
public interface Receiver {
	
	/**
	 * Request data of a certain type.  Depending on the connection type, this may allow for information to be pushed from the publisher.
	 * @param address string representing the address of the publisher. 
	 * @param keyword keyword describing type of data
	 * @return true if the publisher exists and can provide the requested type of data, false otherwise
	 */
	public boolean subscribe(String address, String keyword);
	
	/**
	 * Stop receiving data from the given source of the indicated type.  This should (eventually) close the associated connection.
	 * @param address
	 * @param keyword
	 */
	public void unsubscribe(String address, String keyword);
	
	/**
	 * Retrieve the current available data.  This may be data that has been pushed earlier and cached locally, 
	 * or it may be requested from the server now.
	 * @param keyword type of data to request
	 * @return String representing the current data
	 */
	public String request(String keyword);
	
	/**
	 * Return true if new data (since the last call to this method) is available associated with 
	 * keyword.  This either keeps track of server pushes or else queries the server directly. 
	 */
	public boolean hasUpdate(String keyword);
	
}
