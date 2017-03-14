/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.JDialog;
import javax.swing.JProgressBar;
import javax.swing.JLabel;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.JScrollPane;
import javax.swing.BorderFactory;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;

public class GuiUtil {

	// NOTICE: ALL FUNCTIONS THAT ARE NOT ACTUALL GUI CALLS HAVE BEEN MOVED TO "DebugSupport.java"


	private static File lastDir = null;
	private static JPanel xrt;

	private static final FileFilter main_filter = new FileNameExtensionFilter("Traffic files (.txt, .csv, .pln, .swy)", "txt", "csv", "swy", "pln");

	/**
	 * Open a dialog box to choose a traffic file to open.
	 * @return the chosen filename or "" if no file chosen
	 */
	public static String getFilenameDialog() {
		return getFilenameDialog("",false);
	}

	/**
	 * Open a dialog box to choose a file.  The title of the box will be "open" or "save," depending on 
	 * the value of the "save" parameter.
	 * @param save if true open a "save" dialog box, if false open a "open" dialog box
	 * @param ext the list of extensions to filter results 
	 * @param description a description of the extensions using for filtering
	 * @return the chosen filename or "" if no file chosen
	 */
	public static String getFilenameDialog(String title, boolean save) {
		FileFilter[] ff = new FileFilter[1];
		ff[0] = main_filter;
		return getFilenameDialog(title,save,ff);
	}

	/**
	 * Open a dialog box to choose a file.  
	 * @param title the title of this dialog box
	 * @param save if true open a "save" dialog box, if false open a "open" dialog box
	 * @param ext the list of extensions to filter results 
	 * @param description a description of the extensions using for filtering
	 * @return the chosen filename or "" if no file chosen
	 */
	public static FileFilter makeFileFilter(String description, String[] ext) {
		final String descr = description;
		final String[] extensions = ext;

		FileFilter filter = new FileFilter(){
			public boolean accept(java.io.File f) {
				if (f.isDirectory()) {
					return true;
				}
				int i = f.getName().lastIndexOf('.');

				String ext2 = null;
				if (i > 0 &&  i < f.getName().length() - 1) {
					ext2 = f.getName().substring(i+1).toLowerCase();
				}
				if (ext2 != null) {
					for (int j = 0; j < extensions.length; j++) {
						if (ext2.equals(extensions[j])) return true;
					}
				}
				return false;
			} // accept

			public String getDescription() {
				return descr;
			}
		};
		return filter;
	}

	/**
	 * Open a dialog box to choose a file.  
	 * @param title the title of this dialog box
	 * @param save if true open a "save" dialog box, if false open a "open" dialog box
	 * @param ff	an array of file filters, the 0 index filter will be set as the default
	 * @return the chosen filename or "" if no file chosen
	 */
	public static String getFilenameDialog(String title, boolean save, FileFilter[] ff, int xDim, int yDim) {	
		JFileChooser fc = new JFileChooser(System.getProperty("user.dir"));
		fc.setPreferredSize(new Dimension(xDim,yDim));
		if ( ! title.equals("")) {
			fc.setDialogTitle(title);
		}		
		// load up last directory used
		if (lastDir != null) {
			fc.setCurrentDirectory(lastDir);
		}
		fc.setAcceptAllFileFilterUsed(true);  // if false, then the "all files" option is turned off
		for (FileFilter filter: ff) {
			fc.addChoosableFileFilter(filter); 			
		}
		int returnVal;

		if (ff.length > 0) {
			fc.setFileFilter(ff[0]);
		}

		if (save) returnVal = fc.showSaveDialog(xrt);
		else returnVal = fc.showOpenDialog(xrt);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File f = fc.getSelectedFile();
			lastDir = f.getParentFile();
			return f.getPath();
		} else {
			return "";
		}
	}

	// return file name from interactive input
	public static String getFilenameDialog(String title, boolean save, FileFilter[] ff) {	
		return getFilenameDialog(title, save, ff,1000,800);
	}


	/**
	 * Display a large block of text in a scrollable dialog box.
	 * @param mainFrame the frame to attach this dialog to
	 * @param text the text, this is expected to have many, many lines.
	 */
	public static void getTextDialog(JFrame mainFrame, String title, String text) {
		final JDialog dialog = new JDialog(mainFrame);
		dialog.setLocationByPlatform(true);
		if (title != null) {
			dialog.setTitle(title);
		}
		JTextArea txtArea = new JTextArea(30,80);
		txtArea.setAutoscrolls(true);
		//txtArea.setPreferredSize(new Dimension(900, 500));
		txtArea.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		txtArea.setForeground(Color.BLACK);
		txtArea.setFont(new Font("courier new", Font.PLAIN, 12));
		txtArea.setLineWrap(true);
		JScrollPane txtAreaScroll = new JScrollPane();
		txtAreaScroll.setViewportView(txtArea);
		txtAreaScroll.setAutoscrolls(true);
		txtArea.setText(text);

		JButton ok = new JButton("OK");
		ok.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				dialog.dispose();
				//dialog.dispatchEvent(new WindowEvent(dialog, WindowEvent.WINDOW_CLOSING));				
			}
		});

		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.LINE_AXIS));
		buttonPanel.setBorder(BorderFactory.createEmptyBorder(0, 10, 10, 10));
		buttonPanel.add(Box.createHorizontalGlue());
		buttonPanel.add(ok);

		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		panel.add(txtAreaScroll);
		panel.add(buttonPanel);
		dialog.add(panel);

		dialog.pack();
		dialog.setVisible(true);
	}

	public static String getInteractiveInput(String headerLabel, String prompt) {
		return JOptionPane.showInputDialog(null,prompt,headerLabel,JOptionPane.QUESTION_MESSAGE);
	}


	public static String getInteractiveInput(String headerLabel, String prompt, String defaultValue) {
		return (String)JOptionPane.showInputDialog(null,prompt,headerLabel,JOptionPane.QUESTION_MESSAGE, null, null, defaultValue);
	}


	public static void simpleMessageDialog(String msg) {
		JOptionPane.showMessageDialog(null, msg);	
	}

	public static JDialog dialogModeless(JFrame parent, String title, String msg) {
		JDialog dialog = new JDialog(parent,title, false); //pane.createDialog(parent, "Hello");

		JPanel messagePane = new JPanel();
		messagePane.add(new JLabel(msg));
		dialog.getContentPane().add(messagePane);

		//dialog.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		dialog.pack();
		dialog.setVisible(true);

		//JOptionPane.showMessageDialog(parent, msg);
		return dialog;
	}

	public static void simpleMessageDialog(String msg, Color clr) {
		UIManager.put("OptionPane.messageForeground", clr);
		//um.put("Panel.background", Color.gray);		
		JOptionPane.showMessageDialog(null, msg);	
		UIManager.put("OptionPane.messageForeground", Color.black);
	}


	public static void simpleMessageDialog(JFrame jf, String msg) {
		JOptionPane.showMessageDialog(jf, msg);	
	}

	public static void dumpOutputFile(String outputFileName, String s) {
		try {
			java.io.PrintWriter pw = 
					new java.io.PrintWriter(new java.io.BufferedWriter(new java.io.FileWriter(outputFileName)));
			pw.println(s);
			pw.flush();
			pw.close();
			pw.flush();
		} catch (Exception e) {
			simpleMessageDialog(e.toString());
		}
	}

	/**
	 * Creates a (visible) basic ViewGraph2D window populated with the given values.
	 * 
	 * @param title window title
	 * @param series data series name
	 * @param xvals x values
	 * @param yvals y values
	 * @return ZoomGraphFrame window with default settings (line plot)
	 */
	public static ZoomGraphFrame makeGraphWindow(String frameTitle, String series, double[] xvals, double [] yvals) {
		ViewGraph2D plot = new ViewGraph2D();

		ZoomGraphFrame frame = new ZoomGraphFrame(plot, null, null);

		frame.setTitle(frameTitle);
		frame.setSize(1024,760);
		frame.setVisible(true);    
		frame.clear();
		for (int i = 0; i < xvals.length; i++) {
			frame.addData(series, xvals[i], yvals[i]);
		}
		frame.initScale();
		frame.repaint();

		//vStrat.vg2D.reCenter();
		frame.toFront();  
		frame.requestFocus();

		return frame;
	}

	/**
	 * Create a ViewFrame displaying some plans, polypaths, and/or states
	 * @param frameTitle name of window
	 * @param plans may be null
	 * @param paths may be null
	 * @param states may be null
	 * @return new ViewFrame for plans
	 */
	public static ViewFrame makePlanWindow(String frameTitle, List<Plan> plans, List<PolyPath> paths, List<Triple<String,Position,Velocity>> states) {
		BasicViewHoriz panel = new BasicViewHoriz();
		ViewFrame frame = new ViewFrame(panel, null, null);
		frame.setTitle(frameTitle);
		frame.setSize(1024,760);
		frame.setVisible(true);    
		panel.addPlans(plans);
		panel.addPaths(paths);
		panel.addStates(states);
		panel.initScale();
		frame.repaint();
		frame.toFront();  
		frame.requestFocus();
		return frame;
	}

	public static void showPlanWindow(Plan plan) {
		ArrayList<Plan> p = new ArrayList<Plan>();
		p.add(plan);
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				ViewFrame frame = makePlanWindow(plan.getName(), p, null, null);
				frame.setVisible(true);
			}
		});
	}

	public static void showPlanWindow(List<Plan> p) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				ViewFrame frame = makePlanWindow("Plans", p, null, null);
				frame.setVisible(true);
			}
		});
	}

	public static void showPlanFile(String filename) {
		PlanReader reader = new PlanReader();
		reader.open(filename);
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				ViewFrame frame = makePlanWindow(filename, reader.plans, reader.paths, null);
				frame.setVisible(true);
			}
		});
	}

	/** 
	 * Create a dialog box containing a progress bar.  The progress being monitored is in the class
	 * SwingWorker.<p>
	 * 
	 * The SwingWorker class contains the task, performed in the background, whose progress the
	 * progress bar is measuring.  There are two main methods in SwingWorker that are relevant for 
	 * progress bars: doInBackground() and done().  doInBackground() contains the work to be
	 * performed and done() contains any cleanup activities. After  makeProgressDialog() is
	 * called, the background task must be started with sw.execute().<p>
	 * 
	 * In the doInBackground() method, the setProgress(int) method can be called with
	 * a parameter from 0 to 100, which indicates the value of the progress bar.<p>
	 * 
	 * (Incomplete) Example SwingWorker Class
	 * <code>
	 * 	class FileOpenSlowTask extends SwingWorker<Void, Void> {
	 *   public Void doInBackground() {
	 *      setProgress(0);
	 *      do something...
	 *      setProgress(50);
	 *      do something else...
	 *      setProgress(100);
	 *      return null;
	 *   }
	 *   public void done() {
	 * 		dialog.dispose(); // removes the progress bar dialog box
	 *      frame.setCursor(Cursor.getDefaultCursor()); // resets the mouse pointer to the default
	 *      }
	 *   }
	 *   </code>
	 * 
	 * @param frame the enclosing frame
	 * @param title the title of the progress bar
	 * @param indeterminate true if the 
	 * @param sw the background activity whose progress is being monitored
	 * @return a dialog box containing the progress bar
	 */
	public static JDialog makeProgressDialog(JFrame frame, String title, boolean indeterminate, SwingWorker<Void,Void> sw) {
		//Create and set up the window.
		JDialog dialog = new JDialog(frame, title);
		frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		//Create and set up the content pane.
		ProgressBarImpl newContentPane = new ProgressBarImpl(indeterminate);
		newContentPane.setOpaque(true); //content panes must be opaque
		dialog.setContentPane(newContentPane);

		sw.addPropertyChangeListener(newContentPane);

		//Display the window.
		dialog.pack();
		dialog.setVisible(true);
		dialog.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
		frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

		return dialog;
	}
}


/*package*/ class ProgressBarImpl extends JPanel implements PropertyChangeListener {
	private static final long serialVersionUID = 1L;
	private JProgressBar progressBar;

	public ProgressBarImpl(boolean indeterminate) {
		super(new BorderLayout());
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);
		progressBar.setIndeterminate(indeterminate);

		add(progressBar, BorderLayout.PAGE_START);
		setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
	}

	/**
	 * Invoked when task's progress property changes.
	 */
	public void propertyChange(PropertyChangeEvent evt) {
		if ("progress" == evt.getPropertyName()) {
			int progress = (Integer) evt.getNewValue();
			progressBar.setValue(progress);
		} 
	}
}


