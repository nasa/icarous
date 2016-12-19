/*
 * ViewPort - a graphics pane that can draw in a field of doubles (scaling is automatic)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;


import gov.nasa.larcfm.Util.BoundingRectangle;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.lang.Math;
import java.awt.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

import javax.swing.*;

import java.util.LinkedList;
import java.util.List;

/**
 * This class provides a convenient way to draw objects specified in a user
 * coordinate system (say nautical miles) and have these coordinates be
 * automatically scaled into screen coordinates. The basic use is to inherit
 * from this class, then overload the paintComponent method.
 * <p>
 * 
 * A series of convenience methods are provided to draw objects on the screen
 * (in user coordinates). Some of these methods contain both user coordinates
 * (specified as Vect2 or as doubles), and screen coordinates (expressed as
 * int). This difference is significant. Something specified in screen
 * coordinates will be that size no matter what the scaling is. On the other
 * hand, something specified in user coordinates will change its size depending
 * on the scaling.
 * <p>
 * 
 * When the panel is resized (say by the user dragging the edges of the window),
 * these resizing events are handled automatically, as follows: if the "stretch"
 * parameter is set true (on construction) then as the window is resized the objects
 * in the window change size; if stretch is false, then the scale factors are kept
 * the same and the displayed area increases/decreases. The scale factors may be
 * set by the setScale method or the setUserArea method. The setCenter method
 * can be used to "pan" the display (move the displayed area
 * up/down/left/right).
 * <p>
 * 
 */
public abstract class ViewPort extends JPanel {

	static final long serialVersionUID = 0;

	public static final double Default_Object_Size = 80;
	public static final int    Default_Point_Radius = 5;
	public static final int    Default_Triangle_Size = 7;
	public static final int    Default_Arrowhead_size = 12;

	private static final double[][] chevron = {{0,1,0,-1},{0.5,1,-1.5,1}};
	private static final double[][] square  = {{-1.5,1.5,1.5,-1.5},{1.5,1.5,-1.5,-1.5}};

	private final boolean stretch;
	private double xScale;    // units: screen / user
	private double yScale;    // units: screen / user
	private double center_x;
	private double center_y;
	private double llx; // lower left x-coordinate
	private double ury; // upper right y-coordinate
	private BoundingRectangle screen;

	// single background image
	private BufferedImage image;
	private double img_llx; // lower left x-coordinate
	private double img_lly; // lower left y-coordinate
	private double img_urx; // upper right x-coordinate
	private double img_ury; // upper right y-coordinate

	private BufferedImage[][] imageTiles; // background image broken down into tiles
	private List<BufferedImage> downsampledImages; // downsized versions of background image
	private static int downsampleFactor = 2; // factor for downsampling and tiling, should probably be a power of 2

	private int numXaxis;
	private boolean gridX;
	private String xUnit;
	private int numYaxis;
	private boolean gridY;
	private String yUnit;

	public int xGridPrecision = 0;
	public int yGridPrecision = 0;



	private boolean firstTry;

	// These are just convenience fields for pre-existing colors
	public static final Color red = Color.red;
	public static final Color blue = Color.blue;
	public static final Color green = Color.green;
	public static final Color orange = Color.orange;
	public static final Color gray = Color.gray;
	public static final Color black = Color.black;
	public static final Color yellow = Color.yellow;

	// These are custom colors
	public static final Color amber = new Color(255,69,0); 
	public static final Color lightGreen = new Color(50, 255, 200);
	public static final Color lightBlue = new Color(50, 200, 255);
	public static final Color lightGray = new Color(235,235,235);
	public static final Color lightPink = new Color(255,210,210);
	public static final Color lightYellow = new Color(255,255,210);
	public static final Color medGreen = new Color(0, 200, 100);
	public static final Color medBlue = new Color(0, 150, 200);
	public static final Color brown     = new Color(128, 64, 64 );
	public static final Color purple = new Color(100, 0, 170);
	public static final Color teal      = new Color(0, 222, 209 );
	public static final Color kellyGreen = new Color(0,200, 0 );
	public static final Color burgandy  = new Color(200, 000, 0 );
	public static final Color medRed    = new Color(230, 000, 0 );
	public static final Color redOrange    = new Color(250, 130, 70 );
	public static final Color peach     = new Color(231,177,62,88);
	public static final Color indigo    = new Color(75, 0, 130 );
	public static final Color olive     = new Color(107,142,35);
	public static final Color skyBlue   = new Color(135,206,235);
	public static final Color tan       = new Color(210,180,140);
	public static final Color lightTan  = new Color(235,215,200);
	public static final Color salmon    = new Color(233,150,122);
	public static final Color beige     = new Color(245,245,220);
	public static final Color gold      = new Color(255,215,0);
	public static final Color lavender  = new Color(255,240,245);
	public static final Color darkGreen = new Color( 0, 100, 0 );
	public static final Color darkBlue  = new Color( 75, 0, 150 );
	public static final Color darkDarkBlue  = Color.BLUE.darker();  // probably not darker than darkBlue
	public static final Color transBlack = new Color(0,0,0,150);
	public static final Color darkYellow = new Color(215,205,0);
	public static final Color royalBlue = new Color(20,110,210);
	public static final Color medPurple = new Color(180,0,200);

	public static final Color azure     = new Color(240,255,255);    // ltGreen
	public static final Color wheat     = new Color(245,222,179);
	public static final Color darkMegenta = new Color(139,0,139);
	public static final Color plum        = new Color(221,160,221);
	public static final Color darkViolet  = new Color(148,0,211);
	public static final Color darkRed     = new Color(139,0,0);
	public static final Color deepSkyBlue = new Color(0,191,255);

	// Fonts
	public static final Font Lucida13 = new Font("Lucida Grande", Font.PLAIN, 13);
	public static final Font Lucida16 = new Font("Lucida Grande", Font.PLAIN, 16);
	public static final Font LucidaBold13 = new Font("Lucida Grande", Font.BOLD, 13);
	public static final Font LucidaBold16 = new Font("Lucida Grande", Font.BOLD, 16);
	public static final Font LucidaItalic13 = new Font("Lucida Grande", Font.ITALIC, 13);
	public static final Font LucidaItalic16 = new Font("Lucida Grande", Font.ITALIC, 16);

	public static final Font DEFAULT_FONT = Lucida13;

	/* temporary variables for efficiency */
	private double horiz_axis_x;
	private double horiz_axis_y;
	private double vert_axis_x;
	private double vert_axis_y;
	private double grid1_x;
	private double grid1_y;
	private double grid2_x;
	private double grid2_y;




	//
	// Constructors
	//

	/** Construct a ViewPort */
	public ViewPort() {
		stretch = false;
		staticDataInitializer();
		init();
	}

	/** 
	 * Construct a ViewPort.   
	 * @param stretch If true then as the window is resized the 
	 * graphical display will stretch in and out.  If false, then 
	 * as the window is resized more area will be displayed with
	 * the same scaling.
	 */
	public ViewPort(boolean stretch) {
		this.stretch = stretch;
		staticDataInitializer();
		init();
	}

	private void init() {
		center_x = 0.0;
		center_y = 0.0;
		screen = new BoundingRectangle();

		setCenter(0.0, 0.0);
		setScale(1.0, 1.0);

		numXaxis = 0;
		numYaxis = 0;
		gridX = false;
		gridY = false;

		xUnit = "NM";
		yUnit = "NM";

		firstTry = true;

		setBackground(Color.white);
	}

	//
	// Abstract Methods
	//
	public abstract void setCenter(Position ss);
	/** Take a position and convert it to a screen coordinate in the X (horizontal) direction */
	public abstract int screenMapX(Position p);
	/** Take a position and convert it to a screen coordinate in the Y (vertical) direction */
	public abstract int screenMapY(Position p);
	/** Take a position and put it into a Vect2 (but retain the user coordinates) */
	public abstract Vect2 position2Vect(Position p);
	/** Return the components of a velocity that are in the screen's plane.	 */
	public abstract Vect2 vel2Vect(Velocity v);
	/** 
	 * Given x and y in screen coordinates create a position object.  To compute
	 * a Position, some estimate must be taken (fundamentally, moving from 2-space to
	 * 3-space involves some estimation.
	 * @param x horizontal screen coordinate
	 * @param y vertical screen coordinate
	 * @return an estimate of the position on the screen
	 */
	public abstract Position inversePosition(int x, int y);

	//
	// Methods the probably should be overridden by inheriting classes
	//

	/**
	 * This method scales (or rescales) the display and sets the center to the middle.  This
	 * is usually invoked after a new dataset has been provided.  
	 */
	public void setScaleAndCenterOnMiddle() {
		// Do nothing
	}
	public int pickPoint(int sx, int sy) {
		return 0;
	}
	public int checkPoint(int sx, int sy) {
		return 0;
	}


	//
	// Main paint method
	//

	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		if (firstTry) setDefaults(g);

		//
		// Set scaling parameters
		//

		int xSize = getWidth();
		int ySize = getHeight();

		if (stretch) {
			xScale = (xSize / 2) / (center_x - llx);
			yScale = (ySize / 2) / (ury - center_y);
		} else {
			llx = center_x - (xSize / 2) / xScale;
			ury = (ySize / 2) / yScale + center_y;
		}

		//
		// Draw the background image
		//

		if (image != null) {
			drawImage(g, img_llx, img_lly, img_urx, img_ury, image, imageTiles, downsampledImages);
		}

		//
		// Draw the axis
		//

		if (numXaxis != 0 || numYaxis != 0) {
			double lly = center_y - getRangeY() / 2;
			double urx = center_x + getRangeX() / 2;

			int stepsX = numXaxis + 1; // an extra one for padding
			int stepsY = numYaxis + 1; // an extra one for padding
			double incX = getRangeX() / stepsX;
			double incY = getRangeY() / stepsY;
			vert_axis_x = llx;
			horiz_axis_y = lly;
			for (int i = 1; i < stepsX; i++) {
				horiz_axis_x = incX * i + llx;

				if (gridX) {
					g.setColor(Color.lightGray);
					grid1_x = horiz_axis_x;
					grid2_x = horiz_axis_x;
					grid1_y = lly;
					grid2_y = ury;
					drawLine(g, new Vect2(grid1_x, grid1_y), new Vect2(grid2_x, grid2_y));
				}

				g.setColor(Color.black);
				drawString(g, f.FmPrecision(Units.to(xUnit,	horiz_axis_x),xGridPrecision), horiz_axis_x, horiz_axis_y, 0, -1);
			}

			for (int i = 1; i < stepsY; i++) {
				vert_axis_y = incY * i + lly;

				if (gridY) {
					g.setColor(Color.lightGray);
					grid1_x = llx;
					grid2_x = urx;
					grid1_y = vert_axis_y;
					grid2_y = vert_axis_y;
					drawLine(g, new Vect2(grid1_x, grid1_y), new Vect2(grid2_x, grid2_y));
				}

				g.setColor(Color.black);
				drawString(g, f.FmPrecision(Units.to(yUnit, vert_axis_y),yGridPrecision), vert_axis_x, vert_axis_y, -1, 0);
			}
		}
	}


	//
	// Static Utility Methods
	//

	/** Return a string name for the given color */
	public static String C(Color xx) {                    // For debug
		String rtn = "";
		if (xx == Color.red || xx == red) rtn = "red";
		else if (xx == medGreen  ) rtn = "medGreen";
		else if (xx == medBlue   ) rtn = "medBlue";
		else if (xx == brown     ) rtn = "brown ";
		else if (xx == purple    ) rtn ="purple";
		else if (xx == teal      ) rtn ="teal";
		else if (xx == kellyGreen) rtn ="kellyGreen";
		else if (xx == burgandy  ) rtn ="burgandy";
		else if (xx == indigo    ) rtn ="indigo";
		else if (xx == olive     ) rtn ="olive";
		else if (xx == skyBlue   ) rtn ="skyBlue";
		else if (xx == royalBlue ) rtn ="royalBlue";
		else if (xx == tan       ) rtn ="tan";
		else if (xx == salmon    ) rtn ="salmon";
		else if (xx == beige     ) rtn ="beige";
		else if (xx == gold      ) rtn ="gold"; 
		else if (xx == lightTan  ) rtn ="lightTan"; 
		else if (xx == lavender  ) rtn = "lavender";
		else if (xx == darkGreen ) rtn = "darkGreen";
		else if (xx == darkBlue  ) rtn ="darkBlue";
		else if (xx == transBlack) rtn ="transBlack";
		else if (xx == redOrange ) rtn ="redOrange";
		else if (xx == Color.blue || xx == blue) rtn = "blue";
		else if (xx == Color.green || xx == green) rtn = "green";
		else if (xx == Color.orange || xx == orange) rtn = "orange";
		else if (xx == Color.gray || xx == gray) rtn = "gray";
		else if (xx == Color.black || xx == black) rtn = "black";
		else rtn = "???";
		return rtn;
	}


	private static final Color[] colors = new Color[100];

	// note this can result in overflows with large i values:
	private static Color colorCreator(int j) {
		long i = Math.abs(j);
		int r = (int)((i*3+i*53)%199);
		int g = (int)((i*5+i*31)%151);
		int b = (int)((i*7+i*13)%127);
		return new Color(r+25, g+50, b+70);
	}


	private void staticDataInitializer() {
		colors[0] = blue;
		colors[1] = olive;
		colors[2] = teal;
		for (int i = 3; i < colors.length; i++) {
			colors[i] = colorCreator(i).darker();
		}
	}

	/** Return an arbitrary color from an index.
	 * The color is <b>not</b> random, getColor(372) will 
	 * always be the same color no matter how many times you 
	 * call getColor(372).
	 * 
	 * @param i the color index.  If i==0, then blue is returned, if 
	 * i==1 then olive is returned, if i==2 then teal is returned.  For other indexes 
	 * an arbitrary color is returned.  
	 * @return a color
	 */
	public static Color getColor(int i) {
		if (i < 0) i = 0;
		if (i >= colors.length) {
			return colorCreator(i);
		}
		return colors[i];
	}


	/**
	 * Given a string, return some arbitrary color.  The given string is NOT the 
	 * name of the color.  getColor("Blue") will NOT give the expected results.  Instead
	 * this method, given the same string, will always return the same color. 
	 * 
	 * @param s a string
	 * @return a color
	 */
	public static Color getColor(String s) {
		return getColor(Math.abs(s.hashCode()));
	}

	/** Make a new color with the specified transparency value (alpha)
	 * 
	 * @param c the color
	 * @param alpha the transparency value
	 * @return the color with the new transparency value
	 */
	public static Color makeColorAlpha(Color c, int alpha) {
		if (alpha <0 || alpha > 255) return c;
		return new Color(c.getRed(),c.getGreen(),c.getBlue(),alpha);
	}


	//
	// Utility Methods
	//

	public void setBackgroundImage(BufferedImage image, double lowerLeftX, double lowerLeftY, double upperRightX, double upperRightY) {
		this.image = image;
		img_llx = lowerLeftX;
		img_lly = lowerLeftY;
		img_urx = upperRightX;
		img_ury = upperRightY;
		imageTiles = null;
		downsampledImages = null;
	}
	
	/**
	 * Break up the current background image into a series of tiles.  This is intended to speed up drawing the screen with
	 * very large background images by calling the versions of drawImage() that include the tile array and downsampled list in addition to the base image.
	 * @param tilesize size (in pixels) for the tiles.  Set to zero to remove tiles.
	 */
	public void setTileImage(int tilesize) {
		imageTiles = tileImage(image, tilesize);
	}
		
	/**
	 * Break up a large image into an array of tiles suitable to be used with the versions of drawImage that include the tile array and downsampled list in addition to the base image. 
	 * @param img base image
	 * @param tilesize size of tiles, in pixels
	 * @return array of tile pictures.  These may use that same backend data as the base image (so changing data in one may change the other).
	 */
	public static BufferedImage[][] tileImage(BufferedImage img, int tilesize) {
		if (tilesize > 0 && img != null && img.getWidth() > tilesize || img.getHeight() > tilesize) {
			int szx = img.getWidth()/tilesize;
			int szy = img.getHeight()/tilesize;

			if (szx*tilesize < img.getWidth()) szx++;
			if (szy*tilesize < img.getHeight()) szy++;

			BufferedImage tiles[][] = new BufferedImage[szx][szy];

			for (int x = 0; x < szx; x++) {
				int xx = x*tilesize;
				for (int y = 0; y < szy; y++) {
					int yy = y*tilesize;
					BufferedImage img2 = img.getSubimage(xx, yy, Util.min(tilesize,img.getWidth()-xx), Util.min(tilesize, img.getHeight()-yy));
//					BufferedImage img3 = new BufferedImage(tilesize, tilesize, BufferedImage.TYPE_INT_ARGB);
//					Graphics2D g2d = img3.createGraphics();
//					g2d.drawImage(img2, 0, 0, null);
//					g2d.dispose();
					tiles[x][y] = img2;
				}
			}
			return tiles;
		} else {
			return null;
		}
	}

	/**
	 * Provide downsampled versions of the background image to help speed up zooming and drawing the screen.
	 * This is intended to speed up drawing the screen with
	 * very large background images by calling the versions of drawImage() that include the tile array and downsampled list in addition to the base image.
	 * @param steps number of downsampling "steps" to include.  Set to zero to remove downsampled versions.
	 * The resulting list will have the lowest resolution version first and the full resolution image last
	 */
	public void setDownsampledImages(int steps) {
		downsampledImages = downsampleImage(image, steps, downsampleFactor);
	}
	
	/**
	 * Create a list of downsampled versions of an image suitable to be used with the versions of drawImage that include the tile array and downsampled list in addition to the base image. 
	 * @param img base image
	 * @param steps number of times to apply downsampling
	 * @param downsampleFactor factor to downsample by
	 * @return List of images in increasing resolution order.  This will not include the the original image.
	 */
	public static List<BufferedImage> downsampleImage(BufferedImage img, int steps, int downsampleFactor) {
		if (steps > 0 && img != null && downsampleFactor > 1) {
			BufferedImage prevImg = img;
			List<BufferedImage> list = new LinkedList<BufferedImage>();
//			downsampledImages.add(image);
			for (int i = 0; i < steps; i++) {
				int x = prevImg.getWidth()/downsampleFactor;
				int y = prevImg.getHeight()/downsampleFactor;
				if ( x > 0 && y > 0) {
					Image img2 = prevImg.getScaledInstance(x, y, Image.SCALE_SMOOTH);
					BufferedImage img3 = new BufferedImage(x, y, BufferedImage.TYPE_INT_ARGB);
					Graphics2D g2d = img3.createGraphics();
					g2d.drawImage(img2, 0, 0, null);
					g2d.dispose();
					list.add(0,img3);
					prevImg = img3;
				} else {
					break;
				}
			}
			return list;
		} else {
			return null;
		}
	}

	public BoundingRectangle getScreen() {
		return screen;
	}

	/**
	 * Enter the number of x axis and y axis divisions and turn on/off the
	 * display of a grid. To turn off the axis enter 0 for the number of x/y
	 * axis divisions.
	 */
	public void setAxis(int numXaxis, boolean gridX, String xUnit,
			int numYaxis, boolean gridY, String yUnit) {
		this.numXaxis = numXaxis;
		this.gridX = gridX;
		this.xUnit = xUnit;

		this.numYaxis = numYaxis;
		this.gridY = gridY;
		this.yUnit = yUnit;
	}


	/**
	 * Set the scale of the ViewPort in pixels per user unit and the center of the
	 * screen in user coordinates.  This method is faster than individual calls
	 * to setScale() and setCenter().  <p>
	 * 
	 * Note: If both x and y dimensions have the same unit, then these scale factors
	 * should probably be the same. The graphics will not fill the plot area,
	 * but resizing will give confusing results to the user (circles will become
	 * ovals, etc.).
	 */
	public void setScaleAndCenter(double scaleX, double scaleY, double centerX, double centerY) {
		xScale = scaleX;
		yScale = scaleY;
		center_x = centerX;
		center_y = centerY;

		//    f.pln(" $$$$ viewport.setScale: scaleX = "+scaleX);
		//    f.pln(" $$$$ viewportsetScale: scaleY = "+scaleY);

		int xSize = getWidth();
		int ySize = getHeight();

		//    f.pln(" $$$$ viewport.setScale: xSize = "+xSize);
		//    f.pln(" $$$$ viewport.setScale: ySize = "+ySize);

		llx = center_x - (xSize / 2) / xScale;
		ury = center_y + (ySize / 2) / yScale;

		screen.clear();
		screen.add(llx,center_y - (ySize / 2) / yScale);
		screen.add(center_x + (xSize / 2) / xScale,ury);

		repaint();
	}

	/**
	 * Set the scale of the ViewPort in pixels per user unit.<p>
	 * Note: If both x and y dimensions have the same unit, then these scale factors
	 * should probably be the same. The graphics will not fill the plot area,
	 * but resizing will give confusing results to the user (circles will become
	 * ovals, etc.).
	 */
	public void setScale(double scaleX, double scaleY) {
		setScaleAndCenter(scaleX, scaleY, center_x, center_y);
	}

	/** Sets (in user coordinates) the center of the screen */
	public void setCenter(double centerX, double centerY) {
		setScaleAndCenter(xScale, yScale, centerX, centerY);
	}

	/**
	 * This method computes appropriate scale factors to put the given user area
	 * within the window. If in "stretch" mode (see constructor), then the scale
	 * factors are set to stretch the user area into the window size. If not in
	 * "stretch" mode, then a new user coordinate area is made that completely
	 * contains the given area. This new area is bigger to accommodate the aspect
	 * ratio of the window.
	 * <p>
	 * 
	 * Essentially, this is a convenience method to set the scale factors for a
	 * given display area (in user coordinates). This also sets the center to be
	 * in the middle of this area.
	 * <p>
	 * 
	 * @param lowerleftx  the X coordinate of the lower left corner of the user area
	 * @param lowerlefty  the Y coordinate of the lower left corner of the user area
	 * @param upperrightx the X coordinate of the upper right corner of the user area
	 * @param upperrighty the Y coordinate of the upper right corner of the user area
	 */
	public void setUserArea(double lowerleftx, double lowerlefty,
			double upperrightx, double upperrighty) {
		setUserArea(lowerleftx, lowerlefty, upperrightx, upperrighty, 0, 0, 0, 0);

	}

	/**
	 * This method computes appropriate scale factors to put the given user area
	 * within the window. If in "stretch" mode (see constructor), then the scale
	 * factors are set to stretch the user area into the window size. If not in
	 * "stretch" mode, then a new user coordinate area is made that completely
	 * contains the given area. This new area is bigger to accommodate the aspect
	 * ratio of the window.
	 * <p>
	 * 
	 * Essentially, this is a convenience method to set the scale factors for a
	 * given display area (in user coordinates). This also sets the center to be
	 * in the middle of this area.
	 * <p>
	 * 
	 * @param lowerleftx  the X coordinate of the lower left corner of the user area
	 * @param lowerlefty  the Y coordinate of the lower left corner of the user area
	 * @param upperrightx the X coordinate of the upper right corner of the user area
	 * @param upperrighty the Y coordinate of the upper right corner of the user area
	 * @param inset_north number of pixels to reserve at the top of the screen
	 * @param inset_east number of pixels to reserve at the right side of the screen
	 * @param inset_south number of pixels to reserve at the bottom of the screen
	 * @param inset_west number of pixels to reserve at the left side of the screen
	 */
	public void setUserArea(double lowerleftx, double lowerlefty,
			double upperrightx, double upperrighty, 
			int inset_north, int inset_east, int inset_south, int inset_west) {
		int xSize = Util.max(1, getWidth());
		int ySize = Util.max(1, getHeight());

		// Avoid divide by zero
		if (upperrightx - lowerleftx < 1e-10) {
			upperrightx += 0.5;
			lowerleftx -= 0.5;
		}
		if (upperrighty - lowerlefty < 1e-10) {
			upperrighty += 0.5;
			lowerlefty -= 0.5;
		}

		double tScaleX = (xSize - inset_east - inset_west) / (upperrightx - lowerleftx);
		double tScaleY = (ySize - inset_north - inset_south) / (upperrighty - lowerlefty);

		double cenX = (upperrightx + lowerleftx) / 2 + (inset_east - inset_west) / tScaleX / 2;
		double cenY = (upperrighty + lowerlefty) / 2 + (inset_north - inset_south) / tScaleY / 2;

		if (stretch) {
			setScaleAndCenter(tScaleX, tScaleY, cenX, cenY);
		} else {
			double war = (double) xSize / (double) ySize; // window aspect ratio
			double uar = (upperrightx - lowerleftx)
					/ (upperrighty - lowerlefty); // user area aspect ratio

			if (war < uar) {
				setScaleAndCenter(tScaleX, tScaleX, cenX, cenY);
			} else {
				setScaleAndCenter(tScaleY, tScaleY, cenX, cenY);
			}
		}
	}

	public double getScaleX() {
		return xScale;
	}

	public double getScaleY() {
		return yScale;
	}

	/**
	 * How to change the scales when the user requests to zoom in
	 */
	public void zoomIn() {} // should be overridden by the deriving class

	/**
	 * How to change the scales when the user requests to zoom out. 
	 */
	public void zoomOut() {} // should be overridden by the deriving class


	/** return the center of the screen in user coordinates */
	public Vect2 getCenter() {
		return new Vect2(center_x,center_y);
	}
	public double getCenterX() {
		return center_x;
	}
	public double getCenterY() {
		return center_y;
	}


	public double getRangeX() {
		int xSize = getWidth();
		return xSize / xScale;
	}

	public double getRangeY() {
		int ySize = getHeight();
		return ySize / yScale;
	}


	/** user coordinate to screen coordinate */
	public int screenMapX(double x) {
		return (int) ((x - llx) * xScale);
	}

	/** user coordinate to screen coordinate */
	public int screenMapY(double y) {
		return (int) ((ury - y) * yScale);
	}

	/** screen coordinate to user coordinate */
	public double userMapX(int x) {
		return x / xScale + llx;
	}

	/** screen coordinate to user coordinate */
	public double userMapY(int y) {
		return ury - y / yScale;
	}

	protected int pixelDistanceX(Position p1, Position p2) {
		return Math.abs(screenMapX(p1) - screenMapX(p2));
	}

	protected int pixelDistanceY(Position p1, Position p2) {
		return Math.abs(screenMapY(p1) - screenMapY(p2));
	}

	protected static Vect2 simpleConvToVect2(double lat, double lon) {
		double deg_lat = Units.to("deg",lat);
		double deg_lon = Units.to("deg",lon);
		return new Vect2(Units.from("NM",60.0*deg_lon*Math.cos(lat)), Units.from("NM",60.0*deg_lat));
	}

	protected static Pair<Double,Double> simpleConvToLatLon(double x, double y) {
		double lat = Units.to("NM",y)/60.0;
		return new Pair<Double,Double>(Units.from("deg",lat), Units.from("deg",Units.to("NM",x)/(60.0*Math.cos(lat))));
	}

	/**
	 * Return true if the given point is within buffer pixels of the visible drawing area
	 */
	public boolean onScreen(int x, int y, int buffer) {
		return x >= -buffer && y >= -buffer && x <= getWidth()+buffer && y <= getHeight()+buffer; 
	}

	/**
	 * Return true if the given point in user space is within buffer pixels of the visible drawing area
	 */
	public boolean onUserScreen(double x, double y, int buffer) {
		return onScreen(screenMapX(x), screenMapY(y), buffer);
	}

	/**
	 * Return true if the line drawn from (x1,y1) to (x2,y2) is (probably) within buffer pixels of the visible drawing area
	 */
	public boolean onScreen(int x1, int y1, int x2, int y2, int buffer) {
		// both points out of bounds on the same side
		int w = getWidth();
		int h = getHeight();
		if ((x1 < -buffer && x2 < -buffer) || (x1 > w+buffer && x2 > w+buffer) || 
				(y1 < -buffer && y2 < -buffer) || (y1 > h+buffer && y2 > h+buffer)) {
			return false;
		}
		//TODO check diagonals outside box
		return true;
	}
	
	/**
	 * Return true if the line drawn from (x1,y1) to (x2,y2) in user space is (probably) within buffer pixels of the visible drawing area
	 */
	public boolean onUserScreen(double x1, double y1, double x2, double y2, int buffer) {
		return onScreen(screenMapX(x1), screenMapY(y1), screenMapX(x2), screenMapY(y2), buffer);
	}
	

	public java.awt.Point getCurrentMouseLocation() {
		java.awt.Point p1 = MouseInfo.getPointerInfo().getLocation(); 
		java.awt.Point p2 = this.getLocationOnScreen().getLocation();
		if (p1 == null || p2 == null) {
			return new java.awt.Point(0,0);
		} else {
			return new java.awt.Point(p1.x - p2.x, p1.y - p2.y);
		}
	}

	private void setDefaults(Graphics g) {
		g.setFont(DEFAULT_FONT);
		firstTry = false;
	}

	protected void setLineStyleDashed(Graphics g) {
		float dash1[] = {10.0f,5.0f};
		BasicStroke dashed = new BasicStroke(1.0f, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f, dash1, 0.0f);
		((Graphics2D)g).setStroke(dashed);
	}

	protected void setLineStyleBoldDashed(Graphics g) {
		float dash1[] = {10.0f,5.0f};
		BasicStroke dashed = new BasicStroke(2.0f, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f, dash1, 0.0f);
		((Graphics2D)g).setStroke(dashed);
	}

	protected void setLineStyleDotted(Graphics g) {
		float dash1[] = {2.0f};
		BasicStroke dashed = new BasicStroke(1.0f, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f, dash1, 0.0f);
		((Graphics2D)g).setStroke(dashed);
	}

	protected void setLineStyleBold(Graphics g) {
		float thickness = 2.0f;                                                     // modifiable to 3.0f
		BasicStroke bolded = new BasicStroke(thickness, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f);
		((Graphics2D)g).setStroke(bolded);		
	}

	protected void setLineStyleExtraBold(Graphics g) {
		float thickness = 4.0f;                                                     // modifiable to 3.0f
		BasicStroke bolded = new BasicStroke(thickness, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f);
		((Graphics2D)g).setStroke(bolded);		
	}

	/**
	 * Return the line style to a simple solid line
	 * @param g graphic context
	 */
	protected void clearLineStyle(Graphics g) {
		((Graphics2D)g).setStroke(new BasicStroke());
	}




	//
	// ------------ Drawing Primitives ----------------------
	//

	/** Draw a line from p1 to p2 */
	protected void drawLine(Graphics g, double p1_x, double p1_y, double p2_x, double p2_y) {
		g.drawLine(screenMapX(p1_x), screenMapY(p1_y), screenMapX(p2_x),
				screenMapY(p2_y));
	}

	/** Draw a line from p1 to p2 
	 * 
	 * @param g graphics context
	 * @param p1 the first point
	 * @param p2 the second point
	 */
	protected void drawLine(Graphics g, Vect2 p1, Vect2 p2) {
		drawLine(g, p1.x, p1.y, p2.x, p2.y);
	}

	/** Draw a line from p1 to p2, with the given width */
	protected void drawLine(Graphics g, Vect2 s1, Vect2 s2, float width) {
		Graphics2D g2d = (Graphics2D)g;
		Stroke stroke = g2d.getStroke();
		BasicStroke basicStroke = new BasicStroke(width, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_ROUND);  
		g2d.setStroke(basicStroke);
		g2d.drawLine(screenMapX(s1.x),screenMapY(s1.y),screenMapX(s2.x),screenMapY(s2.y));
		g2d.setStroke(stroke);
	}

	/** Draw a line from p1 to p2 */
	protected void drawLineBold(Graphics g, Vect2 p1, Vect2 p2) {
		drawLineBold(g, p1.x, p1.y, p2.x, p2.y);
	}

	/** Draw a line from p1 to p2 */
	protected void drawLineBold(Graphics g, double p1_x, double p1_y, double p2_x, double p2_y) {
		setLineStyleBold(g);
		g.drawLine(screenMapX(p1_x), screenMapY(p1_y), screenMapX(p2_x),
				screenMapY(p2_y));
		clearLineStyle(g);
	}

	/** Draw a line from p1 to p2 */
	protected void drawLineDashed(Graphics g, Vect2 p1, Vect2 p2) {
		drawLineDashed(g, p1.x, p1.y, p2.x, p2.y);
	}

	/** Draw a line from p1 to p2 */
	protected void drawLineDashed(Graphics g, double p1_x, double p1_y, double p2_x, double p2_y) {
		setLineStyleDashed(g);
		g.drawLine(screenMapX(p1_x), screenMapY(p1_y), screenMapX(p2_x),
				screenMapY(p2_y));
		clearLineStyle(g);
	}


	/**
	 * Draw a line of the given length from the given point at the given angle
	 * (angle in radians)
	 */
	protected void drawTick(Graphics g, Vect2 pos, double angle, int length) {
		int vlx = screenMapX(pos.x);
		int vly = screenMapY(pos.y);
		int vux = vlx + (int) (length * Math.cos(angle));
		int vuy = vly - (int) (length * Math.sin(angle));
		g.drawLine(vlx, vly, vux, vuy);
	}

	protected void drawTick(Graphics g, Vect2 s, double angle, double vLen){
		double ang = Units.from("deg",angle)-Math.PI/2;
		Vect2 v = new Vect2(Math.cos(ang),Math.sin(ang));
		int vlx = (int) (screenMapX(s.x)+0.9*v.x*vLen);       
		int vly = (int) (screenMapY(s.y)+0.9*v.y*vLen);       
		int vux = (int) (screenMapX(s.x)+1.1*v.x*vLen);       
		int vuy = (int) (screenMapY(s.y)+1.1*v.y*vLen);       
		g.drawLine(vlx,vly,vux,vuy);
	}


	//	protected void drawSquare(Graphics g, Vect2 s, double halfLength, boolean filled){
	//		double x = screenMapX(s.x);
	//		double y = screenMapY(s.y);
	//		int ww = (int) (2.0*halfLength);
	//		g.drawRect((int) (x - halfLength), (int) (y - halfLength), ww, ww);
	//	}

	protected void drawPolygon(Graphics g, List<Vect2> vs, boolean filled) {
		int [] xs = new int[vs.size()];
		int [] ys = new int[vs.size()];
		for (int i = 0; i < vs.size(); i++) {
			xs[i] = screenMapX(vs.get(i).x);
			ys[i] = screenMapY(vs.get(i).y);
		}
		if (filled) {
			g.fillPolygon(xs, ys, vs.size());
		} else {
			g.drawPolygon(xs, ys, vs.size());
		}
	}

	protected void drawSquare(Graphics g, Vect2 s, int side, boolean filled){
		double sside_x = side / (xScale * 2.0);
		double sside_y = side / (yScale * 2.0);
		drawRect(g,s.x-sside_x,s.y-sside_y,s.x+sside_x,s.y+sside_y,filled);
	}

	protected void drawSquare(Graphics g, Position p, int size, boolean filled){
		drawSquare(g, position2Vect(p), size, filled);		

	}

	protected void drawDashedRect(Graphics g, double lowerleft_x, double lowerleft_y, 
			double upperright_x, double upperright_y, int dashy) {
		int lly = screenMapY(lowerleft_y);
		for (int ury = screenMapY(upperright_y); ury < lly ; ury += (int)(1.25*dashy)) {
			g.fillRect(screenMapX(lowerleft_x), ury,
					screenMapX(upperright_x) - screenMapX(lowerleft_x),Util.min(dashy,lly-ury));
		}
	}

	protected void drawRect(Graphics g, double lowerleft_x, double lowerleft_y, double upperright_x, double upperright_y,
			boolean filled) {
		if (filled) {
			g.fillRect(screenMapX(lowerleft_x), screenMapY(upperright_y),
					screenMapX(upperright_x) - screenMapX(lowerleft_x),
					screenMapY(lowerleft_y) - screenMapY(upperright_y));
		} else {
			g.drawRect(screenMapX(lowerleft_x), screenMapY(upperright_y),
					screenMapX(upperright_x) - screenMapX(lowerleft_x),
					screenMapY(lowerleft_y) - screenMapY(upperright_y));
		}
	}

	protected void drawRect(Graphics g, Vect2 lowerleft, Vect2 upperright, boolean filled) {
		drawRect(g, lowerleft.x, lowerleft.y, upperright.x, upperright.y, filled);
	}

	/**
	 * Equilateral triangle centered at point pos, with the given length of a
	 * side. If eps = 1, then the triangle points up, if eps = -1, then it
	 * points down.
	 */
	protected void drawTriangle(Graphics g, Vect2 pos, int length, int eps) {
		int lsqrt3div2 = (int) (0.8660254 * length); // sqrt(3)/2 * length
		int ldiv2 = length / 2;

		int x = screenMapX(pos.x);
		int y = screenMapY(pos.y) - eps * lsqrt3div2;

		g.drawLine(x, y, x + lsqrt3div2, y + eps * (lsqrt3div2 + ldiv2));
		g.drawLine(x + lsqrt3div2, y + eps * (lsqrt3div2 + ldiv2), x
				- lsqrt3div2, y + eps * (lsqrt3div2 + ldiv2));
		g.drawLine(x - lsqrt3div2, y + eps * (lsqrt3div2 + ldiv2), x, y);
	}// drawTriangle

	protected void drawTriangle(Graphics g, Position p) {
		drawTriangle(g, position2Vect(p), Default_Triangle_Size, 1);
	}

	protected void drawTriangle(Graphics g, Position p, int size, int eps){
		drawTriangle(g, position2Vect(p), size, eps);
	}

	/**
	 * Draw a circle with the center in user coordinates and the radius in pixels. For a circle to look
	 * like a circle, the x and y scale factors must be the same, this is easy
	 * in "scale" mode, in "area" mode, this depends on the window size relative
	 * to the plotting area.
	 * 
	 * @param g the graphics context
	 * @param pos the position, in user coordinates, of the circle
	 * @param radius the radius in pixels of the circle
	 * @param filled true if the circle is filled
	 */
	protected void drawCircle(Graphics g, Vect2 pos, int radius, boolean filled) {
		if (filled) {
			g.fillOval(screenMapX(pos.x) - radius, screenMapY(pos.y) - radius, 2 * radius, 2 * radius);
		} else {
			g.drawOval(screenMapX(pos.x) - radius, screenMapY(pos.y) - radius, 2 * radius, 2 * radius);
		}
	}

	/**
	 * Draw a circle with the radius in user coordinates. For a circle to look
	 * like a circle, the x and y scale factors must be the same, this is easy
	 * in "scale" mode, in "area" mode, this depends on the window size relative
	 * to the plotting area.
	 * 
	 * @param g the graphics context
	 * @param pos the position, in user coordinates, of the circle
	 * @param radius the radius, in user coordinates, of the circle
	 * @param filled true if the circle is filled
	 */
	protected void drawCircle(Graphics g, Vect2 pos, double radius,	boolean filled) {
		if (filled) {
			g.fillOval(screenMapX(pos.x - radius), screenMapY(pos.y + radius), (int) (2 * radius * xScale), (int) (2 * radius * yScale));
		} else {
			g.drawOval(screenMapX(pos.x - radius), screenMapY(pos.y + radius), (int) (2 * radius * xScale), (int) (2 * radius * yScale));
		}
	}

	/**
	 * Draw a circle with the radius in user coordinates. For a circle to look
	 * like a circle, the x and y scale factors must be the same, this is easy
	 * in "scale" mode, in "area" mode, this depends on the window size relative
	 * to the plotting area.
	 * 
	 * @param g the graphics context
	 * @param pos the position, in user coordinates, of the circle
	 * @param radius the radius, in user coordinates, of the circle
	 */
	protected void drawCircle(Graphics g, Vect2 pos, double radius) {
		drawCircle(g,pos,radius,false);
	}

	protected void drawPt(Graphics g, Position p) {
		drawCircle(g, position2Vect(p), Default_Point_Radius, true);
	}

	protected void drawPt(Graphics g, Position p, int point_radius) {
		drawCircle(g, position2Vect(p), point_radius, true);
	}

	protected void drawCircle(Graphics g, Position p, int size, boolean filled){				
		drawCircle(g, position2Vect(p), size, filled);		
	}

	/** Draw an arc.
	 * the radius refers to the outer radius, the width refers to the width of
	 * the arc which is inside the outer radius. angles are in radians, zero
	 * radians is "east"
	 */
	protected void drawArc(Graphics g, Vect2 pos, int radius,
			double start_angle, double end_angle, int width) {
		Graphics2D g2d = (Graphics2D) g;
		Stroke stroke = g2d.getStroke();
		g2d.setStroke(new BasicStroke((float) width, BasicStroke.CAP_BUTT,
				BasicStroke.JOIN_ROUND));

		g2d.draw(new Arc2D.Double(screenMapX(pos.x) - radius, screenMapY(pos.y)
				- radius, 2 * radius, 2 * radius, Units.to(Units.deg, start_angle),
				Units.to(Units.deg, end_angle - start_angle), Arc2D.OPEN));

		g2d.setStroke(stroke);
	}

	/** Draw an arc.
	 * the radius refers to the outer radius, the width refers to the width of
	 * the arc which is inside the outer radius. Angles are in radians, zero
	 * radians is "east"
	 */
	protected void drawArc(Graphics g, Vect2 pos, double radius,
			double start_angle, double end_angle, int width) {
		Graphics2D g2d = (Graphics2D) g;
		Stroke stroke = g2d.getStroke();
		g2d.setStroke(new BasicStroke((float) width, BasicStroke.CAP_BUTT,
				BasicStroke.JOIN_ROUND));

		g2d.draw(new Arc2D.Double(screenMapX(pos.x - radius), screenMapY(pos.y
				+ radius), (int) (2 * radius * xScale),
				(int) (2 * radius * yScale), Units.to(Units.deg, start_angle), Units
				.to(Units.deg, end_angle - start_angle), Arc2D.OPEN));

		g2d.setStroke(stroke);
	}

	/** Draw an arc, where the position refers to a point on the arc (not the center of the circle).
	 * The radius refers to the outer radius, the width refers to the width of
	 * the arc which is inside the outer radius. <p>
	 * 
	 * The radius of the circle is correct only in "scale" mode (where x and y scale factors are the same)
	 * 
	 * @param g
	 * @param pos
	 * @param radius 
	 * @param track the track angle direction (in radians) to point concave part of the arc, (track angle means zero degrees is up)
	 * @param angle the amount (in radians) of arc on each side of the "pos" to draw 
	 * @param width
	 */
	protected void drawArcSide(Graphics g, Vect2 pos, int radius,
			double track, double angle, int width) {
		double direction = Util.track2math(track);
		Vect2 dir = Vect2.mkLengthAngle(radius/xScale,direction); // TODO: need to convert the radius in a better way (this assumes x and y scale are the same)
		Vect2 npos = pos.Add(dir);
		double nangle = Util.to_pi(direction) + Math.PI;  // reverse direction
		drawArc(g,npos,radius,nangle-angle,nangle+angle,width);
		//drawCircle(g,npos,radius,false);
		//drawArrow(g,npos,pos);
		//f.pln("drawArc "+Units.str("deg",direction));

	}

	/** Draw an arc with a dashed line
	 * 
	 * @param g
	 * @param pos
	 * @param radius
	 * @param start_angle
	 * @param end_angle
	 * @param width
	 */
	protected void drawArcDashed(Graphics g, Vect2 pos, double radius,
			double start_angle, double end_angle, int width) {
		Graphics2D g2d = (Graphics2D) g;
		Stroke stroke = g2d.getStroke();
		float dash1[] = {5.0f,5.0f};
		BasicStroke dashed = new BasicStroke((float) width, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_ROUND, 10.0f, dash1, 0.0f);
		g2d.setStroke(dashed);

		g2d.draw(new Arc2D.Double(screenMapX(pos.x - radius), screenMapY(pos.y
				+ radius), (int) (2 * radius * xScale),
				(int) (2 * radius * yScale), Units.to(Units.deg, start_angle), Units
				.to(Units.deg, end_angle - start_angle), Arc2D.OPEN));

		g2d.setStroke(stroke);
	}

	/** Draw a Chevron
	 * 
	 * @param g graphics context
	 * @param s Center of chevron
	 * @param v direction of chevron
	 * @param color color
	 * @param filled 0=no, 1=chevron only, 2=chevron and symbol 
	 * @param symbol 0=none, 1=circle, 2=square. 
	 * @param scale scale should probably be between 10 and 20
	 */
	protected void drawChevron(Graphics g, Vect2 s, Vect2 v, Color color, int filled, int symbol, double scale) {
		Color saved_color = g.getColor();  
		g.setColor(color);

		int points = 4;
		int [][] aircraft = new int[2][points];
		int x0 = screenMapX(s.x);
		int y0 = screenMapY(s.y);
		int x1 = screenMapX(v.x);
		int y1 = screenMapY(v.y);
		//    double scale = 15;
		double theta = Math.atan2(x1-x0,y0-y1);

		if (symbol > 0) {
			Stroke savedstroke = ((Graphics2D)g).getStroke();
			BasicStroke bolded = new BasicStroke(2.0f, BasicStroke.CAP_BUTT, 
					BasicStroke.JOIN_MITER, 10.0f);
			((Graphics2D)g).setStroke(bolded);
			if (symbol == 1) {
				int radius = (int)(1.5*scale);
				if (filled == 2) {
					g.fillOval(x0 - radius, y0 - radius, 2*radius,2*radius);
				} else {
					g.drawOval(x0 - radius, y0 - radius, 2*radius,2*radius);
				}
			} else if (symbol == 2) {
				for (int i=0;i<points;++i) {
					double x = square[0][i]*Math.cos(theta)-square[1][i]*Math.sin(theta);
					double y = square[0][i]*Math.sin(theta)+square[1][i]*Math.cos(theta);
					aircraft[0][i] = x0+(int)(x*scale);
					aircraft[1][i] = y0+(int)(y*scale);
				}
				if (filled == 2) {
					g.fillPolygon(aircraft[0],aircraft[1],points);
				} else {
					g.drawPolygon(aircraft[0],aircraft[1],points);
				}
			}
			((Graphics2D)g).setStroke(savedstroke);
		}

		// Rotate/scale/translate chevron
		for (int i=0;i<points;++i) {
			double x = chevron[0][i]*Math.cos(theta)-chevron[1][i]*Math.sin(theta);
			double y = chevron[0][i]*Math.sin(theta)+chevron[1][i]*Math.cos(theta);
			aircraft[0][i] = x0+(int)(x*scale);
			aircraft[1][i] = y0+(int)(y*scale);
		}
		if (filled == 1) {
			g.fillPolygon(aircraft[0],aircraft[1],points);
		} else {
			if (filled == 2) {
				g.setColor(saved_color);
			}
			g.drawPolygon(aircraft[0],aircraft[1],points);
		}
		g.setColor(saved_color);
	}

	/** Draw a Chevron, with a size of 15
	 * 
	 * @param g graphics context
	 * @param s Center of chevron
	 * @param v direction of chevron
	 * @param color color
	 * @param filled 0=no, 1=chevron only, 2=chevron and symbol 
	 * @param symbol 0=none, 1=circle, 2=square. 
	 */
	protected void drawChevron(Graphics g, Vect2 s, Vect2 v, Color color, int filled, int symbol) {
		drawChevron(g,s,v,color,filled,symbol,15.0);
	}

	/** Draw a Chevron
	 * @param g graphics context
	 * @param pos Center of chevron
	 * @param v direction of chevron
	 * @param color color
	 * @param scale  scale should probably be between 10 and 20
	 */
	protected void drawChevron(Graphics g, Position pos, Velocity v, Color color, double scale) {
		int filled = 1; // 0=no, 1=chevron only, 2=chevron and symbol 
		int symbol = 0; //0=none, 1=circle, 2=square. 
		drawChevron(g, position2Vect(pos), vel2Vect(v), color, filled, symbol, scale);		
	}

	protected void drawArrow(Graphics g, double fromx, double fromy, double tox, double toy) {
		drawArrow(g, new Vect2(fromx,fromy), new Vect2(tox,toy));
	}

	protected void drawArrow(Graphics g, Vect2 from, Vect2 to) {
		drawArrow(g,from,to,Default_Arrowhead_size);
	}

	protected void drawArrow(Graphics g, Vect2 from, Vect2 to, int sz_arrow) {
		double theta;
		int x, y;
		theta = Math.atan2(screenMapY(from.y) - screenMapY(to.y),
				screenMapX(to.x) - screenMapX(from.x));

		drawLine(g, from, to);
		x = (int) Math.round(sz_arrow * Math.cos(theta + Math.PI / 10.));
		y = (int) Math.round(sz_arrow * Math.sin(theta + Math.PI / 10.));
		g.drawLine(screenMapX(to.x), screenMapY(to.y), screenMapX(to.x) - x,
				screenMapY(to.y) + y);
		x = (int) Math.round(sz_arrow * Math.cos(theta - Math.PI / 10.));
		y = (int) Math.round(sz_arrow * Math.sin(theta - Math.PI / 10.));
		g.drawLine(screenMapX(to.x), screenMapY(to.y), screenMapX(to.x) - x,
				screenMapY(to.y) + y);
	}

	protected void drawArrowBold(Graphics g, Vect2 from, Vect2 to) {
		float thickness = 2.0f;                                                     // modifiable to 3.0f
		BasicStroke bolded = new BasicStroke(thickness, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f);
		((Graphics2D)g).setStroke(bolded);

		drawArrow(g,from,to);

		((Graphics2D)g).setStroke(new BasicStroke());
	}

	protected void drawArrowDashed(Graphics g, Vect2 from, Vect2 to) {
		float dash1[] = {10.0f,5.0f};
		BasicStroke dashed = new BasicStroke(1.0f, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f, dash1, 0.0f);
		((Graphics2D)g).setStroke(dashed);

		drawArrow(g,from,to);

		((Graphics2D)g).setStroke(new BasicStroke());
	}

	protected void drawArrowBoldDashed(Graphics g, Vect2 from, Vect2 to) {
		float dash1[] = {10.0f,5.0f};
		BasicStroke dashed = new BasicStroke(2.0f, BasicStroke.CAP_BUTT, 
				BasicStroke.JOIN_MITER, 10.0f, dash1, 0.0f);
		((Graphics2D)g).setStroke(dashed);

		drawArrow(g,from,to);

		((Graphics2D)g).setStroke(new BasicStroke());
	}


	private int stringOffsetsUpDown(FontMetrics m, int bottom_top) {
		int ud;
		if (bottom_top < 0) {
			ud = 0;
		} else if (bottom_top == 0) {
			ud = m.getAscent() / 2;
		} else {
			ud = m.getAscent();
		}
		return ud;
	}

	private int stringOffsetsLeftRight(FontMetrics m, String str, int left_right) {
		int lr;
		if (left_right < 0) {
			lr = 0;
		} else if (left_right == 0) {
			lr = m.stringWidth(str) / 2;
		} else {
			lr = m.stringWidth(str);
		}
		return lr;
	}

	/**
	 * Draw a string at the location given by the screen coordinates. The down_up and 
	 * left_right parameters set which part of the string is
	 * placed at a location on the screen. <p>
	 * 
	 * Tip: you may not want the string right at the position, try putting a
	 * space in the string<p>
	 * 
	 * This now supports newline (\n) characters in the expected way, 
	 * splitting strings into separate horizontal lines.  The entire block of
	 * text will follow the vertical centering rule.<p>
	 * 
	 * @param g the current graphics context
	 * @param str the string to be displayed
	 * @param posx the horizontal location of the string, in screen coordinates
	 * @param posy the vertical location of the string, in screen coordinates
	 * @param left_right where the text should be centered horizontally: left = -1, center = 0, right = +1
	 * @param bottom_top where the text should be centered vertically: bottom = -1, center = 0, top = +1
	 */
	protected void drawString(Graphics g, String str, int posx, int posy, int left_right, int bottom_top) {
		FontMetrics m = g.getFontMetrics();
		String[] lines = str.split("\n");
		if (lines.length == 1) {
			g.drawString(str, posx - stringOffsetsLeftRight(m, str, left_right), posy + stringOffsetsUpDown(m, bottom_top));
		} else {
			for (int i = 0; i < lines.length; i++) {
				int k;     
				if (bottom_top < 0) { // bottom
					k = (i+1-lines.length)*m.getHeight();
				} else if (bottom_top == 0) { // center
					k = (i+1)*m.getHeight() - (int)((double)lines.length/2.0*m.getHeight());
				} else { // top
					k = i*m.getHeight()+m.getAscent(); 
				}
				g.drawString(lines[i], posx - stringOffsetsLeftRight(m, lines[i], left_right), posy + k);
			}
		}
	}

	/**
	 * Draw a string at the location given by the user coordinates.
	 * The down_up and left_right parameters set which part of the string is
	 * placed at a location on the screen. <p>
	 * 
	 * Tip: you may not want the string right at the position, try putting a
	 * space in the string.<p>
	 * 
	 * This now supports newline (\n) characters in the expected way, 
	 * splitting strings into separate horizontal lines.  The entire block of
	 * text will follow the vertical centering rule.<p>
	 * 
	 * @param g the current graphics context
	 * @param str the string to be displayed
	 * @param posx the horizontal location of the string, in user coordinates
	 * @param posy the vertical location of the string, in user coordinates
	 * @param left_right where the text should be centered horizontally: left = -1, center = 0, right = +1
	 * @param bottom_top where the text should be centered vertically: bottom = -1, center = 0, top = +1
	 */
	protected void drawString(Graphics g, String str, double posx, double posy,	int left_right, int bottom_top) {
		drawString(g,str,screenMapX(posx),screenMapY(posy),left_right,bottom_top);
	}

	/**
	 * Draw a string at the location given by the user coordinates.
	 * The down_up and left_right parameters set which part of the string is
	 * placed at position pos. <p>
	 * 
	 * Tip: you may not want the string right at the position, try putting a
	 * space in the string
	 * @param g the current graphics context
	 * @param pos the location of the string, in user coordinates
	 * @param str the string to be displayed
	 * @param left_right where the text should be centered horizontally: left = -1, center = 0, right = +1
	 * @param bottom_top where the text should be centered vertically: bottom = -1, center = 0, top = +1
	 */
	protected void drawString(Graphics g, String str, Vect2 pos, int left_right, int bottom_top) {
		drawString(g,str,pos.x,pos.y,left_right,bottom_top);
	}

	protected void drawString(Graphics g, String str, Vect2 s, double angle, int left_right, int bottom_top){
		Graphics2D g2 = (Graphics2D) g;
		FontMetrics m = g.getFontMetrics();
		g2.translate(screenMapX(s.x()) , screenMapY(s.y()) );

		g2.rotate(-angle);
		g2.drawString(str,-stringOffsetsLeftRight(m, str, left_right),stringOffsetsUpDown(m, bottom_top)); //screenMapX(s.x()),screenMapY(s.y()));
		g2.rotate(angle);
		g2.translate(-screenMapX(s.x()),-screenMapY(s.y()));
	}

	protected void drawString(Graphics g, String s, Position p, int hpos, int vpos){
		drawString(g, s, position2Vect(p), hpos, vpos);		
	}


	/**
	 * Draw a buffered image, scaled to fit in the indicated screen region.  Null images 
	 * are filtered out.
	 * @param g graphics
	 * @param x0 upper left screen position
	 * @param y0 upper left screen position
	 * @param x1 lower right screen position
	 * @param y1 lower right screen position
	 * @param img image to draw
	 */
	protected void drawImage(Graphics g, int x0, int y0, int x1, int y1, BufferedImage img) {
		try {
			if (img != null) {
				g.drawImage(img, x0,y0,x1,y1, 0,0,img.getWidth(this),img.getHeight(this), null);
			}
		} catch (OutOfMemoryError e) {
			f.pln("WARNING: ViewPort: Insufficient memory allocated to display background image");    	  
		}
	}

	/**
	 * 
	 * @param ulx upper left of screen image
	 * @param uly upper left of screen image
	 * @param x x in screen image
	 * @param y y in screen image
	 * @param scalx x scaling factor
	 * @param scaly y scaling factor
	 * @param tsz size of tile (original image scale)
	 * @return tile index tx,ty for tile containing screen point x,y
	 */
	private Pair<Integer,Integer> whichTile(int ulx, int uly, int x, int y, double scalx, double scaly, int tsz) {
		int xx = (int)Math.floor((x-ulx)/(scalx*tsz));
		int yy = (int)Math.floor((y-uly)/(scaly*tsz));
		return Pair.make(xx, yy);
	}

	/**
	 * 
	 * @param ulx upper left of screen image
	 * @param uly upper left of screen image
	 * @param tx tile index
	 * @param ty tile index
	 * @param scalx scaling factor
	 * @param scaly scaling factor
	 * @param tsz (original) size of tile
	 * @return location on screen for upper left corner of tile tx,ty
	 */
	private Pair<Integer,Integer> tileScreenLocation(int ulx, int uly, int tx, int ty, double scalx, double scaly, int tsz) {
		int xx = (int)Math.floor((tx*tsz)*scalx+ulx);
		int yy = (int)Math.floor((ty*tsz)*scaly+uly);
		return Pair.make(xx, yy);
	}



	/**
	 * Draw an image based on various prerendered variations of the image.
	 * @param g
	 * @param x0 upper left location on screen
	 * @param y0 upper left location on screen
	 * @param x1 lower right position on screen
	 * @param y1 lower right position on screen
	 * @param img base image
	 * @param tiles array of tiles that comprise img, with 0,0 being the "upper left" corner (may be null)
	 * @param dList list of downsampled versions of img of increasing resolution (may be null)
	 */
	protected void drawImage(Graphics g, int x0, int y0, int x1, int y1, BufferedImage img, BufferedImage[][] tiles, List<BufferedImage> dList) {
		int dx = x1-x0;
		int dy = y1-y0;

		int imgx = img.getWidth();
		int imgy = img.getHeight();

		double scalx = dx*1.0/imgx;
		double scaly = dy*1.0/imgy;

		int scrx = this.getWidth();
		int scry = this.getHeight();

		if (dList != null && dx < dList.get(dList.size()-1).getWidth() && dy < dList.get(dList.size()-1).getHeight()) {
			// we have zoomed out enough to use a downsampled image
			// start with the lowest resolution one and look for a match
			for (int i = 0; i < dList.size(); i++) {
				BufferedImage sub = dList.get(i);
				if (dx <= sub.getWidth() && dy <= sub.getHeight()) {
					drawImage(g,x0,y0,x1,y1,sub);
					break;
				}
			}
		} else if (tiles != null) {
			// we have zoomed in enough for tiles to be relevant
			int xsz = tiles.length;
			int ysz = tiles[0].length;
			int tilesize = tiles[0][0].getWidth();
			Pair<Integer, Integer> ul = whichTile(x0, y0, 0, 0, scalx, scaly, tilesize);
			Pair<Integer, Integer> lr = whichTile(x0, y0, scrx, scry, scalx, scaly, tilesize);
			for (int x = Util.max(0, ul.first); x <= Util.min(lr.first, xsz-1); x++) {
				for (int y = Util.max(0, ul.second); y <= Util.min(lr.second, ysz-1); y++) {
					Pair<Integer,Integer> corner1 = tileScreenLocation(x0, y0, x, y, scalx, scaly, tilesize);
					Pair<Integer,Integer> corner2 = tileScreenLocation(x0, y0, x+1, y+1, scalx, scaly, tilesize);
					int xa = corner1.first;
					int xb = corner2.first;
					int ya = corner1.second-1;
					int yb = corner2.second-1;
					if (x == xsz-1) { // far edge tiles may be smaller than normal
						double fr = tiles[x][y].getWidth()/tilesize;
						xb = xa+(int)Math.floor((xb-xa)*fr);
					}
					if (y == ysz-1) { // far edge tiles may be smaller than normal
						double fr = tiles[x][y].getHeight()/tilesize;
						yb = ya+(int)Math.floor((yb-ya)*fr);						
					}
					drawImage(g,xa,ya,xb,yb,tiles[x][y]);
				}
			}
		} else {
			// default to original image
			drawImage(g,x0,y0,x1,y1,img);
		}
	}

	protected void drawImage(Graphics g, double ulx, double uly, double urx, double ury, BufferedImage img, BufferedImage[][] tiles, List<BufferedImage> dList) {
		drawImage(g, screenMapX(ulx), screenMapY(uly), screenMapX(urx), screenMapY(ury), img, tiles, dList);
	}


	/**
	 * Draw a buffered image, scaled to fit in the indicated user region.  Null images 
	 * are filtered out.
	 * 
	 * @param g graphics
	 * @param ulx user upper left (NW) corner of image -- x direction (lower)
	 * @param uly user upper left (NW) corner of image -- y direction (lower)
	 * @param urx user lower right (SE) corner of image -- x direction
	 * @param ury user lower right (SE) corner of image -- y direction
	 * @param img image.
	 */
	protected void drawImage(Graphics g, double ulx, double uly, double urx, double ury, BufferedImage img) {
		drawImage(g, screenMapX(ulx), screenMapY(uly), screenMapX(urx), screenMapY(ury), img);
	}

	/**
	 * Draw a buffered image, scaled to fit in the indicated user region. Null images 
	 * are filtered out.
	 * @param g graphics
	 * @param ul user upper left (NW) corner of image
	 * @param lr user lower right (SE) corner of image
	 * @param img image
	 */
	protected void drawImage(Graphics g, Vect2 ul, Vect2 lr, BufferedImage img) {
		drawImage(g, screenMapX(ul.x()), screenMapY(ul.y()), screenMapX(lr.x()), screenMapY(lr.y()), img);
	}

	@Override
	public String toString() {
		return "ViewPort [stretch=" + stretch + ", xScale=" + xScale + ", yScale="
				+ yScale + ", center_x=" + center_x + ", center_y=" + center_y
				+ ", llx=" + llx + ", ury=" + ury + "]";
	}


}// class ViewPort

