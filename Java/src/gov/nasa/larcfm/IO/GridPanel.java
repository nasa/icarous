/*
 * Copyright (c) 2013-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.JPanel;

public class GridPanel extends JPanel {
	private static final long serialVersionUID = 1L;
	
	protected int ROW;
	  protected int COL;
	  protected int LASTCOL;
	  protected GridBagConstraints c;
	  
	  protected void newRow() {
		  ROW++;
		  COL = 0;
	  }
	  
	  protected void blankSpace() {
		  COL++;
	  }
	  
	  protected int nextCol() {
		  if (COL > LASTCOL && LASTCOL > 0) LASTCOL = COL;
		  return COL++;
	  }

	  public GridPanel() {
		  super();
		  ROW = 0;
		  COL = 0;
		  LASTCOL = -1;
		  setLayout(new GridBagLayout() );
		  c = new GridBagConstraints();
		  c.fill = GridBagConstraints.BOTH;
		  c.insets = new Insets(0, 5, 0, 5);
		  c.gridx = COL;
		  c.gridy = ROW;
	  }

	  public GridPanel(int lastColumn) {
		  super();
		  ROW = 0;
		  COL = 0;
		  LASTCOL = lastColumn;
		  setLayout(new GridBagLayout() );
		  c = new GridBagConstraints();
		  c.fill = GridBagConstraints.BOTH;
		  c.insets = new Insets(0, 5, 0, 5);
		  c.gridx = COL;
		  c.gridy = ROW;
	  }

	  public GridPanel(int lastColumn, Insets ins) {
		  super();
		  ROW = 0;
		  COL = 0;
		  LASTCOL = lastColumn;
		  setLayout(new GridBagLayout() );
		  c = new GridBagConstraints();
		  c.fill = GridBagConstraints.BOTH;
		  c.insets = ins;
		  c.gridx = COL;
		  c.gridy = ROW;
	  }

	  public void addCell(Component x) {
		  c.gridx = nextCol();
		  c.gridy = ROW;
		  add(x,c);
	  }

	  public void addCell(Component x, int width) {
		  c.gridx = nextCol();
		  c.gridy = ROW;
		  c.gridwidth = width;
		  add(x,c);
		  COL += (width-1);
		  c.gridwidth = 1;
	  }

	  public void addCell(Component x, int width, int height) {
		  c.gridx = nextCol();
		  c.gridy = ROW;
		  c.gridwidth = width;
		  c.gridheight = height;
		  add(x,c);
		  COL += (width-1);
		  c.gridwidth = 1;
		  c.gridheight = 1;
	  }
	  
	  public void addCellAt(Component z, int x, int y) {
		  c.gridx = x;
		  c.gridy = y;
		  add(z,c);
	  }

	  public void nextRow() {
		 newRow();
	  }
	  
}
