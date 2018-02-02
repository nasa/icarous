/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universities Space Research Association nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package gov.nasa.luv;

import java.io.BufferedReader;
import java.io.EOFException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.Reader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Set;
import java.util.TreeSet;
import java.util.Vector;
import java.util.concurrent.Semaphore;

import static java.awt.Color.RED;
import static java.lang.Thread.State.TERMINATED;

import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

import static gov.nasa.luv.Constants.END_OF_MESSAGE;

public class LuvSocketServer {

    private boolean exitRequested;
    private Semaphore stopSem;
    private Thread listenThread;
    private Thread serverThread;

    private static LuvSocketServer _the_instance_ = null;

    /**
     * Constructs a server which listens on the specified port and StreamWranglerFactory.
     *
     * @param port port on which this server listens.
     */
    private LuvSocketServer() {
        exitRequested = false;
        stopSem = new Semaphore(0, true); // initially blocked
        listenThread = serverThread = null;
        Runtime.getRuntime().addShutdownHook(new Thread() {
                public void run() {
                    if (LuvSocketServer.serverActive())
                        LuvSocketServer.stopServer();
                }
            });
    }

    public static LuvSocketServer instance() {
        if (_the_instance_ == null)
            _the_instance_ = new LuvSocketServer();
        return _the_instance_;
    }

    public static boolean serverActive() {
        if (_the_instance_ == null)
            return false;
        else
            return _the_instance_.isActive();
    }

    public static boolean serverConnected() {
        if (_the_instance_ == null)
            return false;
        else
            return _the_instance_.isConnected();
    }

    private boolean isActive() {
        if (listenThread == null)
            return false;
        else
            return listenThread.getState() != TERMINATED;
    }

    public boolean isConnected() {
        if (!isActive() || serverThread == null)
            return false;
        else
            // N.B. Slight chance of race condition on server thread exit
            try {
                return serverThread.getState() != TERMINATED;
            }
            catch (NullPointerException n) {
                return false;
            }
    }

    public static void startServer(int port) {
        instance().start(port);
    }

    private void start(int port) {
        if (listenThread != null) {
            StatusMessageHandler.instance().displayErrorMessage(null, "ERROR: attempt to start server when it is already running");
            return;
        }

        if (!portFree(port)) {
            if (Settings.instance().getPortSupplied()) {
                // User chose this port
                StatusMessageHandler.instance().displayErrorMessage(null,
                                                                    "ERROR: port " + port + " is in use, please try another.");
                StatusMessageHandler.instance().showChangeOnPort("Unable to listen on port " + port);
                return;
            }
            // Used default, choose another
            else {
                Vector<Integer> ports = getPortList();
                StatusMessageHandler.instance().displayWarningMessage("Port " + port
                                                                      + " is unavailable, using port " + ports.firstElement()
                                                                      + " instead.",
                                                                      "Port unavailable");
                port = ports.firstElement();
                Settings.instance().setPort(port);
            }
        }

        final int thePort = port; // work around compiler error
        // create a thread which listens for connections
        listenThread = new Thread() {
                public void run() {
                    acceptConnections(thePort);
                }
            };
        listenThread.start();
        StatusMessageHandler.instance().showChangeOnPort("Listening on port " + port);
    }
    
    public static void stopServer() {
        if (!instance().isActive())
            return; // nothing to do
        instance().stop();
    }

    private void stop() {
        StatusMessageHandler.instance().showChangeOnPort("Stopping service on port "
                                                         + Settings.instance().getPort());
        Thread.State s = listenThread.getState();
        if (s != Thread.State.TERMINATED) {
            exitRequested = true;
            try {
                listenThread.join(2000);
            } catch (InterruptedException e) {
            }
        }
        listenThread = null;
    }

    /**
     * Waits for client to connect on a given port.  This method
     * blocks indefinitely and spins off threads for each connection.
     *
     * @param port port on which this server listens.
     */
    public void acceptConnections(int port) {
        try {
            ServerSocket luvSocket = new ServerSocket(port);
            try {
                luvSocket.setSoTimeout(1000); // check for request to quit once a second
            } catch (java.net.SocketException e) {
                StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: while configuring server socket on port " + port);
            }
            while (!exitRequested) {
                try {
                    new ServerThread(luvSocket.accept()).start();
                } catch (java.net.SocketTimeoutException ie) {
                    // do nothing and repeat
                } catch (java.net.SocketException e) {
                    if (e.getMessage().equals("Socket closed")) {
                        StatusMessageHandler.instance().showStatus("Previous " + e.getMessage(), 100);
                        return;
                    }
                    else if (e.getMessage().equals("Permission denied")) {
                        StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: " + e.getMessage()
                                                                            + " Port " + port + " is in use");
                        StatusMessageHandler.instance().showChangeOnPort("Port " + port + " unavailable, please change server port");
                        return;
                    }
                    else {        			
                        StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: socket exception occurred while starting server on port " + port);
                        return;
                    }
                }
            }
            luvSocket.close();
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception occurred while connecting to server using port " + port);
            e.printStackTrace();
        }
        exitRequested = false;
    }

    private class ServerThread
        extends Thread {
        Socket socket;

        public ServerThread(Socket s) {
            socket = s;
        }

        public void run() {
            serverThread = this;
            try {
                wrangle(socket.getInputStream(), socket.getOutputStream());
            } catch (IOException e) {
                StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception in server on port "
                                                                    + socket.getLocalPort());
            }
            serverThread = null;
        }

        /**
         * Wrangles the connection to the Luv server with the specified input and output streams.
         * @param in the InputStream
         * @param out the OutputStream
         */
        private void wrangle(InputStream in, OutputStream out) {
            // set up an XML reader
            XMLReader parser;
            try {
                parser = XMLReaderFactory.createXMLReader();
            } catch (Exception e) {
                StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception occurred while initializing XML reader");
                return;
            }
            parser.setContentHandler(new DispatchHandler());

            InputSource is = new InputSource(new InputStreamWrapper(in));
            boolean quit = false;
            do {
                // if there is input, grab it up
                try {
                    parser.parse(is);
                } // The stream wrapper signals an EOFException when the wrapped stream hits EOF.
                // This would be a good place to notify viewer that execution is complete.
                catch (EOFException e) {
                    quit = true;

                    // TODO: notify whoever is listening of EOF
                    Luv.getLuv().finishedExecutionState(); // FIXME: tell Luv what (EOF), not how
                    break;
                }
                catch (Exception e) {
                    quit = true;
                    StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: while parsing input stream");
                    e.printStackTrace();
                    // TODO: notify whoever is listening that it broke
                    Luv.getLuv().finishedExecutionState();
                    break;
                }

                if (Settings.instance().blocksExec()) {
                    if (ExecutionHandler.instance().shouldBlock())
                        block();

                    // tell Exec it's OK to proceed
                    try {
                        out.write(END_OF_MESSAGE);
                    } catch (Exception e) {
                        StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception while acknowledging Exec Listener message");
                        break;
                    }
                }
            } while (!quit);

            try {
                in.close();
            } catch (Exception f) {
                StatusMessageHandler.instance().displayErrorMessage(f, "ERROR: exception while closing Exec Listener stream");
            }
        }
    }

    /** 
     * Pauses the execution of the Plexil plan by the Universal Executive
     * when directed to by the user. 
     */
    private void block() {
        ExecutionHandler.instance().blocked();
        // wait here for user action
        boolean released = false;
        do {
            try {
                stopSem.acquire();
                released = true;
            }
            catch (InterruptedException e) {
            }
        }
        while (!released);
    }

    public static void resume() {
        if (serverConnected())
            _the_instance_.stopSem.release();
    }
    
    // Utilities

    public static Set<Integer> getPortsInUse() {
        Set<Integer> result = new TreeSet<Integer>();
        String[] cmd = {"list_ports_in_use"};
        try {
            Process p = Runtime.getRuntime().exec(cmd);
            BufferedReader r = new BufferedReader(new InputStreamReader(p.getInputStream()));
            // Port numbers are returned one at a time
            String line = null;
            while ((line = r.readLine()) != null) {
                try {
                    result.add(Integer.parseUnsignedInt(line));
                } catch (NumberFormatException e) {
                    StatusMessageHandler.instance().displayErrorMessage(e, "Error reading ports in use");
                    return new TreeSet<Integer>();
                }
            }
        } catch (IOException e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "Error reading ports in use");
            return new TreeSet<Integer>();
        }
        return result;
    }

    public static Vector<Integer> getPortList() {
        Set<Integer> inUse = getPortsInUse();
        Vector<Integer> portList = new Vector<Integer>(Constants.PORT_MAX - Constants.PORT_MIN + 1);
        int deflt = Settings.instance().getPort(); // current setting
		for (Integer i = Constants.PORT_MIN; i <= Constants.PORT_MAX; i++)
			if (!inUse.contains(i)
                || (serverConnected() && i == deflt))
				portList.add(Integer.valueOf(i));
        return portList;
    }

    /** Return true if port free, false otherwise */
    public static boolean portFree(int port) {
        String[] cmd = {"port_in_use", "-q", ""};
        cmd[2] = Integer.toString(port);
        try {
            return (1 == Runtime.getRuntime().exec(cmd).waitFor());
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception occurred while checking port");
        }
        return false; // caution on side of error
    }


}
