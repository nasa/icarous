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
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Toolkit;
import java.io.File;
import java.net.URL;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.ImageIcon;

import static gov.nasa.luv.PlexilSchema.*;

/**
 * The Constants class provides a repository for global constants in the 
 * Luv application.
 */

public class Constants
{     
    public static final String PLEXIL_VERSION = "Version 4.5a\n";
    public static final String PLEXIL_COPYRIGHT = "Copyright 2006-2015 Universities Space Research Association\n";
    public static final String PLEXIL_WEBSITE = "plexil.sourceforge.net";
      
    ////////// Info stored in the Luv application properties file  //////////

    /** the number of characters the viewer will allow arrays to display for arrays. */ 
    public static final String    PROP_ARRAY_MAX_CHARS = "plexil.viewer.array_max_chars";

    /** the list of Plexil nodes that hidden by the HideOrShowWindow. */ 
    public static final String    PROP_HIDE_SHOW_LIST = "plexil.viewer.hide_show_list";

    /** the list of Plexil nodes previously searched for in the FindWindow. */ 
    public static final String    PROP_SEARCH_LIST = "plexil.viewer.search_list";

    /** the location of the Luv application window. */ 
    public static final String    PROP_WIN_LOC        = "plexil.viewer.window.location";

    /** the size of the Luv application window. */ 
    public static final String    PROP_WIN_SIZE       = "plexil.viewer.window.size";

    /** the background color of the Luv application window. */
    public static final String    PROP_WIN_BCLR       = "plexil.viewer.window.background";

    /** the foreground color of the Luv application window. */
    public static final String    PROP_WIN_FCLR       = "plexil.viewer.window.foreground";

    /** the location of the NodeInfoWindow. */
    public static final String    PROP_NODEINFOWIN_LOC      = "plexil.viewer.niwindow.location";

    /** the size of the NodeInfoWindow. */ 
    public static final String    PROP_NODEINFOWIN_SIZE     = "plexil.viewer.niwindow.size";

    /** the location of the FindWindow. */
    public static final String    PROP_FINDWIN_LOC          = "plexil.viewer.fwindow.location";

    /** the size of the FindWindow. */ 
    public static final String    PROP_FINDWIN_SIZE         = "plexil.viewer.fwindow.size";

    /** the location of the HideOrShowWindow. */
    public static final String    PROP_HIDESHOWWIN_LOC      = "plexil.viewer.hswindow.location";

    /** the size of the HideOrShowWindow. */ 
    public static final String    PROP_HIDESHOWWIN_SIZE     = "plexil.viewer.hswindow.size";

    /** the location of the DebugCFGWindow. */
    public static final String    PROP_CFGWIN_LOC           = "plexil.viewer.cfgwindow.location";

    /** the size of the DebugCFGWindow. */ 
    public static final String    PROP_CFGWIN_SIZE          = "plexil.viewer.cfgwindow.size";


    //
    // Default values
    //

    /** Default number of characters the viewer will allow arrays to display for arrays. */ 
    public static final int       PROP_ARRAY_MAX_CHARS_DEF = 40;

    /** Default location of the Luv console window. */ 
    public static final Point     PROP_WIN_LOC_DEF    = new Point(100, 100);

    /** Default size of the Luv console window. */ 
    public static final Dimension PROP_WIN_SIZE_DEF   = new Dimension(800, 500);

    /** Default background color of the Luv application window. */ 
    public static final Color     PROP_WIN_BCLR_DEF   = Color.WHITE;

    /** Default background color of the Luv application window. */ 
    public static final Color     PROP_WIN_FCLR_DEF   = Color.BLACK;

    /** Default location of the NodeInfoWindow. */
    public static final Point     PROP_NODEINFOWIN_LOC_DEF  = new Point(300, 300);

    /** Default size of the NodeInfoWindow. */
    public static final Dimension PROP_NODEINFOWIN_SIZE_DEF = new Dimension(900, 300);     

    /** Default location of the FindWindow. */
    public static final Point     PROP_FINDWIN_LOC_DEF      = new Point(500, 300);

    /** Default size of the FindWindow. */
    public static final Dimension PROP_FINDWIN_SIZE_DEF     = new Dimension(400, 100);     

    /** Default location of the HideOrShowWindow. */
    public static final Point     PROP_HIDESHOWWIN_LOC_DEF  = new Point(300, 300);

    /** Default size of the HideOrShowWindow. */
    public static final Dimension PROP_HIDESHOWWIN_SIZE_DEF = new Dimension(900, 700); 

    /** Default location of the DebugCFGWindow. */
    public static final Point     PROP_CFGWIN_LOC_DEF       = new Point(300, 300);

    /** Default size of the DebugCFGWindow. */
    public static final Dimension PROP_CFGWIN_SIZE_DEF      = new Dimension(900, 700);

    /** Default application type. */
    public static final AppType   APP_TYPE_DEF = AppType.NO_APP;

    /** Default exec-blocks value. */
    public static final boolean   PROP_BLOCKS_EXEC_DEF = false;

    /** Default check-plan value. */
    public static final boolean   PROP_CHECK_PLAN_DEF = false;

    /** Default auto-run value. */
    public static final boolean   PROP_AUTO_RUN_DEF = false;

      
    //////////// Port configuration information /////////////
    /** Represents the smallest port value. */      
    public static final int    PORT_MIN = 49100; // was 49000

    /** Represents the maximum port value. */      
    public static final int    PORT_MAX = 49150;
      
    /** Default server port number */
    public static final int       NET_SERVER_PORT_DEF  = 49100;

    //////////// Executive configuration information /////////////
    public enum AppType {
        NO_APP,
        EXTERNAL_APP,
        PLEXIL_EXEC,
        PLEXIL_SIM,
        PLEXIL_TEST,
        USER_SPECIFIED
    }
      
    //////////// Various constant info used within this program /////////////

    /** Represents the "PLEXIL_HOME" environment variable. */
    public static final File      PLEXIL_HOME = new File(System.getenv("PLEXIL_HOME"));

    /** Location of the PLEXIL shell scripts. */
    public static final File      PLEXIL_SCRIPTS_DIR = new File(PLEXIL_HOME, "scripts");

    public static final File      PLEXIL_EXAMPLES_DIR = new File(PLEXIL_HOME, "examples");
      
    /** Represents the python script that is used to create a complete list of debug flag list. */ 
    public static final String    PYTHON_SCRIPT = (new File(PLEXIL_SCRIPTS_DIR,
                                                            "debugFlagCollector.py")).getAbsolutePath();
      
    /** Represents the complete list of debug flags. */ 
    public static final String    COMPLETE_FLAG_LIST = PLEXIL_HOME +
        System.getProperty("file.separator") +
        "doc" + 
        System.getProperty("file.separator") +
        "CompleteDebugFlags.cfg";
      
    /** Represents the user defined list of debug flags. */
    public static final String    DEBUG_CFG_FILE = PLEXIL_HOME +
        System.getProperty("file.separator") +
        "viewers" +
        System.getProperty("file.separator") +
        "pv" + 
        System.getProperty("file.separator") +
        "Debug.cfg";

    /** Represents the location of the Plexil node icon files. */
    public static final String    ICONS_DIR = "resources" + 
        System.getProperty("file.separator") + 
        "icons" + 
        System.getProperty("file.separator");
      
    /** Plexil node icon file names */
    public static final String    IF_ICO  = "ACTION_IF.gif";
    public static final String    THEN_ICO  = "ACTION_THEN.gif";
    public static final String    ELSE_ICO  = "ACTION_ELSE.gif";
    public static final String    WHILE_ICO  = "ACTION_WHILE.gif";
    public static final String    TRY_ICO  = "ACTION_TRY.gif";
    public static final String    FOR_ICO  = "ACTION_FOR.gif";
    public static final String    SEQ_ICO  = "ACTION_SEQ.gif";
    public static final String    UNCHKD_SEQ_ICO  = "ACTION_UNCHECKED_SEQ.gif";
    public static final String    CONCURRENCE_ICO  = "ACTION_CONCURRENCE.gif";

    public static final String    THEN_LIST_ICO  = "THEN_NODE_LIST.gif";
    public static final String    THEN_ASSN_ICO  = "THEN_NODE_A.gif";
    public static final String    THEN_CMD_ICO  = "THEN_NODE_C.gif";
    public static final String    THEN_EMPTY_ICO  = "THEN_NODE_E.gif";
    public static final String    THEN_LIB_ICO  = "THEN_NODE_L.gif";
    public static final String    THEN_UPDATE_ICO  = "THEN_NODE_U.gif";

    public static final String    THEN_IF_ICO  = "THEN_ACTION_IF.gif";
    public static final String    THEN_WHILE_ICO  = "THEN_ACTION_W.gif";
    public static final String    THEN_FOR_ICO  = "THEN_ACTION_FOR.gif";
    public static final String    THEN_TRY_ICO  = "THEN_ACTION_TRY.gif";
    public static final String    THEN_SEQ_ICO  = "THEN_ACTION_SEQ.gif";
    public static final String    THEN_UNCHKD_SEQ_ICO  = "THEN_ACTION_UNCHKD_SEQ.gif";
    public static final String    THEN_CONCURRENCE_ICO  = "THEN_ACTION_CONC.gif";

    public static final String    ELSE_LIST_ICO  = "ELSE_NODE_LIST.gif";
    public static final String    ELSE_ASSN_ICO  = "ELSE_NODE_A.gif";
    public static final String    ELSE_CMD_ICO  = "ELSE_NODE_C.gif";
    public static final String    ELSE_EMPTY_ICO  = "ELSE_NODE_E.gif";
    public static final String    ELSE_LIB_ICO  = "ELSE_NODE_L.gif";
    public static final String    ELSE_UPDATE_ICO  = "ELSE_NODE_U.gif";

    public static final String    ELSE_IF_ICO  = "ELSE_ACTION_IF.gif";
    public static final String    ELSE_WHILE_ICO  = "ELSE_ACTION_W.gif";
    public static final String    ELSE_FOR_ICO  = "ELSE_ACTION_FOR.gif";
    public static final String    ELSE_TRY_ICO  = "ELSE_ACTION_TRY.gif";
    public static final String    ELSE_SEQ_ICO  = "ELSE_ACTION_SEQ.gif";
    public static final String    ELSE_UNCHKD_SEQ_ICO  = "ELSE_ACTION_UNCHKD_SEQ.gif";
    public static final String    ELSE_CONCURRENCE_ICO  = "ELSE_ACTION_CONC.gif";
      
    public static final String    ABOUT_SCREEN_ICO  = "LUV_plexil-logo.jpg";//LUV_ICON.gif
    public static final String    START_SCREEN_ICO  = "LUV_SPLASH_SCREEN.jpg";//LUV_SPLASH_SCREEN.gif
    public static final String    NODELIST_ICO_NAME = "NODE_LIST.gif";
    public static final String    COMMAND_ICO_NAME  = "NODE_COMMAND.gif";
    public static final String    ASSN_ICO_NAME     = "NODE_ASSN.gif";
    public static final String    EMPTY_ICO_NAME    = "NODE_EMPTY.gif";
    public static final String    UPDATE_ICO_NAME   = "NODE_UPDATE.gif";
    public static final String    LIBCALL_ICO_NAME  = "NODE_LIB_CALL.gif";    
  
    private static Map<String, ImageIcon> iconLut =
        new HashMap<String, ImageIcon>() {
            {
                add(NODELIST,        NODELIST_ICO_NAME);
                add(COMMAND,         COMMAND_ICO_NAME);
                add(ASSIGNMENT,      ASSN_ICO_NAME);
                add(EMPTY,           EMPTY_ICO_NAME);
                add(UPDATE,          UPDATE_ICO_NAME);
                add(LIBRARYNODECALL, LIBCALL_ICO_NAME);
                add(START_LOGO,      START_SCREEN_ICO);
                add(ABOUT_LOGO,      ABOUT_SCREEN_ICO);

                add(IF,      IF_ICO);
                add(THEN, THEN_ICO);
                add(ELSE, ELSE_ICO);
                add(WHILE, WHILE_ICO);
                add(FOR, FOR_ICO);
                add(TRY, TRY_ICO);
                add(SEQ, SEQ_ICO);
                add(UNCHKD_SEQ, UNCHKD_SEQ_ICO);
                add(CONCURRENCE, CONCURRENCE_ICO);

                add(THEN_EMPTY, THEN_EMPTY_ICO);
                add(THEN_CMD, THEN_CMD_ICO);
                add(THEN_ASSN, THEN_ASSN_ICO);
                add(THEN_LIST, THEN_LIST_ICO);
                add(THEN_LIB, THEN_LIB_ICO);
                add(THEN_UPDATE, THEN_UPDATE_ICO);

                add(THEN_IF, THEN_IF_ICO);
                add(THEN_WHILE, THEN_WHILE_ICO);
                add(THEN_FOR, THEN_FOR_ICO);
                add(THEN_TRY, THEN_TRY_ICO);
                add(THEN_SEQ, THEN_SEQ_ICO);
                add(THEN_UNCHKD_SEQ, THEN_UNCHKD_SEQ_ICO);
                add(THEN_CONCURRENCE, THEN_CONCURRENCE_ICO);

                add(ELSE_EMPTY, ELSE_EMPTY_ICO);
                add(ELSE_CMD, ELSE_CMD_ICO);
                add(ELSE_ASSN, ELSE_ASSN_ICO);
                add(ELSE_LIST, ELSE_LIST_ICO);
                add(ELSE_LIB, ELSE_LIB_ICO);
                add(ELSE_UPDATE, ELSE_UPDATE_ICO);

                add(ELSE_IF, ELSE_IF_ICO);
                add(ELSE_WHILE, ELSE_WHILE_ICO);
                add(ELSE_FOR, ELSE_FOR_ICO);
                add(ELSE_TRY, ELSE_TRY_ICO);
                add(ELSE_SEQ, ELSE_SEQ_ICO);
                add(ELSE_UNCHKD_SEQ, ELSE_UNCHKD_SEQ_ICO);
                add(ELSE_CONCURRENCE, ELSE_CONCURRENCE_ICO);
            }
         
            public void add(String tag, String iconName)
            {
                put(tag, loadImage(iconName));
            }
        };

    /**
     * Returns the matching ImageIcon of the specified icon name.
     * @param icon the icon name on which to find a matching ImageIcon
     * @return the ImageIcon of the specified icon name
     */
    public static ImageIcon getIcon(String icon)
    {
        return iconLut.get(icon);
    }

    public static URL getIconLocation(String iconFileName) {
        return ClassLoader.getSystemResource(ICONS_DIR + iconFileName);
    }
      
    public static ImageIcon loadImage(String name)
    {         
        return new ImageIcon(Toolkit.getDefaultToolkit().getImage(getIconLocation(name)));
    }

    public static final String UNKNOWN = "[unknown_value]";
      
    /** Indicates that a Plexil node has a breakpoint set on it. */
    public static final String NODE_ENABLED_BREAKPOINTS = "model.breakpoint.enabled";
    /** Indicates that a Plexil node does not have a breakpoint set on it. */
    public static final String NODE_DISABLED_BREAKPOINTS = "model.breakpoint.disabled";

    /** Holds the Luv application row color pattern. */
    public static final Vector<Color> TREE_TABLE_ROW_COLORS = new Vector<Color>()
                                                              {
                                                                  {
                                                                      add(Color.WHITE);
                                                                      add(new Color(245, 245, 255));
                                                                      add(Color.WHITE);
                                                                      add(new Color(255, 246, 246));
                                                                  }
        };

    // colors
    private static HashMap<String, Color> colorLut = new HashMap<String, Color>()
                                                     {
                                                         {
                                                             // model colors
                                                             put(NODE_ENABLED_BREAKPOINTS,          Color.RED);
                                                             put(NODE_DISABLED_BREAKPOINTS,         Color.ORANGE);
                                                         }
        };

    /**
     * Returns the matching color of the specified Plexil node State, Outcome, 
     * Failure Type or if a breakpoint was set on it.
     * 
     * @param value the type of Plexil node State, Outcome, Failure Type or Breakpoint status
     * @return the matching color
     */
    public static Color lookupColor(String value)
    {
        return colorLut.get(value);
    }

    /** Represents an end of message in an input stream. */      
    public static final int       END_OF_MESSAGE = 4;

    /** Used as a marker for formatting Plexil node condition, local variable and action information. */
    public static final String SEPARATOR = "_Separator_";

    /** A collection of all the default file extensions visible to the Luv application. */
    public static final String[] FILE_EXTENSIONS = 
    {
        "xml",
        "plx",
        "psx",
        "txt",
        "pst",
        "pls",
    };
      
    /** Represents an empty Plexil script. */ 
    public static final String DEFAULT_SCRIPT_NAME = "empty.psx";
    /** Represents a default UE config. */      
    public static final String DEFAULT_CONFIG_NAME = "dummy-config.xml";
    /** Represents a default UE path. */
    public static final File DEFAULT_CONFIG_PATH = new File(PLEXIL_HOME, "examples");

    public static final String NODETYPE_ATTR_PLX = "NodeTypePLX";

    public static final String    START_LOGO        = "Clear Screen";
    public static final String    ABOUT_LOGO        = "About Logo";

    /* Extended Plexil elements */

    public static final String THEN_LIST  = "ThenNodeList";
    public static final String THEN_ASSN  = "ThenAssignment";
    public static final String THEN_CMD  = "ThenCommand";
    public static final String THEN_EMPTY  = "ThenEmpty";
    public static final String THEN_LIB  = "ThenLibraryNodeCall";
    public static final String THEN_UPDATE  = "ThenUpdate";

    public static final String THEN_IF  = "ThenIf";
    public static final String THEN_WHILE  = "ThenWhile";
    public static final String THEN_FOR  = "ThenFor";
    public static final String THEN_TRY  = "ThenTry";
    public static final String THEN_SEQ  = "ThenSequence";
    public static final String THEN_UNCHKD_SEQ  = "ThenUncheckedSequence";
    public static final String THEN_CONCURRENCE  = "ThenConcurrence";

    public static final String ELSE_LIST  = "ElseNodeList";
    public static final String ELSE_ASSN  = "ElseAssignment";
    public static final String ELSE_CMD  = "ElseCommand";
    public static final String ELSE_EMPTY  = "ElseEmpty";
    public static final String ELSE_LIB  = "ElseLibraryNodeCall";
    public static final String ELSE_UPDATE  = "ElseUpdate";

    public static final String ELSE_IF  = "ElseIf";
    public static final String ELSE_WHILE  = "ElseWhile";
    public static final String ELSE_FOR  = "ElseFor";
    public static final String ELSE_TRY  = "ElseTry";
    public static final String ELSE_SEQ  = "ElseSequence";
    public static final String ELSE_UNCHKD_SEQ  = "ElseUncheckedSequence";
    public static final String ELSE_CONCURRENCE = "ElseConcurrence";

}
