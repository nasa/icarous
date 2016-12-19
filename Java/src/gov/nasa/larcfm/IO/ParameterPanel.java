/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;


import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * Generic GUI panel for viewing and setting parameters.
 * 
 * This displays all parameters in the provided keyList with values from the given ParameterData object.
 * setup() must be called whenever new parameters are added.  The user specifies the number of approximately equal 
 * length columns the parameters will be divided into.
 * 
 * Each column will have up to three sub-elements: a label containing the parameter key, a user-modifiable JComponent 
 * containing the paremeter's value, and a label or combo box containing the associated unit (for numeric parameters
 * with units). 
 * 
 * By default a JTextField is automatically generated for every parameter key in the associated ParameterData object.
 * The keys displayed can be limited by the setKeyList() method.  This can also be used to specify the order in which
 * keys are presented, with a flag to determine if the keys are arranged by row (horizontal) or columns (vertical).  
 * There are also flags to allow units to be displayed as an adjacent label or pull-down menu, or to have boolean 
 * entries default to a checkbox (checked = true).
 *  
 * The user may specify a JTextField, JLabel, JSpinner, or JComboBox component to replace the default user entry 
 * component for any specific key by using the appropriate setMiscEntry() call.  The appropriate ActionListener or
 * ItemListener will be automatically generated for the component in question and added to it, but other parameters 
 * for the component (such as the list of choices associated with a JComboBox) must be specified by the user.
 *  
 * For text-based inputs, the value change will be registered when the user hits the enter key; 
 * a parameter can be set so that at that time the value field's color will temporarily change to 
 * indicate successful entry.  Entries may also be stored by calling the processAll() method, 
 * which could be associated with a "done" button, for example. 
 *  
 * This panel is expected to be held in a frame of some sort, such as a ParameterFrame.
 * 
 * If ParameterData objects are to be shared between ParameterPanels, they should all reference the same object.
 * 
 * This is assumed to be extended for specific applications, with the functions
 * log(), saveData(), and updateDisplays() tailored to the application, if necessary.
 * When an entry is updated, log() is called, then the local ParameterData object is updated, then saveData() 
 * is called, and finally updateDisplays() is called.
 * 
 */
public class ParameterPanel extends JPanel {
	private static final long serialVersionUID = 1L;

	private Color normalColor = Color.BLACK;
	private Color modifiedColor = Color.BLUE;

	int columns = 2; // number of columns of parameter/value pairs
	String id = ""; // label for this panel

	private int ROW = 0;
	private int COL = 0;
	private int LASTCOL = 0;

	protected ParameterData params;
	protected ArrayList<String> keyList = null; // list of all keys, in order (optional)
	protected HashMap<String, JTextField> textEntries = new HashMap<String,JTextField>(); // list of default user elements to be displayed
	protected HashMap<String, JCheckBox> boolEntries = new HashMap<String,JCheckBox>(); // list of default boolean user elements to be displayed (only used if pulldownBooleans flag is set)
	protected HashMap<String, JComponent> unitEntries = new HashMap<String,JComponent>(); // list of unit elements to be displayed
	protected HashMap<String, JComponent> miscEntries = new HashMap<String,JComponent>(); // list of user-specified user elements to be displayed

	private boolean addNewParamButton = false;
	private boolean preserveUserUnits = true;
	private boolean showUnits = true;
	private boolean sortKeys = false;
	private boolean vertical = true;
	private boolean enabled = true;
	private boolean pulldownUnits = false;
	private boolean checkBoxBooleans = false;
	private boolean showModifiedColor = false;
	private int precision = 4;

	private boolean inUpdate = false; // This value is used to guard JComboBox and JSpinners when updating to avoid recursive changes.

	// used to check if setup needs to be run again
	private List<String> oldKeyList = null; // list of keys from last setup call
	private boolean dirty = true;  // set to true if any parameters for this panel have changed, or if setup not run yet


	/**
	 * Default constructor.
	 */
	public ParameterPanel() {
		params = new ParameterData();
		id = "";
	}

	/**
	 * Builds keylist based on p.
	 * Also calls setup()
	 * @param name
	 * @param p
	 */
	public ParameterPanel(String name, ParameterData p) {
		id = name;
		params = p;
		keyList = new ArrayList<String>(p.getList());
		setup();
	}

	/**
	 * Change header name for this panel
	 * Must call setup() after this call.
	 */
	public void setName(String name) {
		id = name;
		dirty = true;
	}

	public String getName() {
		return id;
	}

	/**
	 * Number of key/value pair columns for this panel
	 * Must call setup() after this call.
	 * @param i
	 */
	public void setColumnNumber(int i) {
		columns = i;
		dirty = true;
	}

	public int getColumnNumber() {
		return columns;
	}

	/**
	 * Parameter values for this panel
	 * Must call setup() after this call.
	 * @param p
	 */
	public void setData(ParameterData p) {
		params = p;
		// not automatically "dirty" -- this is checked via the keysDirty() call
	}

	public ParameterData getData() {
		return params;
	}

	/**
	 * Only display these parameters.
	 * These will be displayed in the provided order as columns unless sortKeys() is called.
	 * Setting this value to null will display all parameters in the associated ParameterData object.
	 * If this list includes parameter keys not in the ParameterData object, they will be added with "empty" values.
	 * Must call setup() after this call.
	 * @param keys
	 */
	public void setKeyList(List<String> keys) {
		if (keys != null) {
			keyList = new ArrayList<String>(keys);
			dirty = true;
		}
	}

	/**
	 * Return the stored list of parameter keys that will be displayed for this ParameterPanel
	 * @return
	 */
	public List<String> getKeyList() {
		return keyList;
	}


	/**
	 * Set flag to include a field to add additional parameters by user.  This will be an extra text field and associated button
	 * that lets the user add additional parameter keys. 
	 * @param b
	 */
	public void setAddParams(boolean b) {
		addNewParamButton = b;
		dirty = true;
	}

	public boolean getAddParams() {
		return addNewParamButton;
	}

	/**
	 * Set flag to include default units labels in text boxes.
	 * @param b
	 */
	public void setShowUnits(boolean b) {
		showUnits = b;
		dirty = true;
	}

	public boolean getShowUnits() {
		return showUnits;
	}

	/**
	 * Set flag to have user-specified units override existing units (if compatible)
	 * @param b
	 */
	public void setPreserveUserUnits(boolean b) {
		preserveUserUnits = b;
		dirty = true;
	}

	public boolean getPreserveUserUnits() {
		return preserveUserUnits;
	}

	/**
	 * Set the default precision (number of fractional digits) to be displayed for numeric data
	 * @param i
	 */
	public void setPrecision(int i) {
		precision = i;
		dirty = true;
	}

	public int getPrecision() {
		return precision;
	}

	/**
	 * Set flag to determine if this panel is enabled (user-modifiable)
	 * @param b
	 */
	public boolean isEnabled() {
		return enabled;
	}

	public void setEnabled(boolean b) {
		enabled = b;
		dirty = true;
	}

	/**
	 * Set flag to have units be displayed (and chosen) for text fields via an adjacent pull-down menu.  This requires the showUnits flag
	 * be set to true. These pulldown menus will only apply to the default automatically generated JTextFields, not user-specified components.
	 * @param b
	 */
	public void setPulldownUnits(boolean b) {
		pulldownUnits = b;
		dirty = true;
	}

	public boolean getPulldownUnits() {
		return pulldownUnits;
	}

	/**
	 * Set flag to have boolean entries default to having a checkbox (with checked = true).  Note that parameters with stored string values
	 * that appear to be boolean values (specifically "true", "false", "T", or "F") can be confused for actual boolean 
	 * values, possibly resulting in an incorrect interface.
	 * @param b
	 */
	public void setCheckBoxBooleans(boolean b) {
		checkBoxBooleans = b;
		dirty = true;
	}

	public boolean getCheckBoxBooleans() {
		return checkBoxBooleans;
	}

	/**
	 * Set the default foreground color for items in this panel.  Items will revert to this color whenever update() or restColors() is called.
	 * Default is black.  This only applies if the showModifiedColors flag has been set.
	 */
	public void setNormalForegroundColor(Color c) {
		normalColor = c;
		dirty = true;
	}

	public Color getNormalForegroundColor() {
		return normalColor;
	}

	/**
	 * Set the modified foreground color for items in this panel.  The just-modified item will temporarily change its foreground color to this.
	 * Default is blue.  This only applies if the showModifiedColors flag has been set.
	 */
	public void setModifiedForegroundColor(Color c) {
		modifiedColor = c;
		dirty = true;
	}

	public Color getModifiedForegroundColor() {
		return modifiedColor;
	}

	/**
	 * Set flag to user-modified values in this panel will change color.
	 * @param b
	 */
	public void setShowModifiedColor(boolean b) {
		showModifiedColor = b;
		dirty = true;
	}

	public boolean getShowModifiedColor() {
		return showModifiedColor;
	}


	/**
	 * Add a single new key to the existing keyList.
	 * @param key
	 */
	public void addKey(String key) {
		if (keyList == null) {
			keyList = new ArrayList<String>();
		}
		if (!keyList.contains(key)) {
			keyList.add(key);
		}
		dirty = true;
	}

	/**
	 * Add a specific JComponent interface item for this key, overriding the default UI element.
	 * This automatically generates the appropriate ActionListener, but does not set other parameters for this component.
	 * The default value for the component should be set prior to making this call to avoid recursive update problems.
	 * @param key parameter key to tie this item to
	 * @param jc JComboBox with values to handle this key's entry
	 */
	public void setMiscEntry(String key, JComboBox<?> jc) {
		final String skey = key;
		final JComboBox<?> txf = jc;
		if (!params.contains(skey)) {
			params.set(skey,"");
		}
		txf.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				if (!inUpdate) {
					inUpdate = true;
					String s = (String)txf.getSelectedItem();
					log(skey+" "+s);
					if (params.isNumber(skey)) {
						try {
							String unit = params.getUnit(skey);
							double val = Double.parseDouble(s);
							params.set(skey, val, unit);
						} catch (NumberFormatException e){
							params.set(skey, s);
						}
					} else {
						params.set(skey, s);
					}
					saveData(skey);
					updateMiscEntry(skey);
					updateDisplays(skey);
					if (showModifiedColor) txf.setForeground(modifiedColor);					
					inUpdate = false;
				}
			}// actionPerformed
		});
		miscEntries.put(key, txf);
		dirty = true;
	}

	//	public void setMiscEntry(String key, JSlider jc) {
	//		miscEntries.put(key, txf);
	//	}

	/**
	 * Add a specific JComponent interface item for this key, overriding the default UI element.
	 * This automatically generates the appropriate ActionListener, but does not set other parameters for this component.
	 * The default value for the component should be set prior to making this call to avoid recursive update problems.
	 * @param key parameter key to tie this item to
	 * @param jc JSpinner with values to handle this key's entry
	 */
	public void setMiscEntry(String key, JSpinner jc) {
		final String skey = key;
		final JSpinner txf = jc;
		if (!params.contains(skey)) {
			params.set(skey,"");
		}
		txf.addChangeListener( new ChangeListener() {
			public void stateChanged(ChangeEvent ch) {
				if (!inUpdate) {
					inUpdate = true;
					String s = ""+txf.getValue();
					log(skey+" "+s);
					if (params.isNumber(skey)) {
						try {
							String unit = params.getUnit(skey);
							double val = Double.parseDouble(s);
							params.set(skey, val, unit);
						} catch (NumberFormatException e){
							params.set(skey, s);
						}
					} else {
						params.set(skey, s);
					}
					saveData(skey);
					updateMiscEntry(skey);
					updateDisplays(skey);
					if (showModifiedColor) txf.setForeground(modifiedColor);					
					inUpdate = false;
				}
			}// actionPerformed
		});
		miscEntries.put(key, txf);
		dirty = true;
	}

	/**
	 * Add a specific JComponent interface item for this key, overriding the default UI element.
	 * This automatically generates the appropriate ActionListener, but does not set other parameters for this component.
	 * The default value for the component should be set prior to making this call to avoid recursive update problems.
	 * @param key parameter key to tie this item to
	 * @param jc JTextField to handle this key's entry
	 */
	public void setMiscEntry(String key, JTextField jc) {
		final String skey = key;
		final JTextField txf = jc;
		if (!params.contains(skey)) {
			params.set(skey,"");
		}
		txf.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				log(skey+" "+txf.getText());
				if (params.isNumber(skey)) {
					try {
						String unit = params.getUnit(skey);
						double val = Double.parseDouble(txf.getText());
						params.set(skey, val, unit);
					} catch (NumberFormatException e){
						params.set(skey, txf.getText());
					}
				} else {
					params.set(skey, txf.getText());
				}
				saveData(skey);
				updateMiscEntry(skey);
				updateDisplays(skey);
				if (showModifiedColor) txf.setForeground(modifiedColor);					
			}// actionPerformed
		});
		miscEntries.put(key, txf);
		dirty = true;
	}

	/**
	 * Add a specific JComponent interface item for this key, overriding the default UI element.
	 * This automatically generates the appropriate ActionListener, but does not set other parameters for this component.
	 * @param key parameter key to tie this item to
	 * @param jc JLabel to display this key's value (without possible user modification)
	 */
	public void setMiscEntry(String key, JLabel jc) {
		//no actionlistener
		miscEntries.put(key, jc);
		dirty = true;
	}

	/**
	 * Remove a user-specified interface item for the given key, if it exists.  Do nothing if there is no specific interface tied to this key.
	 * @param key parameter key to remove intrface from
	 */
	public void removeMiscEntry(String key) {
		miscEntries.remove(key);
		dirty = true;
	}

	/**
	 * Remove all user-specified interfaces for all keys.
	 */
	public void clearMiscEntries() {
		miscEntries.clear();
		dirty = true;
	}

	/**
	 * Return the user-specified interface JComponent for the given key, or null if none has been defined.
	 * @param key
	 * @return
	 */
	public JComponent getMiscEntries(String key) {
		return miscEntries.get(key);
	}

	/**
	 * Sets a flag if the keys should be alphabetically sorted when displayed or not
	 */
	public void setSortKeys(boolean b) {
		sortKeys = b;
		dirty = true;
	}

	public boolean getSortKeys() {
		return sortKeys;
	}

	/**
	 * Set a flag to indicate ordering direction - true is vertical first (down columns), while false is horizontal first (across rows)
	 * @param b
	 */
	public void setVertical(boolean b) {
		vertical = b;
		dirty = true;
	}

	public boolean getVertical() {
		return vertical;
	}

	// grid organization
	private void newRow() {
		ROW++;
		COL = 0;
	}

	private int nextCol() {
		if (COL > LASTCOL) LASTCOL = COL;
		return COL++;
	}

	private void blankSpace() {
		COL++;
	}


	/**
	 * Called just before a parameter is updated in the (local) ParameterData object.
	 * This is intended to allow for application logging of a value assignment, if desired.
	 * Subclasses should redefine this, if necessary.
	 * @param key parameter name
	 */
	protected void log(String key) {}

	/**
	 * Called just after a parameter is updated in the (local) ParameterData object.
	 * This is intended to allow for the main application to update its parameter value, if desired.
	 * This is probably not needed if this panel and the main application share a ParameterData object. 
	 * Subclasses should redefine this, if necessary.  
	 * Note: If the subclass is not updating a shared ParameterData object, it should probably only 
	 * call setParameters() using the relevant subset of this ParameterData object (i.e. just this entry's key)
	 * @param key parameter name
	 */
	protected void saveData(String key) {}

	/**
	 * Called at the end of a parameter update.
	 * This is intended to inform the main application of a change in values to trigger a display refresh, if desired. 
	 * Subclasses should redefine this, if necessary.
	 * @param key parameter name
	 */
	protected void updateDisplays(String key) {}


	/**
	 * Return true if the keys have changed since the last setup call.
	 */
	private boolean keysDirty(List<String> localKeys) {
		if (localKeys == oldKeyList) return false;
		if (localKeys == null || oldKeyList == null) return true;
		if (localKeys.size() != oldKeyList.size()) return true;
		for (int i = 0; i < localKeys.size(); i++) {
			if (localKeys.get(i).equals(oldKeyList.get(i))) return true;
		}
		return false;
	}

	/**
	 * Main display setup.  
	 * This should be re-run whenever parameters could have changed (including parameters for this object).
	 * Note this does not populate the table.  Call update() for that.
	 */
	@SuppressWarnings("unchecked")
	public void setup() {
		List<String> localKeys = keyList;
		if (localKeys == null) {
			localKeys = params.getList();
		}
		// no need to do anything if the panel is "clean"
		if (dirty || keysDirty(localKeys)) {
			this.removeAll();
			setLayout(new GridBagLayout() );
			GridBagConstraints c = new GridBagConstraints();
			ROW = 0;
			COL = 0;
			c.anchor = GridBagConstraints.WEST;

			if (!id.equals("")) {
				c.gridx = COL;
				c.gridy = ROW;
				c.gridwidth = GridBagConstraints.REMAINDER;
				c.fill = GridBagConstraints.HORIZONTAL;
				add(new JLabel(id+":",JLabel.LEFT),c);
				c.gridwidth = 1;
			}
			c.fill = GridBagConstraints.NONE;

			if (sortKeys) {
				Collections.sort(localKeys);
			}


			int size = localKeys.size();
			int n = (int)Math.ceil(1.0 * size / columns); // upper limit
			textEntries.clear();
			boolEntries.clear();
			for (int k = 0; k < n; k++) {
				newRow();
				for (int j = 0; j < columns; j++) {
					int i = j*n+k;
					if (!vertical) {
						i = k*columns+j;
					}
					if (i < size) {
						final String key = localKeys.get(i);
						JLabel lab = new JLabel(localKeys.get(i));
						lab.setBorder(BorderFactory.createEmptyBorder(0,10,0,0));

						// user-specified UI elements
						if (miscEntries.containsKey(key)) {
							JComponent jc = miscEntries.get(key);
							c.gridx = nextCol();
							c.gridy = ROW;
							c.fill = GridBagConstraints.NONE;
							c.weightx = 0.0;
							this.add(lab,c);
							c.gridx = nextCol();
							c.gridy = ROW;
							c.fill = GridBagConstraints.HORIZONTAL;
							c.weightx = 1.0;
							this.add(jc,c);
						} else { // default UI elements
							if (checkBoxBooleans && params.isBoolean(key)) {
								final JCheckBox bcb = new JCheckBox();
								bcb.setSelected(params.getBool(key));
								bcb.addActionListener(new ActionListener() {
									public void actionPerformed(ActionEvent event) {
										if (!inUpdate) {
											inUpdate = true;
											log(key+" "+bcb.isSelected());
											params.set(key, (Boolean)bcb.isSelected());
											saveData(key);
											updateBoolEntry(key);
											updateDisplays(key);
											if (showModifiedColor) bcb.setForeground(modifiedColor);					
											inUpdate = false;
										}
									}// actionPerformed
								});
								boolEntries.put(key,bcb);
								c.gridx = nextCol();
								c.gridy = ROW;
								c.fill = GridBagConstraints.NONE;
								c.weightx = 0.0;
								this.add(lab,c);
								c.gridx = nextCol();
								c.gridy = ROW;
								c.fill = GridBagConstraints.HORIZONTAL;
								c.weightx = 1.0;
								this.add(bcb,c);
							} else { // textfield default
								final JTextField txf = new JTextField(10);
								if (!params.contains(key)) {
									params.set(key,"");
								}
								txf.requestFocus();
								txf.setEditable(true);
								txf.addActionListener(new ActionListener() {
									public void actionPerformed(ActionEvent event) {
										log(key+" "+txf.getText());
										if (params.isNumber(key)) {
											boolean presUnits = params.isPreserveUnits();
											if (preserveUserUnits) {
												params.setPreserveUnits(false);
											}
											String unit = params.getUnit(key);
											try {
												double val = Double.parseDouble(txf.getText());
												params.set(key, val, unit);
											} catch (NumberFormatException e){
												params.set(key, txf.getText());
											}
											if (preserveUserUnits) {
												params.setPreserveUnits(presUnits);
												if (!Units.isCompatible(unit, params.getUnit(key))) {
													f.pln("ParameterPanel: WARNING! "+unit+" is not compatible with "+params.getUnit(key));
												}
											}
										} else {
											params.set(key, txf.getText());
										}
										saveData(key);
										updateTextEntry(key);
										if (showModifiedColor) txf.setForeground(modifiedColor);					
										updateDisplays(key);
									}// actionPerformed
								});
								textEntries.put(key,txf);
								c.gridx = nextCol();
								c.gridy = ROW;
								c.fill = GridBagConstraints.NONE;
								c.weightx = 0.0;
								this.add(lab,c);
								c.gridx = nextCol();
								c.gridy = ROW;
								c.fill = GridBagConstraints.HORIZONTAL;
								c.weightx = 1.0;
								this.add(txf,c);
							}
						}
						if (showUnits) {
							String unit = params.getUnit(key);
							if (params.isNumber(key) 
									&& !unit.equals("unitless") && !unit.equals("unspecified") && !unit.equals("internal")) {
								c.gridx = nextCol();
								c.gridy = ROW;
								c.fill = GridBagConstraints.NONE;
								c.weightx = 0.0;
								if (pulldownUnits) {
									final JComboBox<String> ucb = new JComboBox<String>(Units.getCompatibleUnits(unit));
									ucb.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
									ucb.setSelectedItem(unit);
									ucb.addActionListener(new ActionListener() {
										public void actionPerformed(ActionEvent event) {
											if (!inUpdate) {
												inUpdate = true;
												String sel = ucb.getSelectedItem().toString();
												log(key+" unit to "+sel);
												boolean presUnits = params.isPreserveUnits();
												if (preserveUserUnits) {
													params.setPreserveUnits(false);
												}
												double val = Units.to(params.getUnit(key), params.getValue(key)); //user-visible value
												params.set(key, val, sel); // set user-visible value in new units
												if (preserveUserUnits) {
													params.setPreserveUnits(presUnits);
												}
												saveData(key);
												updateOne(key); // not sure which one it is
												updateDisplays(key);
												// do not change foreground color for units combobox (update does not change them back)					
												inUpdate = false;
											}
										}
									});
									unitEntries.put(key, ucb);
									this.add(ucb,c);
								} else {
									JLabel ulab = new JLabel(unit);
									ulab.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
									unitEntries.put(key, ulab);
									this.add(ulab,c);			
								}
							} else {
								blankSpace();
							}
						}
					}
				}
			}
			if (addNewParamButton) {
				newRow();
				c.gridx = nextCol();
				c.gridy = ROW;
				c.fill = GridBagConstraints.NONE;
				c.gridwidth = 1;
				c.weightx = 0.0;
				this.add(new JLabel("Add Param:"),c);
				final JTextField addfield = new JTextField(10);
				addfield.addActionListener( new ActionListener() {
					public void actionPerformed(ActionEvent event) {
						String key = addfield.getText();
						if (!key.equals("")) {
							log("add "+key);
							List<String> keys0 = params.getList();
							params.set(key);
							List<String> keys1 = params.getList();
							addfield.setText("");
							if (keys1.size() > keys0.size()) {
								for (int i = 0; i < keys1.size(); i++) {
									if (!keys0.contains(keys1.get(i)) && keyList != null) {
										keyList.add(keys1.get(i));
									}
								}
								setup();
							}
							saveData("add "+key);
							updateDisplays("add "+key);
						}
					}// actionPerformed
				});
				c.gridx = nextCol();
				c.gridy = ROW;
				c.fill = GridBagConstraints.HORIZONTAL;
				c.gridwidth = GridBagConstraints.REMAINDER;
				this.add(addfield,c);
			}
			revalidate();
			if (keyList != null) {
				oldKeyList = (List<String>)keyList.clone();
			}
			dirty = false;
		}
	}

	private void setValue(JComponent jc, double val) {
		if (jc instanceof JTextField) {
			if (val == Math.floor(val)) {
				((JTextField)jc).setText(f.Fm0(val));
			} else {
				((JTextField)jc).setText(f.FmPrecision(val,precision));
			}
		} else if (jc instanceof JLabel) {
			if (val == Math.floor(val)) {
				((JLabel)jc).setText(f.Fm0(val));
			} else {
				((JLabel)jc).setText(f.FmPrecision(val,precision));
			}			  
		} else if (jc instanceof JComboBox) {
			if (val == Math.floor(val)) {
				((JComboBox<?>)jc).setSelectedItem(f.Fm0(val));
			} else {
				((JComboBox<?>)jc).setSelectedItem(f.FmPrecision(val,precision));
			}
		} else if (jc instanceof JSpinner) {
			((JSpinner)jc).setValue(val);
			//		} else if (jc instanceof JSlider) {
			//			((JSlider)jc).setValue((int)val);
		}
	}

	private void setValue(JComponent jc, String val) {
		if (jc instanceof JTextField) {
			((JTextField)jc).setText(val);
		} else if (jc instanceof JLabel) {
			((JLabel)jc).setText(val);
		} else if (jc instanceof JComboBox) {
			((JComboBox<?>)jc).setSelectedItem(val);
		} else if (jc instanceof JSpinner) {
			((JSpinner)jc).setValue(val);
			//		} else if (jc instanceof JSlider) {
			//			// do nothing
		}
	}

	private void updateMiscEntry(String key) {
		inUpdate = true;
		if (params.isNumber(key) ) {
			double val = Units.to(params.getUnit(key), params.getValue(key));
			setValue(miscEntries.get(key), val);
			String unit = params.getUnit(key);
			if (unitEntries.containsKey(key)) {
				if (unitEntries.get(key) instanceof JLabel) {
					((JLabel)unitEntries.get(key)).setText(unit);
				} else if (unitEntries.get(key) instanceof JComboBox) {
					((JComboBox<?>)unitEntries.get(key)).setSelectedItem(unit);
				}
			}
		} else {
			setValue(miscEntries.get(key), params.getString(key));
		}
		miscEntries.get(key).setEnabled(enabled);
		inUpdate = false;

	}

	private void updateBoolEntry(String key) {
		inUpdate = true;
		boolEntries.get(key).setSelected(params.getBool(key));
		boolEntries.get(key).setEnabled(enabled);
		inUpdate = false;	
	}

	private void updateTextEntry(String key) {
		inUpdate = true;
		if (params.isNumber(key) ) {
			double val = Units.to(params.getUnit(key), params.getValue(key));
			setValue(textEntries.get(key), val);
			String unit = params.getUnit(key);
			if (unitEntries.containsKey(key)) {
				if (unitEntries.get(key) instanceof JLabel) {
					((JLabel)unitEntries.get(key)).setText(unit);
				} else if (unitEntries.get(key) instanceof JComboBox) {
					((JComboBox<?>)unitEntries.get(key)).setSelectedItem(unit);
				}
			}
		} else {
			setValue(textEntries.get(key), params.getString(key));
		}
		textEntries.get(key).setEnabled(enabled);
		inUpdate = false;
	}

	/**
	 * Update value for one entry with that held in the parameter data object.
	 * This is intended for code internal to ActionListeners associated with a specific entry.
	 */
	protected void updateOne(String key) {
		if (miscEntries.containsKey(key)) {
			updateMiscEntry(key);
		} else if (boolEntries.containsKey(key)) {
			updateBoolEntry(key);
		} else if (textEntries.containsKey(key)) {
			updateTextEntry(key);
		}
	}

	/**
	 * Update the values in the panel with those held in the parameter data object.
	 */
	@SuppressWarnings("unchecked")
	public void update() {
		for (String key : miscEntries.keySet()) {
			updateMiscEntry(key);
		}
		for (String key : boolEntries.keySet()) {
			updateBoolEntry(key);
		}
		for (String key : textEntries.keySet()) {
			updateTextEntry(key);
		}
		resetColors();
	}

	/**
	 * Return all entry colors to the default color.  This has no effect if the showModifiedColors flag is not true.
	 */
	public void resetColors() {
		if (showModifiedColor) {
			for (String key : miscEntries.keySet()) {
				miscEntries.get(key).setForeground(normalColor);
			}
			for (String key : boolEntries.keySet()) {
				boolEntries.get(key).setForeground(normalColor);			
			}
			for (String key : textEntries.keySet()) {
				textEntries.get(key).setForeground(normalColor);
			}
		}
	}

	private void processTextField(JTextField entry, String key) {
		if (entry != null && entry.isEnabled()) {
			String text = entry.getText();
			String value = params.getString(key);
			if (params.isNumber(key) ) {
				double val = Units.to(params.getUnit(key), params.getValue(key));
				if (val == Math.floor(val)) {
					value = f.Fm0(val);		
				} else {
					value = f.FmPrecision(val,precision);
				}
			} 
			if (!text.equals(value)) {
				entry.postActionEvent();
			}
		}
	}

	//TODO test this
	private void processComboBox(JComboBox<?> entry, String key) {
		if (entry.isEditable()) {
			String text = entry.getEditor().getItem().toString();
			String value = params.getString(key);
			if (params.isNumber(key) ) {
				double val = Units.to(params.getUnit(key), params.getValue(key));
				if (val == Math.floor(val)) {
					value = f.Fm0(val);		
				} else {
					value = f.FmPrecision(val,precision);
				}
			} 
			if (!text.equals(value)) {
				entry.dispatchEvent(new ActionEvent(this,ActionEvent.ACTION_PERFORMED,null));
			}
		}
	}

	//TODO test this
	private void processSpinner(JSpinner entry, String key) {
		if (entry.getEditor() instanceof JSpinner.DefaultEditor) {
			JTextField tf = ((JSpinner.DefaultEditor)entry.getEditor()).getTextField();
			if (tf.isEditable()) {
				String text = tf.getText();
				String value = params.getString(key);
				if (params.isNumber(key) ) {
					double val = Units.to(params.getUnit(key), params.getValue(key));
					if (val == Math.floor(val)) {
						value = f.Fm0(val);		
					} else {
						value = f.FmPrecision(val,precision);
					}
				} 
				if (!text.equals(value)) {
					entry.dispatchEvent(new ActionEvent(this,ActionEvent.ACTION_PERFORMED,null));
				}
			}
		}
	}

	/**
	 * This reads in the current text in each text field and programmatically sets each that does not match its stored value
	 * by triggering its ActionListener. This is intended to allow for something like a "done" button that updates all 
	 * values with current user input, even if the enter key has not been pressed for some of them.
	 * Note: This may not work properly if the subclass' saveData() method calls setParameters using the entire params object.  (It will work only the needed subset of params is used.)   
	 * This may also work with some editable JSpinners and JComboBoxes. 
	 */
	public void processAll() {
		for (String key : textEntries.keySet()) {
			JTextField entry = textEntries.get(key);
			processTextField(entry, key);
		}
		for (String key : miscEntries.keySet()) {
			JComponent entry = miscEntries.get(key);
			if (entry instanceof JTextField) {
				processTextField((JTextField)entry, key);
			} else if (entry instanceof JComboBox<?>) {
				processComboBox((JComboBox<?>)entry, key);
			} else if (entry instanceof JSpinner) {
				processSpinner((JSpinner)entry, key);
			}
		}
	}

}
