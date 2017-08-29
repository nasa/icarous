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

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;

import java.lang.reflect.InvocationTargetException;

import java.util.concurrent.LinkedBlockingQueue;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.border.EmptyBorder;

import static gov.nasa.luv.Constants.UNKNOWN;

/**
 * The StatusMessageHandler class provides methods to display status messages 
 * to the status bar at the bottom of the Luv application and to the Debug Window.
 */

public class StatusMessageHandler
{   
    private static StatusMessageHandler _the_instance_ = null;

    private static JLabel statusBar;
    private static JLabel portBar;
    private static final long DEFAULT_WAIT = 0;

    private boolean abortAutoClear;

    public static StatusMessageHandler instance() {
        if (_the_instance_ == null)
            _the_instance_ = new StatusMessageHandler();
        return _the_instance_;
    }

    /**
     * Constructs a StatusMessageHandler.
     */
    private StatusMessageHandler() {
        statusBar = new JLabel();
        statusBar.setFont(statusBar.getFont().deriveFont(Font.PLAIN, 12.0f));
        statusBar.setBorder(new EmptyBorder(2, 2, 2, 2));        

        portBar = new JLabel();
        portBar.setFont(portBar.getFont().deriveFont(Font.PLAIN, 12.0f));
        portBar.setBorder(new EmptyBorder(2, 2, 2, 2));
    }

    public JLabel getStatusBar() {
        return statusBar;
    }

    public JLabel getPortBar() {
        return portBar;
    }

    private void displayMessage(Runnable s) {
        if (javax.swing.SwingUtilities.isEventDispatchThread())
            try {
                s.run();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        else 
            try {
                javax.swing.SwingUtilities.invokeAndWait(s);
            }
            catch (InterruptedException i) {
            }
            catch (InvocationTargetException e) {
                e.printStackTrace();
            }
    }

    // Exactly one caller, in class Luv.

    public void showIdlePortMessage() {   	
        displayMessage(new StatusMessage(portBar, "Connected", Color.GREEN, true, DEFAULT_WAIT));
    }
     
    /**
     * Displays the specified message to the port status bar and not the Debug Window.
     * 
     * @param message the message to be displayed
     */
    public void showChangeOnPort(String message) {
    	showChangeOnPort(message, Color.BLACK);
    }

    /**
     * Adds the specified message and color to port status bar
     * (Color only apply to port status bar, not Debug Window)
     * 
     * @param message the message to display
     * @param color the color the message will display in the port status bar
     */
    public void showChangeOnPort(String message, Color color) {
        displayMessage(new StatusMessage(portBar, message, color, DEFAULT_WAIT));
    }
    
    /**
     * Adds the specified message to status bar and debug Window with default
     * color and time. (Color and time only apply to status bar, not Debug Window)
     * 
     * @param message the message to display
     */
    public void showStatus(String message)
    {
         showStatus(message, Color.BLACK, 0);
    }

    /**
     * Adds the specified message and time to status bar and debug Window with default
     * color. (Color and time only apply to status bar, not Debug Window)
     * 
     * @param message the message to display
     * @param autoClearTime the amount of time the message will display
     */
    public void showStatus(String message, long autoClearTime)
    {
        showStatus(message, Color.BLACK, autoClearTime);
    }

    /**
     * Adds the specified message and color to status bar and debug Window with default
     * time. (Color and time only apply to status bar, not Debug Window)
     * 
     * @param message the message to display
     * @param color the color the message will display in the status bar
     */
    public void showStatus(String message, Color color)
    {
       showStatus(message, color, 0);
    }

    /**
     * Adds the specified message, color and time to status bar and debug Window.
     * (Color and time only apply to status bar, not Debug Window)
     * 
     * @param message the message to display
     * @param color the color the message will display in the status bar
     * @param autoClearTime the amount of time the message will display
     */
    public void showStatus(String message, Color color, final long autoClearTime)
    {
        displayMessage(new StatusMessage(statusBar, message, color, autoClearTime));
    }
      
    /**
     * Displays a consistently formatted error message in a Dialog Box and 
     * again to the Debug Window.
     * 
     * @param e the exception that triggered the error message, can be null
     * @param errorMessage the message to be displayed with the error
     */
    public void displayErrorMessage(Exception e, String errorMessage) {
        displayErrorMessage(Luv.getLuv(), e, errorMessage);
    }
      
    /**
     * Displays a consistently formatted error message in a Dialog Box and 
     * again to the Debug Window.
     * 
     * @param c The display component reporting the error.
     * @param e the exception that triggered the error message, can be null
     * @param errorMessage the message to be displayed with the error
     */
    public void displayErrorMessage(Component c, Exception e, String errorMessage) {
        displayMessage(new ErrorMessage(c, e, errorMessage));
    }
    
    /**
     * Displays a consistently formatted error information or message in a Dialog Box.
     * 
     * @param msg the message to be displayed 
     */
    public void displayWarningMessage(String msg, String title)
    {
        displayWarningMessage(Luv.getLuv(), msg, title);
    }
    
    /**
     * Displays a consistently formatted error information or message in a Dialog Box.
     * 
     * @param component the component to return focus to when the dialog is dismissed
     * @param msg the message to be displayed 
     * @param title Title to display on the warning dialog.
     */
    public void displayWarningMessage(Component component, String msg, String title)
    {
        displayMessage(new DialogMessage(JOptionPane.WARNING_MESSAGE,
                                         msg,
                                         title,
                                         component));
    }

    //
    // Helper classes
    //

    private class StatusMessage
        implements Runnable {
        public JLabel display;
        public String message;
        public Color color;
        public long autoClearTime;
        public boolean idleMessage;

        /**
         * Constructs a StatusMessage with the specified message, color and 
         * amount of time it will be displayed.
         * 
         * @param message the message displayed
         * @param color the color the message will be displayed in
         * @param autoClearTime the amount of time message will be displayed
         */
        public StatusMessage(JLabel display, String message, Color color, long autoClearTime) {
            this.display = display;
            this.message = message;
            this.color = color;
            this.autoClearTime = autoClearTime;
            idleMessage = false;
        }

        /**
         * Constructs a StatusMessage with the specified message, color, idleStatus, and 
         * amount of time it will be displayed.
         * 
         * @param message the message displayed
         * @param color the color the message will be displayed in
         * @param autoClearTime the amount of time message will be displayed
         */
        public StatusMessage(JLabel display, String message, Color color, Boolean idle, long autoClearTime) {
            this.display = display;
            this.message = message;
            this.color = color;
            this.autoClearTime = autoClearTime;
            this.idleMessage = idle;
        }    

        // Runs in AWT event handler context
        public void run() {
            display.setForeground(color);
            display.setText(message);

            if (!message.isEmpty())
                System.out.println("STATUS: " + message);

            // if auto clear requested set a timer for that
            if (autoClearTime > 0 && !idleMessage) {
                // TODO
            }
        }

    }

    // Helper class for both warning and info dialogs.
    private class DialogMessage
        implements Runnable {
        private Component component;
        private String message;
        private String title;
        private int msgType;

        public DialogMessage(int kind, String msg, String titl, Component c) {
            msgType = kind;
            message = msg;
            title = titl;
            component = c;
        }

        public void run() {
            JOptionPane.showMessageDialog(component,
                                          message,
                                          title,
                                          msgType);
            
            System.out.println((msgType == JOptionPane.WARNING_MESSAGE
                                ? "WARNING: "
                                : "INFO:  ")
                               + message);
        }
    }

    private class ErrorMessage
        implements Runnable {

        private Component component;
        private Throwable exception;
        private String message; 

        public ErrorMessage(Component c, Exception e, String msg) {
            component = c;
            exception = e;
            message = msg;
        }

        public void run() {
            if (exception != null) {
                JOptionPane.showMessageDialog(component, 
                                              message + ".\nPlease see Viewer console.", 
                                              "Error", 
                                              JOptionPane.ERROR_MESSAGE);
                System.out.println(message + "\n" + exception.getMessage());
                exception.printStackTrace(System.out);
                // make console visible
                Luv.getLuv().setVisible(true);
                Luv.getLuv().toFront();
            }
            else {
                JOptionPane.showMessageDialog(component,
                                              message, 
                                              "Error", 
                                              JOptionPane.ERROR_MESSAGE);

                System.out.println(message);
            }
        }
    }

}
