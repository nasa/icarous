/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.GeneralPlan;
import gov.nasa.larcfm.Util.GeneralState;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.f;

import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

/**
 * Known keywords:
 * GeneralState, GeneralPlan, ParameterData
 */
public class DataClient {
	Hashtable<String,Receiver> receivers;

	public DataClient() {
		receivers = new Hashtable<String,Receiver>();
	}

	/**
	 * Add a communication link with a new source
	 * @param address
	 * @param r
	 */
	public void addReceiver(String address, Receiver r) {
		receivers.put(address,r);
	}

	/**
	 * Return the Receiver associated with an address
	 * @param address
	 * @param r
	 */
	public Receiver getReceiver(String address) {
		return receivers.get(address);
	}
	
	/**
	 * Delete the receiver associated with an address
	 * @param address
	 */
	public void removeReceiver(String address) {
		receivers.remove(address);
	}
	
	
	/**
	 * Notify a source what sort of data you are interested in
	 * @param address
	 * @param keyword
	 * @return
	 */
	public boolean subscribe(String address, String keyword) {
		Receiver r = receivers.get(address);
		if (r == null) return false;
		return r.subscribe(address, keyword);
	}

	/**
	 * Notify a source that you are no longer interested in a type of data
	 * @param keyword
	 */
	public void unsubscribe(String keyword) {
		for (String address : receivers.keySet()) {
			receivers.get(address).unsubscribe(address, keyword);
		}
	}

	/**
	 * Get a list of keyword-specified strings from all subscribed sources
	 * @param keyword
	 * @return
	 */
	public List<String> request(String keyword) {
		ArrayList<String> list = new ArrayList<String>();
		for (String address : receivers.keySet()) {
			if (receivers.get(address).hasUpdate(keyword)) {
				list.add(receivers.get(address).request(keyword));
			}
		}
		return list;
	}


	public Pair<List<GeneralState>,ParameterData> getGeneralStatesAndParameters() {
		List<String> p = request("GeneralState");
		ArrayList<GeneralState> states = new ArrayList<GeneralState>();
		ParameterData pd = new ParameterData();
		for (String s : p) {
			GeneralSequenceReader gsr = new GeneralSequenceReader();
			StringReader sr = new StringReader(s);
			gsr.open(sr);
			pd.copy(gsr.getParameters(), true);
			states.addAll(gsr.getGeneralStateList());
		}
		return Pair.make(states,pd);
		
	}
	
	/**
	 * Get a list of GeneralStates from all subscribed sources
	 * There may be duplicate entries if multiple sources have similar data
	 * @return
	 */
	public List<GeneralState> getGeneralStates() {
		return getGeneralStatesAndParameters().first;
	}


	public Pair<List<GeneralPlan>,ParameterData> getGeneralPlansAndParameters() {
		List<String> p = request("GeneralPlan");
		ArrayList<GeneralPlan> plans = new ArrayList<GeneralPlan>();
		ParameterData pd = new ParameterData();
		for (String s : p) {
			GeneralPlanReader gpr = new GeneralPlanReader();
			StringReader sr = new StringReader(s);
			gpr.open(sr);			
			pd.copy(gpr.getParameters(), true);
//f.pln(s);
//f.pln("gpr.size="+gpr.size()+" msg="+gpr.getMessage());			
			plans.addAll(gpr.getGeneralPlanList());
		}
		return Pair.make(plans,pd);		
	}
	
	/**
	 * Get a list of GeneralPlans from all subscribed sources
	 * There may be duplicate entries if multiple sources have similar data
	 * @return
	 */
	public List<GeneralPlan> getGeneralPlans() {
		return getGeneralPlansAndParameters().first;
	}

	/**
	 * Get a list of parameterdata objects from all subscribed sources
	 * @return
	 */
	public List<ParameterData> getParameterData() {
		ArrayList<ParameterData> pd = new ArrayList<ParameterData>();
		List<String> p = request("ParameterData");
		for (String s : p) {
			ConfigReader cr = new ConfigReader();
			StringReader sr = new StringReader(s);
			cr.open(sr);
			pd.add(cr.getParameters());
		}
		return pd;
	}

	/**
	 * Get a list of bare strings from all subscribed sources
	 * @return
	 */
	public List<String> getStrings() {
		return request("String");
	}

	/**
	 * Get a list of strings associated with a user-specified keyword from all subscribed sources
	 * @param keyword
	 * @return
	 */
	public List<String> getStrings(String keyword) {
		return request(keyword);
	}

}
