/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;

/**
 * This server actively pushes data to and SocketPushReceivers that have signed up with it.
 * Data is cached on the client side.
 * This is intended for applications where the client updates more frequently than the server.
 */
public class SocketPushTransmitter implements Transmitter {
	int port;
	ServerThread server;
	Hashtable<Socket, String> servers; // clients, keywords
	Hashtable<Socket, ObjectOutputStream> outs;
	HashSet<String> keywords;

	public SocketPushTransmitter() {
		servers = new Hashtable<Socket,String>();
		outs = new Hashtable<Socket,ObjectOutputStream>();
		port = -1;
		server = null;
		keywords = new HashSet<String>();
	}

	@Override
	public void publish(String address, String keyword) {
		try {
			port = Integer.parseInt(address);
			keywords.add(keyword);
			server = new ServerThread(port);
			server.start();
		} catch (NumberFormatException e) {
			System.out.println("Could not open port "+address);
		}
	}

	public void unpublish(String keyword) {
		keywords.remove(keyword);
		Iterator<Socket> it = servers.keySet().iterator();
		while (it.hasNext()) {
			Socket s = it.next();
			if (servers.get(s).equals(keyword)) {
				try {
					s.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				servers.remove(s);
			}
		}
	}

	private void sendPacket(String keyword, String packet, int num, int max) {
		Iterator<Socket> i = servers.keySet().iterator();
		while (i.hasNext()) {				
			Socket socket = i.next();
			if (!socket.isClosed() && servers.get(socket).equals(keyword)) {
				try {
					ObjectOutputStream out = outs.get(socket);
					out.writeInt(num);
					out.writeInt(max);
					out.writeUTF(keyword);
					out.writeUTF(packet);
					out.writeLong(System.currentTimeMillis());
					out.flush();
				} catch (IOException e) {
					f.pln("Error sending data to "+socket.getInetAddress());
					try {
						socket.close();
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					i.remove();
					e.printStackTrace();
				}
			}
		}
	}
	
	@Override
	public void update(String keyword, String data) {
		if (data != null) { // there is a size limit of 64kb to serializable strings (UTF char = 2b?)
			int max = data.length()/32000; // integer division
			int start = 0;
			int end = Util.min(32000, data.length());
			for (int i = 0; i <= max; i++) {
				sendPacket(keyword, data.substring(start, end), i, max);
				start = end;
				end = Util.min(start+32000, data.length());				
			}
		}
	}

	public boolean hasSubscribers(String keyword) {
		return servers.containsValue(keyword);
	}

	class ServerThread extends Thread {
		ServerSocket ssocket;
		int port;
		boolean running;
		Hashtable<ObjectOutputStream,String> clients; //client socket, keyword  

		public ServerThread(int p) {
			super("SocketTransmitter.ServerThread");
			port = p;
			ssocket = null;
			running = true;
			try {
				ssocket = new ServerSocket(port);
			} catch (IOException e) {
				System.out.println("Could not open server socket on port "+port);
				e.printStackTrace();
			}
		}

		public void halt() {
			try {
				ssocket.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			running = false;
		}

		public void run() {
			while (running) {
				try {
					Socket socket = ssocket.accept();
					System.out.println("New socket "+socket.getInetAddress());					
					ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
					String key = in.readUTF();
					long time = in.readLong();
					System.out.println("Received subscription request from "+socket.getInetAddress()+" word="+key+" time="+time);					
					socket.shutdownInput();
					ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
					if (keywords.contains(key)) {
						out.writeBoolean(true);	
						servers.put(socket, key);
						outs.put(socket, out);
						System.out.println(socket.getInetAddress()+" subscribed");						
					} else {
						out.writeBoolean(false);
						socket.close();
						System.out.println(socket.getInetAddress()+" keyword not recognized");						
					}
					out.flush();
				} catch (IOException e1) {
					f.pln("communicaiton error");
					e1.printStackTrace();
				}			
			}
		}
	}
}
