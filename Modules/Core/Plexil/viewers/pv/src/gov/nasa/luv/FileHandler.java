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

import gov.nasa.luv.Luv;

import java.io.FileInputStream;
import java.io.InputStream;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;
import javax.swing.JOptionPane;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

import gov.nasa.luv.PlexilPlanHandler.SimplePlanCatcher;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.Constants.AppType.*;
import static javax.swing.JFileChooser.*;

/**
 * 
 * The FileHandler class handles the searching, opening and loading of Plexil 
 * plans, scripts and libraries.
 */

public class FileHandler 
{
    private static FileHandler _the_instance_ = null;

    // is library found? if so, stop searching for missing libraries
    private static boolean stopSearchForMissingLibs;           
    // directory chooser object       
    private JFileChooser dirChooser;      
    // file chooser object       
    private JFileChooser fileChooser;

    public static FileHandler instance() {
        if (_the_instance_ == null)
            _the_instance_ = new FileHandler();
        return _the_instance_;
    }
    
    /**
     * Constructs a FileHandler.
     */
    private FileHandler() {
        stopSearchForMissingLibs  = false; 
        
        dirChooser = new JFileChooser();
        dirChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        
        fileChooser = new JFileChooser();
        // XML file filter                  
        fileChooser.addChoosableFileFilter(new FileFilter() {
                // accept file?                          
                public boolean accept(File f) {
                    // allow browse directories                             
                    if (f.isDirectory())
                        return true;
                              
                    // allow files with correct extention                              
                    String extension = getExtension(f);
                    Boolean correctExtension = false;
                    if (extension != null) {
                        for (String ext: FILE_EXTENSIONS)
                            if (extension.equals(ext))
                                correctExtension = true;
                    }
                    return correctExtension;
                }

                // get file extension                        
                public String getExtension(File f) {
                    String ext = null;
                    String s = f.getName();
                    int i = s.lastIndexOf('.');
                              
                    if (i > 0 && i < s.length() - 1)
                        ext = s.substring(i+1).toLowerCase();
                              
                    return ext;
                }

                // return descriton                          
                public String getDescription() {
                    return "XML / PLX / PLS";
                }
            });
    }
    
    /**
     * Returns whether or not Luv should stop searching for missing Plexil libraries.
     * @return whether or not Luv should stop searching for missing Plexil libraries
     */
 
    public boolean getStopSearchForMissingLibs()
    {
        return stopSearchForMissingLibs;
    }
    
    /**
     * Sets whether or not Luv should stop searching for missing Plexil libraries.
     * @param value sets whether or not Luv should stop searching for missing Plexil libraries
     */
    public void setStopSearchForMissingLibs(boolean value)
    {
        stopSearchForMissingLibs = value;
    }
    
    /** Finds the Plexil library needed.
     * 
     * @param libraryName the name of the Plexil library to search for
     * @return Plexil library or null if not found
     * @throws java.io.InterruptedIOException
     */

    public Plan searchForLibrary(String libraryName, boolean askUser)
        throws InterruptedIOException {

        // Try known libraries first
        Plan p = RootModel.getLibrary(libraryName);
        if (p != null) {
            File f = p.getPlanFile();
            if (f != null)
                // No file info, have to presume this is the one
                return p;
            else if (!f.isFile())
                // file no longer exists in that location
                RootModel.removeLibrary(libraryName);
            else {
                // Check if current
                Date newLastMod = new Date(f.lastModified());
                Date oldLastMod = p.getLastModified();
                if (oldLastMod == null || newLastMod.after(oldLastMod)) {
                    p = loadLibraryFile(f);
                    if (p != null)
                        return p;
                }
                else
                    return p;
            }
        }

        // Try plan directory next
        String candidateName = libraryName + ".plx";
        File planLoc = Settings.instance().getPlanLocation();
        if (planLoc != null) {
            File libDir = planLoc.getParentFile();
            File candidate = new File(libDir, candidateName);
            if (candidate.isFile()) {
                Plan result = loadLibraryFile(candidate);
                if (result != null)
                    return result;
            }
        }

        // Check user specified library path
        for (File entry : Settings.instance().getLibDirs()) {
            if (entry.isDirectory()) {
                File candidate = new File(entry, candidateName);
                if (candidate.isFile()) {
                    Plan m = loadLibraryFile(candidate);
                    if (m != null)
                        return m;
                }
            }
        }
            
        // Search failed, ask user
        if (askUser) {
            File candidate = unfoundLibrary(libraryName);
            if (candidate != null && candidate.isFile()) {
                return loadLibraryFile(candidate);
            }
        }
        return null;
    }

    private Plan loadLibraryFile(File f) {
        File location = f.getAbsoluteFile();
        Plan result = readPlan(location);
        if (result == null)
            return null;

        result.setLastModified(new Date(location.lastModified()));
        Settings.instance().addLib(location); // FIXME: plan needs this too
        StatusMessageHandler.instance().showStatus("Library "
                                                          + location.toString()
                                                          + " loaded",
                                                          1000);
        RootModel.libraryLoaded(result);
        return result;
    }
          
    /** Selects and loads a Plexil library from the disk. This operates on the global model.
     * @return the Plexil library path or null if not found
     */
    private File chooseLibrary() {
        try {
            File defaultDir = Settings.instance().getPlanLocation().getParentFile();
            fileChooser.setCurrentDirectory(defaultDir);
            if (fileChooser.showDialog(dirChooser, "Open Library") == APPROVE_OPTION)
                return fileChooser.getSelectedFile().getAbsoluteFile();
        } catch(Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception occurred while choosing library");
        }
        return null;
    }
      
    // read plexil plan from disk and create an internal model.
    public static Plan readPlan(File file) {
        try {
            Plan result = parseXml(new FileInputStream(file));
            File f = file.getAbsoluteFile();
            result.setPlanFile(f);
            result.setLastModified(new Date(f.lastModified()));
            return result;
        } catch(Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: while loading: " + file.getName());
        }
        return null;
    }

    // parse a plan from an XML stream
    private static Plan parseXml(InputStream input) {
        SimplePlanCatcher c = new SimplePlanCatcher();
        PlexilPlanHandler ch =
            new PlexilPlanHandler(c);
        try {
            InputSource is = new InputSource(input);
            XMLReader parser = XMLReaderFactory.createXMLReader();
            parser.setContentHandler(ch);
            parser.parse(is);          
        }
        catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: exception occurred while parsing XML message");
            return null;
        }
        return c.plan;
    }

    private File unfoundLibrary(String callName)
        throws InterruptedIOException {
        boolean retry = true;
        File libFile = null;
        do {
            // if we didn't make the link, ask user for library
            Object[] options = 
                {
                    "I will locate library",
                    "Skip library"
                };

            // show the options
            StatusMessageHandler.instance().showStatus("Unable to locate the \"" + callName + "\" library", 1000);
            int result = JOptionPane.showOptionDialog(Luv.getLuv(),
                                                      "Unable to locate the \"" + callName + "\" library.\n\n" +
                                                      "What do you want to do?\n\n",
                                                      "Load the library?",
                                                      JOptionPane.YES_NO_CANCEL_OPTION,
                                                      JOptionPane.WARNING_MESSAGE,
                                                      null,
                                                      options,
                                                      options[0]);

            // process the results
            switch (result) {
                // try to load the library and retry the link
            case 0:
                libFile = chooseLibrary(); // *** FIXME ***
                retry = false;
                break;

                // if the user doesn't want to load any libraries,
                // halt the link operation now
            case 1:
                retry = false;
                stopSearchForMissingLibs = true;
			
                break;
            }
        }
        while (retry); 
          
        return libFile;
    }
    
}
