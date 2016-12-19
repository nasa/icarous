/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.f;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Hashtable;
import java.util.LinkedList;

/**
 * This class signs up with a SocketPushTransmitter and locally caches any data sent to it.
 * This is intended for applications where the client updates more frequently than the server. 
 */
public class SocketPushReceiver implements Receiver {
	Hashtable<String,ClientThread> clients; // address, client
	Hashtable<String,Triple<String,Long,Long>> data; // data string, timestamp retrieved, timestamp checked

	public SocketPushReceiver() {
		clients = new Hashtable<String,ClientThread>();
		data = new Hashtable<String,Triple<String,Long,Long>>();
	}

	public SocketPushReceiver(String address, String keyword) {
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
//f.pln("hasupdate: "+dat.first.length()+" "+dat.second+" "+dat.third+" new="+ok);			
				if (ok) {
					data.put(keyword, Triple.make(dat.first, dat.second, System.currentTimeMillis()));
				}
			}
		}
		return ok;
	}

	class ClientThread extends Thread {
		Socket socket;
		String key;
		boolean running;
		ObjectOutputStream out;
		ObjectInputStream in;

		public ClientThread() {	
			socket = null;
			key = null;
			running = true;
		}

		/**
		 * Set up a socket with the indicated address.
		 * @param address String in format "host:port"
		 * @return true if success, else false.
		 */
		public boolean subscribe(String address, String keystring) {
			socket = null;
			String[] addrline = address.split(":");
			if (addrline.length != 2) {
				f.pln("Invalid socket address string -- must be in format host:port");
				return false;
			}
			try {
				String host = addrline[0];
				int port = Integer.parseInt(addrline[1]);
//f.pln("opening socket to "+host+" "+port);				
				socket = new Socket(host, port);
//f.pln("opening stream to "+host+" "+port);				
				out = new ObjectOutputStream(socket.getOutputStream());
//f.pln("requesting subscription");				
				out.writeUTF(keystring);
				out.writeLong(System.currentTimeMillis());
				out.flush();
				in = new ObjectInputStream(socket.getInputStream());
//f.pln("waiting for response");				
				boolean supported = in.readBoolean();
//f.pln("supported = "+supported);
				if (!supported) {
					socket.close();
				}
				return supported;
			} catch (NumberFormatException e) {
				f.pln("Could not parse port number");
				e.printStackTrace();
			} catch (IOException e) {
				f.pln("Could not open socket at address="+addrline[0]+" port="+addrline[1]);
				e.printStackTrace();				
			}
			return false;
		}

		public void halt() {
			running = false;
			if (socket != null) {
				try {
					socket.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}

		public void run() {
//f.pln("running client");			
			if (socket != null) {
//f.pln("starting loop");				
				try {
//f.pln("streams open");		
					StringBuffer fulldata = new StringBuffer();
					int prev = -1;
					boolean ok = true;
					while (running) {
//f.pln("waiting for data");						
						// data is keyword, data, timestamp
						int num = in.readInt();
						// reset packet counting
						if (num == 0) {
							ok = true;
							prev = -1;
							fulldata = new StringBuffer();
						} else {
							prev++;
						}
						int max = in.readInt();
//f.pln("num/max "+num+"/"+max);						
						String key = in.readUTF();
						String dat = in.readUTF();
						long time = in.readLong();
						// check for out-of-sync packets
						if (num == prev+1) {
							fulldata.append(dat);
						} else {
							ok = false;
						}
						// full message received, add to data
						if (num == max && ok) {
//f.pln("store full message");
							long mytime = System.currentTimeMillis();
							synchronized(data) {
								data.put(key, Triple.make(fulldata.toString(),mytime,-1L)); // never read
							}
						}
					}
				} catch (IOException e) {
					try {
						socket.close();
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}


}
