/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import gov.nasa.larcfm.Util.Pair;



/**
 * This transmitter utilizes an existing *nix named FIFO pipe (see mkfifo) to communicate.  
 * This utilizes separate writer threads so that the main process doesn't block waiting for a receiver to connect.
 * This potentially supports multiple FIFOs.
 * 
 * Data string blocks are designated by special comment lines NamedPipeTransmitter.STARTDATA and NamedPipeTransmitter.ENDDATA:
 * 
 * #START_PIPE_COMMUNICATION
 * ...data here...
 * #END_PIPE_COMMUNICATION
 */
public class NamedPipeTransmitter implements Transmitter {
	public static final String STARTDATA = "#START_PIPE_COMMUNICATION";
	public static final String ENDDATA = "#END_PIPE_COMMUNICATION";

	Hashtable<String,Pair<String,Long>> data; // current set of data to write: key -> (data,timestamp)
	Boolean sync;
	Hashtable<ServerThread, String> servers; // server, keyword
	HashSet<String> keywords;

	public NamedPipeTransmitter() {
		servers = new Hashtable<ServerThread,String>();
		keywords = new HashSet<String>();
	}

	@Override
	public void publish(String address, String keyword) {
		keywords.add(keyword);
		ServerThread server = new ServerThread(address,keyword);
		servers.put(server,keyword);
		server.start();
	}

	public void unpublish(String keyword) {
		keywords.remove(keyword);
		Iterator<ServerThread> it = servers.keySet().iterator();
		while (it.hasNext()) {
			ServerThread s = it.next();
			if (servers.get(s).equals(keyword)) {
				s.halt();
				servers.remove(s);
			}
		}
	}

	public boolean shutdown() {
		Iterator<ServerThread> it = servers.keySet().iterator();
		while (it.hasNext()) {
			ServerThread s = it.next();
			s.halt();
			servers.remove(s);
		}
		keywords.clear();
		return true;
	}

	@Override
	public void update(String keyword, String newdata) {
		if (newdata != null) { // there is a size limit of 64kb to serializable strings (UTF char = 2b?)
			synchronized(sync) {
				data.put(keyword, new Pair<String, Long>(newdata,System.currentTimeMillis()));
			}
		}
	}

	public boolean hasSubscribers(String keyword) {
		return servers.containsValue(keyword);
	}

	class ServerThread extends Thread {
		String filename;
		String key;
		File outfile;
		PrintWriter writer;
		boolean running;
		long myLastTime;

		public ServerThread(String address, String keystring) {
			super("NamedPipeTransmitter.ServerThread");
			filename = address+"."+keystring;
			running = true;
			myLastTime = -1L;
			try {
				writer = new PrintWriter(new FileWriter(outfile));
				key = keystring;
			} catch (IOException e) {
				System.out.println("Could not open server FIFO file "+filename);
				e.printStackTrace();
			}
		}

		public void halt() {
			writer.close();
			running = false;
		}

		public void run() {
			while (running) {
				long time;
				String localdata = "";
				synchronized (sync) {
					time = data.get(key).second;
					if (time > myLastTime) {
						localdata = data.get(key).first;
					}
				}
				if (time > myLastTime) {
					// may block on writing
					writer.println(STARTDATA);
					writer.println(localdata);
					writer.println(ENDDATA);
					writer.flush();
					myLastTime = System.currentTimeMillis();
				}
			}
		}
	}

}
