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

//
// TODO:
//  - use default properties (see Properties javadoc)
//

package gov.nasa.luv;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.Constants.AppType.*;

import java.awt.Color;
import java.awt.Point;
import java.awt.Dimension;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Vector;
import java.util.Base64;
import java.util.Collection;
import java.util.List;
import java.util.Properties;

public class Settings {

    /** Represents the location of the Luv application properties file. */      
    private static final File PROPERTIES_FILE_LOCATION = new File(System.getProperty("user.home"), ".luv");

    private static final String HELP_TEXT  =
        "Usage:\n"
        + "\tplexil [options]\nOptions:\n"
        + " -h  | --help\n\t displays this message\n"
        + "\nOptions to specify which executive is used:\n"
        + " -E  | -external\n\t Do not run a PLEXIL exec, but monitor an external exec\n"
        + " -S  | -plexilsim\n\t Use the Universal Exec with the StandAlone Simulator\n"
        + " -T  | -plexiltest\n\t Use the TestExec (interleaves execution & simulation in one process) (default)\n"
        + " -U  | -plexilexec\n\t Use the Universal Exec\n"
        + "\nOptions to specify which files are used:\n"
        + " -p  | -plan <plan>\n\t specify PLEXIL plan\n"
        + " -s  | -script <scrpt>\n\t specify simulation script\n"
        + " -c  | -config <cfg>\n\t specify Universal Exec interface configuration file\n"
        + " -l  | -library <name>\n\t specifies a PLEXIL library (.plx file)\n"
        + "\t (option may be repeated)\n"
        + " -L  | -libraryPath <directory name>\n\t specifies a directory for finding PLEXIL library files\n"
        + "\t (option may be repeated)\n"
        + "\nOther options:\n"
        + " -a  | -autorun\n\t start execution immediately\n"
        + " -b  | -blocking\n\t enables breakpoints in viewer\n"
        + " -ch | -check\n\t runs the PLEXIL static checker prior to executing plan\n"
        + " -n  | -port <number>\n\t TCP port number for viewer-exec comms (default " + Integer.toString(NET_SERVER_PORT_DEF) + ")\n"
        + " -d  | -debug <file name>\n\t debug configuration file (default is Debug.cfg)\n"
        + "\nAll command line options are strictly optional, and may be overridden inside the Plexil Viewer."
        ;

    // Private constants used as property names
    private static final String    PROP_APP_TYPE         = "plexil.application_type";
    private static final String    PROP_NET_SERVER_PORT  = "plexil.server_port";
    private static final String    PROP_PLAN_FILE        = "plexil.plan";
    private static final String    PROP_LIB_DIRS         = "plexil.libDirs";
    private static final String    PROP_LIB_NAMES        = "plexil.libs";
    private static final String    PROP_CONFIG_FILE      = "plexil.ue.config";
    private static final String    PROP_SCRIPT_FILE      = "plexil.sim.script";
    private static final String    PROP_DEBUG_FILE       = "plexil.debug";
    private static final String    PROP_BLOCKS_EXEC      = "plexil.viewer.blocks_exec";
    private static final String    PROP_CHECK_PLAN       = "plexil.checkPlan";
    private static final String    PROP_AUTO_RUN         = "plexil.autoRun";

    private static Settings _the_instance_ = null;

    // Persistent storage
    private File m_file;
    private Properties m_properties;

    // Particular settings
    private int serverPort;

    // Tracking supplied options, to distinguish them from saved values
    private boolean autoRunSupplied;
    private boolean blockSupplied;
    private boolean checkSupplied;
    private boolean configSupplied;
    private boolean debugSupplied;
    private boolean librariesSupplied;
    private boolean modeSupplied;
    private boolean pathSupplied;
    private boolean planSupplied;
    private boolean portSupplied;
    private boolean scriptSupplied;

    // Local caches
    private Vector<File> libDirs;
    private Vector<File> libs;


    public static Settings instance() {
        if (_the_instance_ == null)
            _the_instance_ = new Settings();
        return _the_instance_;
    }

    protected Settings() {
        m_properties = new java.util.Properties(getDefaultDefaults());
        libDirs = new Vector<File>();
        libs = new Vector<File>();
        autoRunSupplied = false;
        blockSupplied = false;
        checkSupplied = false;
        configSupplied = false;
        debugSupplied = false;
        librariesSupplied = false;
        modeSupplied = false;
        pathSupplied = false;
        planSupplied = false;
        portSupplied = false;
        scriptSupplied = false;

        // set default defaults
        m_file = PROPERTIES_FILE_LOCATION;
    }
    
    private final Properties getDefaultDefaults() {
        Properties defaults = new Properties();

        defaults.setProperty(PROP_APP_TYPE, APP_TYPE_DEF.toString()); // FIXME
        defaults.setProperty(PROP_NET_SERVER_PORT, Integer.toString(NET_SERVER_PORT_DEF));
        defaults.setProperty(PROP_BLOCKS_EXEC, Boolean.toString(PROP_BLOCKS_EXEC_DEF));
        defaults.setProperty(PROP_CHECK_PLAN, Boolean.toString(PROP_CHECK_PLAN_DEF));
        defaults.setProperty(PROP_AUTO_RUN, Boolean.toString(PROP_AUTO_RUN_DEF));

        defaults.setProperty(PROP_ARRAY_MAX_CHARS, Integer.toString(PROP_ARRAY_MAX_CHARS_DEF));

        defaults.setProperty(PROP_WIN_LOC, serialize(PROP_WIN_LOC_DEF));
        defaults.setProperty(PROP_WIN_SIZE, serialize(PROP_WIN_SIZE_DEF));
        defaults.setProperty(PROP_WIN_BCLR, serialize(PROP_WIN_BCLR_DEF));

        defaults.setProperty(PROP_NODEINFOWIN_LOC, serialize(PROP_NODEINFOWIN_LOC_DEF));
        defaults.setProperty(PROP_NODEINFOWIN_SIZE, serialize(PROP_NODEINFOWIN_SIZE_DEF));
        defaults.setProperty(PROP_FINDWIN_LOC, serialize(PROP_FINDWIN_LOC_DEF));
        defaults.setProperty(PROP_FINDWIN_SIZE, serialize(PROP_FINDWIN_SIZE_DEF));
        defaults.setProperty(PROP_HIDESHOWWIN_LOC, serialize(PROP_HIDESHOWWIN_LOC_DEF));
        defaults.setProperty(PROP_HIDESHOWWIN_SIZE, serialize(PROP_HIDESHOWWIN_SIZE_DEF));
        defaults.setProperty(PROP_CFGWIN_LOC, serialize(PROP_CFGWIN_LOC_DEF));
        defaults.setProperty(PROP_CFGWIN_SIZE, serialize(PROP_CFGWIN_SIZE_DEF));

        return defaults;
    }

    public void setFile(File f) {
        m_file = f;
    }

    public boolean parseCommandOptions(String[] opts) {
        // Reset option-supplied flags
        autoRunSupplied = false;
        blockSupplied = false;
        checkSupplied = false;
        configSupplied = false;
        debugSupplied = false;
        librariesSupplied = false;
        modeSupplied = false;
        pathSupplied = false;
        planSupplied = false;
        portSupplied = false;
        scriptSupplied = false;

        Vector<File> specdLibDirs = new Vector<File>();
        Vector<File> specdLibNames = new Vector<File>();
        File plan = null;
        File script = null;
        File config = null;
        File debug = null;
        int port = 0;
        boolean block = false;
        boolean check = false;
        boolean autoRun = false;

        AppType specdMode = PLEXIL_TEST;
        for (int i = 0; i < opts.length; ++i) {
            String opt = opts[i];
            try {
                if (opt.equals("-a") || opt.equals("-autorun")) {
                    autoRunSupplied = true;
                    autoRun = true;
                }
                else if (opt.equals("-b") || opt.equals("-blocking")) {
                    block = true;
                    blockSupplied = true;
                }
                else if (opt.equals("-c") || opt.equals("-config")) {
                    config = new File(opts[++i]);
                    configSupplied = true;
                }
                else if (opt.equals("-ch") || opt.equals("-check")) {
                    check = true;
                    checkSupplied = true;
                }
                else if (opt.equals("-d") || opt.equals("-debug")) {
                    debug = new File(opts[++i]);
                    debugSupplied = true;
                }
                else if (opt.equals("-E") || opt.equals("-external")) {
                    specdMode = EXTERNAL_APP;
                    modeSupplied = true;
                }
                else if (opt.startsWith("-h") || opt.equals("--help")) {
                    usage();
                    System.exit(0);
                }
                else if (opt.equals("-L") || opt.equals("-libraryPath")) {
                    pathSupplied = true;
                    specdLibDirs.add((new File(System.getProperty("user.dir"), opts[++i])).getAbsoluteFile());
                }
                else if (opt.equals("-l") || opt.equals("-library")) {
                    librariesSupplied = true;
                    specdLibNames.add(new File(System.getProperty("user.dir"), opts[++i]).getAbsoluteFile());
                }
                else if (opt.equals("-n") || opt.equals("-port")) {
                    try {
                        port = Integer.valueOf(opts[++i]);
                    } catch (NumberFormatException e) {
                        System.out.println("The argument to the " + opt + " option, "
                                           + opts[i]
                                           + ", is not a valid integer");
                        usage();
                        System.exit(1);
                    }
                    portSupplied = true;
                }
                else if (opt.equals("-p") || opt.equals("-plan")) {
                    planSupplied = true;
                    plan = new File(opts[++i]);
                }
                else if (opt.equals("-s") || opt.equals("-script")) {
                    scriptSupplied = true;
                    script = new File(opts[++i]);
                }
                else if (opt.equals("-S") || opt.equals("-plexilsim")) {
                    specdMode = PLEXIL_SIM;
                    modeSupplied = true;
                }
                else if (opt.equals("-T") || opt.equals("-plexiltest")) {
                    specdMode = PLEXIL_TEST;
                    modeSupplied = true;
                }
                else if (opt.equals("-U") || opt.equals("-plexilexec")) {
                    specdMode = PLEXIL_EXEC;
                    modeSupplied = true;
                }
                else {
                    System.out.println("Unrecognized option \"" + opt + "\"");
                    usage();
                    System.exit(1);
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                System.out.println("Missing argument for " + opts[opts.length - 1] + " option");
                usage();
                System.exit(1);
            }
        }

        // Store results of parse
        if (autoRunSupplied)
            setAutoRun(autoRun);

        if (modeSupplied)
            setAppMode(specdMode);

        if (blockSupplied)
            setBlocksExec(block);

        if (checkSupplied)
            setCheckPlan(check);

        if (portSupplied) {
            if (port < 1 || port > 65535) {
                System.out.println("The argument to the -n (-port) option, "
                                   + port
                                   + ", is not between 1 and 65535");
                usage();
                System.exit(1);
            }
            setPort(port);
        }

        if (planSupplied) {
            plan = plan.getAbsoluteFile();
            setPlanLocation(plan);
        }
        if (configSupplied) {
            config = config.getAbsoluteFile();
            setConfigLocation(config);
        }
        if (debugSupplied) {
            debug = debug.getAbsoluteFile();
            setDebugLocation(debug);
        }
        if (scriptSupplied) {
            script = script.getAbsoluteFile();
            setScriptLocation(script);
        }

        if (!specdLibDirs.isEmpty()) {
            setLibDirs(specdLibDirs);
        }

        if (!specdLibNames.isEmpty()) {
            setLibs(specdLibNames);
        }

        return true;
    }

    public void usage() {
        System.out.println("Supported options:");
        System.out.println(HELP_TEXT);
    }

    public void loadSavedSettings(File file) {
        try {
            m_properties.load(new FileInputStream(file));
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e,
                                                                "Unable to load settings from file "
                                                                + file.getPath());
            return;
        }
        loadFromProperties();
    }

    public void saveSettings(File file) {
        try {
            m_properties.store(new FileOutputStream(file), null);
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e,
                                                                "Unable to save settings to file "
                                                                + file.getPath());
        }
    }

    public void load() {
        if (m_file != null
            && m_file.exists())
            loadSavedSettings(m_file);
    }

    public void save() {
        saveToProperties();
        if (m_file != null)
            saveSettings(m_file);
    }

    public String get(String name) {
        return m_properties.getProperty(name);
    }

    public String getProperty(String name) {
        return m_properties.getProperty(name);
    }

    public void setProperty(String name, String value) {
        m_properties.setProperty(name, value);
    }

    public boolean getBoolean(String name) {
        return Boolean.parseBoolean(m_properties.getProperty(name));
    }

    public int getInteger(String name) {
        String temp = m_properties.getProperty(name);
        if (temp == null)
            return 0;
        try {
            return Integer.parseInt(temp);
        } catch (NumberFormatException e) {
            return 0;
        }
    }

    public File getFile(String name) {
        String fname = m_properties.getProperty(name);
        if (fname == null || fname.isEmpty())
            return null;
        return new File(fname);
    }

    // Handles any serializable object
    public Object getObject(String name) {
        String temp = get(name);
        if (temp == null)
            return null;
        try {
            return deserialize(temp);
        } catch (Exception e) {
            // *** TEMP (?) DEBUG ***
            System.err.println("Settings.getObject: error deserializing \"" + name + "\":\n" + e.toString());
            return null;
        }
    }

    // Collections of files are stored as arrays of strings
    public Vector<File> getFileList(String name) {
        Object o = getObject(name);
        if (o instanceof String[]) {
            @SuppressWarnings("unchecked")
                String[] sa = (String[]) o;
            Vector<File> result = new Vector<File>(sa.length);
            for (String s : sa)
                result.add(new File(s));
            return result;
        }
        return null;
    }

    public Vector<String> getStringList(String name) {
        Object o = getObject(name);
        if (o instanceof String[]) {
            @SuppressWarnings("unchecked")
                String[] sa = (String[]) o;
            Vector<String> result = new Vector<String>(sa.length);
            for (String s : sa)
                result.add(s);
            return result;
        }
        return null;
    }

    // Color is serializable
    public Color getColor(String name) {
        Object o = getObject(name);
        if (o instanceof Color)
            return (Color) o;
        return null;
    }

    // Dimension is serializable
    public Dimension getDimension(String name) {
        Object o = getObject(name);
        if (o instanceof Dimension)
            return (Dimension) o;
        return null;
    }

    // Point is serializable
    public Point getPoint(String name) {
        Object o = getObject(name);
        if (o instanceof Point)
            return (Point) o;
        return null;
    }
    
    public void set(String name, String value) {
        m_properties.setProperty(name, value);
    }

    public void set(String name, boolean value) {
        m_properties.setProperty(name, Boolean.toString(value));
    }

    public void set(String name, int value) {
        m_properties.setProperty(name, Integer.toString(value));
    }

    public void set(String name, File value) {
        if (value != null)
            m_properties.setProperty(name, value.getAbsolutePath());
        else
            m_properties.setProperty(name, "");
    }

    public void setStringList(String name, Collection<String> c) {
        String[] proto = new String[0];
        set(name, c.toArray(proto));
    }

    public void setFileList(String name, Collection<File> c) {
        String[] tmp = new String[c.size()];
        int i = 0;
        for (File f : c)
            tmp[i++] = f.getAbsolutePath();
        set(name, tmp);
    }

    public void set(String name, Object value) {
        m_properties.setProperty(name, serialize(value));
    }

    // Use Base64 encoding
    // Requires Java 8
    private static String serialize(Object o) {
        ByteArrayOutputStream bs = new ByteArrayOutputStream();
        try {
            ObjectOutputStream os = new ObjectOutputStream(bs);
            os.writeObject(o);
            os.close();
            return Base64.getEncoder().encodeToString(bs.toByteArray());
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e,
                                                                "Unable to serialize object "
                                                                + o.toString());
        }
        return null;
    }

    private static Object deserialize(String s) {
        ByteArrayInputStream bs =
            new ByteArrayInputStream(Base64.getDecoder().decode(s));
        try {
            ObjectInputStream os = new ObjectInputStream(bs);
            Object result = os.readObject();
            os.close();
            return result;
        } catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e,
                                                                       "Unable to serialize object from string \""
                                                                       + s + "\"");
        }
        return null;
    }

    //
    // Particular settings
    //

    public boolean getAutoRunSupplied() {
        return autoRunSupplied;
    }

    public boolean getAutoRun() {
        String a = m_properties.getProperty(PROP_AUTO_RUN);
        if (a != null)
            return Boolean.valueOf(a);
        return PROP_AUTO_RUN_DEF;
    }
    
    public void setAutoRun(boolean val) {
        set(PROP_AUTO_RUN, val);
    }

    public boolean getModeSupplied() {
        return modeSupplied;
    }

    /** Returns Which application is in use.
     *  @return the current instance of mode */
    public AppType getAppMode() {
        String m = m_properties.getProperty(PROP_APP_TYPE);
        if (m != null)
            return AppType.valueOf(m);
        return APP_TYPE_DEF;
    }

    /** Sets the flag that indicates which application we are using.
     */
    public void setAppMode(AppType mode) {
        set(PROP_APP_TYPE, mode.toString());
    }

    public boolean getPortSupplied() {
        return portSupplied;
    }

    public void setPortSupplied(boolean newval) {
        portSupplied = newval;
    }

    public int getPort() {
        return getInteger(PROP_NET_SERVER_PORT);
    }

    public void setPort(int newPort) {
        set(PROP_NET_SERVER_PORT, newPort);
    }

    public boolean getBlockSupplied() {
        return blockSupplied;
    }

    public boolean blocksExec() {
        return getBoolean(PROP_BLOCKS_EXEC);
    }

    public void setBlocksExec(boolean newval) {
        set(PROP_BLOCKS_EXEC, newval);
    }

    public boolean getCheckSupplied() {
        return checkSupplied;
    }

    public boolean checkPlan() {
        return getBoolean(PROP_CHECK_PLAN);
    }

    public void setCheckPlan(boolean newval) {
        set(PROP_CHECK_PLAN, newval);
    }

    public boolean getPlanSupplied() {
        return planSupplied;
    }

    public File getPlanLocation() {
        return getFile(PROP_PLAN_FILE);
    }

    public void setPlanLocation(File f) {
        set(PROP_PLAN_FILE, f);
    }

    public boolean getConfigSupplied() {
        return configSupplied;
    }

    public File getConfigLocation() {
        return getFile(PROP_CONFIG_FILE);
    }

    public void setConfigLocation(File f) {
        set(PROP_CONFIG_FILE, f);
    }

    public boolean getScriptSupplied() {
        return scriptSupplied;
    }

    public File getScriptLocation() {
        return getFile(PROP_SCRIPT_FILE);
    }

    public void setScriptLocation(File f) {
        set(PROP_SCRIPT_FILE, f);
    }

    public boolean getDebugSupplied() {
        return debugSupplied;
    }

    public File getDebugLocation() {
        return getFile(PROP_DEBUG_FILE);
    }

    public void setDebugLocation(File f) {
        set(PROP_DEBUG_FILE, f);
    }

    public boolean getLibDirsSupplied() {
        return pathSupplied;
    }

    public Collection<File> getLibDirs() {
        return libDirs;
    }

    public void setLibDirs(Collection<File> dirs) {
        libDirs = new Vector<File>(dirs);
    }

    public void addLibDir(File f) {
        libDirs.add(f);
    }

    public boolean getLibsSupplied() {
        return librariesSupplied;
    }

    public Collection<File> getLibs() {
        return libs;
    }

    public void setLibs(Collection<File> names) {
        libs = new Vector<File>(names);
    }

    public void addLib(File s) {
        libs.add(s);
    }

    // FIXME: behavior should vary by app mode
    // FIXME: not clear this is right place for this behavior
    public File defaultEmptyScriptFile(AppType mode) {
        switch (mode) {
        case PLEXIL_TEST:
            return new File(Constants.DEFAULT_CONFIG_PATH, Constants.DEFAULT_SCRIPT_NAME);

            // FIXME: default script for plexil sim needed
        case PLEXIL_SIM:
            return new File(Constants.DEFAULT_CONFIG_PATH, Constants.DEFAULT_SCRIPT_NAME);
        }
        return null;
    }

    // Restore settings from stored properties
    public void loadFromProperties() {
        Vector<File> fl = getFileList(PROP_LIB_DIRS);
        if (fl != null)
            libDirs = fl;
        Vector<File> sl = getFileList(PROP_LIB_NAMES);
        if (sl != null)
            libs = sl;
    }

    // Saves current settings
    public void saveToProperties() {
        setFileList(PROP_LIB_DIRS, libDirs);
        setFileList(PROP_LIB_NAMES, libs);
    }

    // N.B. Plan will not have a name or last-modified until loaded
    public Plan newPlanFromDefaults() {
        Plan result = new Plan();
        File pf = getPlanLocation();
        if (pf == null)
            pf = new File(System.getProperty("user.dir"), "example.plx");
        result.setPlanFile(pf);

        if (!libDirs.isEmpty())
            result.setLibraryPath(libDirs);
        if (!libs.isEmpty())
            result.setLibraryFiles(libs);

        AppType t = getAppMode();
        result.setAppType(t);
        File tf = null; // temp
        switch (t) {
        case PLEXIL_TEST:
            tf = getScriptLocation();
            if (tf == null)
                tf = new File(System.getProperty("user.dir"), "example.psx");
            result.setScriptFile(tf);
            break;

        case PLEXIL_SIM:
            tf = getScriptLocation();
            if (tf == null)
                tf = new File(System.getProperty("user.dir"), "example.psx");
            result.setScriptFile(tf);
            // fall through to...
        case PLEXIL_EXEC:
            tf = getConfigLocation();
            if (tf == null)
                tf = new File(System.getProperty("user.dir"), "example.xml");
            result.setConfigFile(tf);
            break;

        default:
            break;
        }

        return result;
    }

    public void setPlanDefaults(Plan p) {
        setPlanLocation(p.getPlanFile());
        setDebugLocation(p.getDebugFile());
        setLibDirs(p.getLibraryPath());
        setLibs(p.getLibraryFiles());
        AppType m = p.getAppType();
        setAppMode(m);
        switch (m) {
        case PLEXIL_TEST:
            setScriptLocation(p.getScriptFile());
            break;

        case PLEXIL_SIM:
            setScriptLocation(p.getScriptFile());
            // fall thru to...
        case PLEXIL_EXEC:
            setConfigLocation(p.getConfigFile());
            break;

        default:
            // can't make any assumptions
            break;
        }

        save(); 
    }

}
