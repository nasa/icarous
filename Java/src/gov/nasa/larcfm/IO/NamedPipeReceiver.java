/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Hashtable;

import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.f;


/**
 * This receiver utilizes an existing *nix named FIFO pipe (see mkfifo) to communicate.  
 * This utilizes separate reader threads so that the main process doesn't block waiting for the transmitter.
 * This potentially supports multiple FIFOs.
 * 
 * Data string blocks are designated by special comment lines NamedPipeTransmitter.STARTDATA and NamedPipeTransmitter.ENDDATA:
 * 
 * #START_PIPE_COMMUNICATION
 * ...data here...
 * #END_PIPE_COMMUNICATION
 */
public class NamedPipeReceiver implements Receiver {
	Hashtable<String,ClientThread> clients; // address, client
	Hashtable<String,Triple<String,Long,Long>> data; // data string, timestamp retrieved, timestamp checked

	public NamedPipeReceiver() {
		clients = new Hashtable<String,ClientThread>();
		data = new Hashtable<String,Triple<String,Long,Long>>();
	}

	public NamedPipeReceiver(String address, String keyword) {
		clients = new Hashtable<String,ClientThread>();
		data = new Hashtable<String,Triple<String,Long,Long>>();
	}

	// start a listening thread for data from this keyword.
	public boolean subscribe(String address, String keyword) {
		if (!clients.containsKey(address)) {
			ClientThread client = new ClientThread();
			if (!client.subscribe(address,keyword)) {
				f.pln("Server does not support "+keyword);
				return false;
			} else {
				f.pln("starting client");
				client.start();
				clients.put(address,client);
				return true;
			}
		} else {
			return false;
		}
	}

	@Override
	public void unsubscribe(String address, String keyword) {
		if (clients.containsKey(address)) {
			clients.get(address).halt();
		}
	}

	@Override
	public String request(String keyword) {
		Triple<String,Long,Long> dat;
		synchronized(data) {
			dat = data.get(keyword);
		}
		if (dat != null) {
			return dat.first;
		}
		return "";
	}

	@Override
	public boolean hasUpdate(String keyword) {
		boolean ok = false;
		synchronized(data) {
			Triple<String,Long,Long> dat = data.get(keyword);
			if (dat != null) {
				ok = (dat.second > dat.third);
				if (ok) {
f.pln("NamedPipeReceiver has new update "+keyword);					
					data.put(keyword, Triple.make(dat.first, dat.second, System.currentTimeMillis()));
				}
			}
		}
		return ok;
	}


	class ClientThread extends Thread {
		File infile;
		String filename;
		String key;
		BufferedReader reader;
		boolean running;

		public ClientThread() {
			infile = null;
			reader = null;
			running = true;
		}

		/**
		 * Set up a socket with the indicated address.
		 * @param address String in format "host:port"
		 * @return true if success, else false.
		 */
		public boolean subscribe(String address, String keystring) {
			reader = null;
			filename = address+"."+keystring;
			infile = new File(filename);
			if (infile.exists()) {
				f.pln("FIFO file "+filename+" does not exist");
				return false;
			}
			try {
				reader = new BufferedReader(new FileReader(infile));
				key = keystring;
				return true;
			} catch (IOException e) {
				f.pln("Could not open reader for FIFO file "+filename);
				reader = null;
				e.printStackTrace();				
			}
			return false;
		}

		public void halt() {
			running = false;
			if (reader != null) {
				try {
					reader.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}

		public void run() {
			if (reader != null) {
				try {
					StringBuffer fulldata = new StringBuffer();
					boolean reading = false;
					while (running) {
						String line = reader.readLine()+"\n";
						if (line.startsWith(NamedPipeTransmitter.STARTDATA)) {
							fulldata.setLength(0);
							reading = true;
						} else if (line.endsWith(NamedPipeTransmitter.ENDDATA)) {
							if (reading) {
								long mytime = System.currentTimeMillis();
								synchronized(data) {
									data.put(key, Triple.make(fulldata.toString(),mytime,-1L)); // never read
								}
							}
							fulldata.setLength(0);
							reading = false;
						} else if (reading){
							fulldata.append(line);
						}
					}
				} catch (IOException e) {
					try {
						reader.close();
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					f.pln("Error reading from FIFO file "+filename);
					e.printStackTrace();
				}
			}
		}
	}

}





