/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Debug;
import gov.nasa.larcfm.Util.GeneralPlan;
import gov.nasa.larcfm.Util.GeneralState;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.f;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

/**
 * This class represents a wrapper for a collection of Transmitter object that represents a server's behavior.
 * The server takes updates to plan, state, and/or parameter information and parcels it out to its various Transmitter object.
 * Each Transmitter is intended to handle a certain type of request (e.g. for plan information that is updated every second 
 * and sent over a socket) and handles distributing the information to its clients.
 * The particulars of the transmission of data are handled by the provided Transmitters.
 * 
 * Known keywords:
 * GeneralState, GeneralPlan, ParameterData, String
 */
public class DataServer {
	Hashtable<String,Transmitter> transmitters; //address->transmitter
	String word;

	public DataServer() {
		transmitters = new Hashtable<String,Transmitter>();
		word = null;
	}

	/**
	 * Add a transmitter at a given address that will listen for clients
	 * @param address This will replace any existing transmitter at this address.
	 * @param t
	 */
	public void addTransmitter(String address, Transmitter t) {
		if (transmitters.containsKey(address)) {
			transmitters.get(address).shutdown();
		}
		transmitters.put(address,t);
		if (word != null) {
			getTransmitter(address).publish(address,word);
		}
	}

	/**
	 * Return the transmitter associated with a given address.  This will return null if no transmitter is associated with the given address.
	 * @param address
	 * @return
	 */
	public Transmitter getTransmitter(String address) {
		return transmitters.get(address);
	}

	/**
	 * Delete the transmitter associated with an address
	 * @param address
	 */
	public void removeTransmitter(String address) {
		transmitters.remove(address).shutdown();
	}

	/**
	 * Tell any clients that connect and ask what sort of data you provide.
	 * This propagates the same keyword to all current and future transmitters
	 * @param keyword
	 */
	public void advertiseAll(String keyword) {
		word = keyword;
		for (String address : transmitters.keySet()) {
			transmitters.get(address).publish(address, keyword);
		}
	}

	/**
	 * Associate a keyword with one particular Transmitter.
	 * This does not affect other Transmitters.
	 * @param address
	 * @param keyword
	 */
	public void advertiseOne(String address, String keyword) {
		if (transmitters.containsKey(address)) {
			transmitters.get(address).publish(address, keyword);
		}
	}

	/**
	 * Transmit String with user-specified keyword
	 * @param keyword
	 * @param data
	 */
	public void update(String keyword, String data) {
		for (String address : transmitters.keySet()) {
			transmitters.get(address).update(keyword, data);
		}		
	}

	/**
	 * Return true if this server has any clients looking for the given keyword
	 * @param keyword
	 * @return
	 */
	public boolean hasClients(String keyword) {
		for (String s : transmitters.keySet()) {
			if (transmitters.get(s).hasSubscribers(keyword)) return true;
		}
		return false;
	}
	
	//*******************************


	/**
	 * Transmit a set of states
	 */
	public void updateGeneralStates(List<GeneralState> p) {
		updateGeneralStates(p,null);
	}

	public void updateGeneralStates(List<GeneralState> p, ParameterData pd) {
		GeneralStateWriter gsw = new GeneralStateWriter();
		StringWriter sw = new StringWriter();
		gsw.open(sw);		
		if (pd != null) gsw.setParameters(pd);
		for (int i = 0; i < p.size(); i++) {
			gsw.writeState(p.get(i));
		}
		update("GeneralState", sw.toString());
		gsw.close();
	}

	/**
	 * Transmit a set of plans
	 */
	public void updateGeneralPlans(List<GeneralPlan> p) {
		updateGeneralPlans(p,null);
	}

	public void updateGeneralPlans(List<GeneralPlan> p, ParameterData pd) {
		GeneralPlanWriter gpw = new GeneralPlanWriter();
		StringWriter sw = new StringWriter();
		gpw.open(sw);
		if (pd != null) gpw.setParameters(pd);
		for (int i = 0; i < p.size(); i++) {
			gpw.writePlan(p.get(i));
		}
		String s = sw.toString();
		update("GeneralPlan", s);
		gpw.close();
	}

	/**
	 * Transmit a ParameterData object
	 */
	public void updateParameters(ParameterData p) {
		update("ParameterData", p.toString());
	}

	/**
	 * Transmit a string
	 */
	public void updateString(String s) {
		update("String", s);
	}

	/**
	 * Transmit a string with some associated keyword
	 * @param keyword
	 * @param s
	 */
	public void updateString(String keyword, String s) {
		update(keyword, s);
	}

}
