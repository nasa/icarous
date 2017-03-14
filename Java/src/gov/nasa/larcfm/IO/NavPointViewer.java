/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;


import static gov.nasa.larcfm.Util.f.Fm2;
import static gov.nasa.larcfm.Util.f.Fm3;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.TcpData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;

import javax.swing.*;

public class NavPointViewer extends JFrame {

	private static final long serialVersionUID = 1L;

	public static boolean modifySourceValues = false;

	static int Xsz = 1000;
	static int Ysz = 250;

	int stepSize = 1;

	private String unitsHDist = "NM";
	private String unitsVDist = "ft";
	//private String unitsHSpd = "kts";
	//private String unitsVSpd = "fpm";
	private String unitsAngles = "deg";
	private String unitsTime = "s";

	private JTextField t_x;
	private JTextField t_y;
	private JTextField t_z;
	private JTextField t_t;

	private JTextField t_gs_in;
	private JTextField t_gs_out;
	private JTextField t_vs_in;
	private JTextField t_vs_out;
	String[] typeStrings = {"Orig","Modified","Added","Virtual","AltPreserve"};
	JComboBox<String> typebox = new JComboBox<String>(typeStrings); 
	String[] tcpStrings = {"NONE","BOT","MOT","EOT","BGSC","EGSC","BVSC","EVSC","TMID"};
	JComboBox<String> tcpbox = new JComboBox<String>(tcpStrings);
	String[] mutabilityStrings = {"MMM","MMF","MFM","MFF","FMM","FMF", "FFM", "FFF"};
	JComboBox<String> mutabilitybox = new JComboBox<String>(mutabilityStrings);

	String[] minorvStrings = {"NONE","TURN","GS","VS"};
	JComboBox<String> minorvbox = new JComboBox<String>(minorvStrings); 

	static boolean inUpdate = false;

	JTextField t_ptlbl;
	JTextField t_chg_acn;
	JTextField t_trafd;
	JTextField t_trafh;
	JLabel xlabel = new JLabel("X (nmi) ", JLabel.RIGHT);
	JLabel ylabel = new JLabel("Y (nmi) ", JLabel.RIGHT);
	JLabel zlabel = new JLabel("Alt (ft) ", JLabel.RIGHT);
	JLabel tlabel = new JLabel("Time (s) ",JLabel.RIGHT);

	JLabel srcxlabel = new JLabel("X (nmi) ", JLabel.RIGHT);
	JLabel srcylabel = new JLabel("Y (nmi) ", JLabel.RIGHT);
	JLabel srczlabel = new JLabel("Alt (ft) ", JLabel.RIGHT);
	JLabel srctlabel = new JLabel("Time (s) ",JLabel.RIGHT);

	JLabel vintrklabel = new JLabel("Vin Trk ", JLabel.RIGHT);
	JLabel vingslabel = new JLabel("Vin Gs ", JLabel.RIGHT);
	JLabel vinvslabel = new JLabel("Vin Vs ", JLabel.RIGHT);
	JLabel accellabel = new JLabel("Accel ",JLabel.RIGHT);

	String velString = "";
	JLabel vEditNoteJL;
	JLabel gs_out;
	JLabel vs_out;
	JLabel trk_out;
	JLabel trk_in;
	
	JTextField t_stepSize;
	
	final JScrollPane scroll;

	final JTextField editMsg = new JTextField(30);
	private ArrayList<Plan> viewplans;
	private int pickAc;
	private int pickWp;

	public NavPointViewer(ArrayList<Plan> vp) {
		viewplans = vp;
		pickAc = 0;
		pickWp = 0;
		setTitle("Edit NavPoint (" + pickAc + "," + pickWp + ")");
		setSize(Xsz, Ysz);
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		
		JPanel contentPane = new JPanel();
		scroll = new JScrollPane(contentPane);
		add(scroll);
		// JPanel rP = new Edit();
		// rP.setBackground(Color.white);
		// contentPane.add(rP);

		// setLayout(new FlowLayout() );
		contentPane.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();

		// t = new JLabel("------------------",JLabel.RIGHT);
		// JLabel t = new JLabel("                  ",JLabel.RIGHT);
		// contentPane.add(t);

		// t = new JLabel("                       ",JLabel.RIGHT);
		// contentPane.add(t);

		// contentPane.add(new JLabel("THIS IS A TEST STRING ",JLabel.RIGHT));

		// ROW 0

	    ROW = 0;
	    COL = 0;
	    
	    // ROW 0

		
		// X FIELD

		t_x = new JTextField(8);
		t_x.requestFocus();
		t_x.setEditable(false);
		t_x.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(xlabel, c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
	    c.insets = new Insets(10,10,0,0);   // RWB ??
		contentPane.add(t_x, c);



		// Y FIELD	

		t_y = new JTextField(8);
		t_y.requestFocus();
		t_y.setEditable(false);
		t_y.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(ylabel, c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_y, c);



		// Z FIELD

		t_z = new JTextField(8);
		t_z.requestFocus();
		t_z.setEditable(false);
//		t_z.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent event) {
//				int ac = viewplans.getPickAc();
//				MenuActions.savePlan(ac);
//
//				commitZ();
//
//				update(); // because "gs into" field must be recalculated
//				vStrat.updateDisplays();
//				//				vStrat.frame.repaint();
//				//				vStrat.Zframe.repaint();
//			}// actionPerformed
//		});
		t_z.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(zlabel, c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_z, c);




		// T FIELD		

		t_t = new JTextField(8);
		t_t.requestFocus();
		t_t.setEditable(true);
		t_t.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				double newT = Double.parseDouble(t_t.getText());
				int ac = pickAc;
				Plan acPlan = viewplans.get(ac);
				double lastTime = -1;
				int foundK = -1;
				for (int k = 0; k < acPlan.size(); k++) {
					double time_k = acPlan.time(k);
					if (lastTime <= newT && newT <= time_k) {
						foundK = k;
						f.pln(" %%%%%%%%%%%%%%%%% foundK = "+foundK);
						break;
					}
                    lastTime = time_k;
				}
				if (foundK >= 0) {
					f.pln(" %%%%%%%%%%%%%%%%% set pickWp = "+foundK);
				    pickWp = foundK;
				}
				update(); // because "gs into" field must be recalculated
				//panel.updateDisplays();
				//				vStrat.frame.repaint();
				//				vStrat.Zframe.repaint();
			}// actionPerformed
		});
		t_t.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(tlabel, c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_t, c);

		
		
		// ROW 1
	   newRow();



		// GS IN FIELD

		t_gs_in = new JTextField(8);
		t_gs_in.requestFocus();
		t_gs_in.setEditable(false);
//		t_gs_in.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent event) {
//				int ac = viewplans.getPickAc();
//				MenuActions.savePlan(ac);
//
//				commitGSIN();
//
//				update(); // because "gs into" field must be recalculated
//				vStrat.updateDisplays();
//				//				vStrat.frame.repaint();
//				//				vStrat.Zframe.repaint();
//			}// actionPerformed
//		});
		t_gs_in.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(new JLabel("gs into ", JLabel.RIGHT), c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_gs_in, c);



		// GS OUT FIELD

//		double gs_out_val = calcGSout();
//		String gs_out_str = "unk";
//		if (gs_out_val >= 0)
//			gs_out_str = Fm2(Units.to(_kn, gs_out_val));
//		gs_out = new JLabel("gs out = " + gs_out_str, JLabel.RIGHT);
//	    c.gridx = nextCol();
//	    c.gridy = ROW;
//		c.gridwidth = 2;
//		contentPane.add(gs_out, c);
//		c.gridwidth = 1;
//		blankSpace();

		t_gs_out = new JTextField(8);
		t_gs_out.requestFocus();
		t_gs_out.setEditable(false);
//		t_gs_out.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent event) {
//				int ac = viewplans.getPickAc();
//				MenuActions.savePlan(ac);
//
//				commitGSIN();
//
//				update(); // because "gs into" field must be recalculated
//				vStrat.updateDisplays();
//				//				vStrat.frame.repaint();
//				//				vStrat.Zframe.repaint();
//			}// actionPerformed
//		});
		t_gs_out.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(new JLabel("gs out ", JLabel.RIGHT), c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_gs_out, c);



		// VS IN FIELD

		t_vs_in = new JTextField(8);
		t_vs_in.requestFocus();
		t_vs_in.setEditable(false);
//		t_vs_in.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent event) {
//				int ac = viewplans.getPickAc();
//				MenuActions.savePlan(ac);
//
//				commitVSIN();
//
//				update();
//				vStrat.updateDisplays();
//				//				vStrat.frame.repaint();
//				//				vStrat.Zframe.repaint();
//			}// actionPerformed
//		});
		t_vs_in.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(new JLabel("vs into ", JLabel.RIGHT), c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_vs_in, c);

		// VS OUT FIELD

//		double vs_out_val = calcVSout();
//		String vs_out_str = "unk";
//		vs_out_str = Fm2(Units.to(_fpm, vs_out_val));
//		vs_out = new JLabel("vs out = " + vs_out_str, JLabel.RIGHT);
//	    c.gridx = nextCol();
//	    c.gridy = ROW;
//		c.gridwidth = 2;
//		contentPane.add(vs_out, c);
//		c.gridwidth = 1;
//		blankSpace();

		t_vs_out = new JTextField(8);
		t_vs_out.requestFocus();
		t_vs_out.setEditable(false);
//		t_vs_out.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent event) {
//				int ac = viewplans.getPickAc();
//				MenuActions.savePlan(ac);
//
//				commitVSIN();
//
//				update();
//				vStrat.updateDisplays();
//				//				vStrat.frame.repaint();
//				//				vStrat.Zframe.repaint();
//			}// actionPerformed
//		});
		t_vs_out.setText("unk");
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(new JLabel("vs out ", JLabel.RIGHT), c);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(t_vs_out, c);

		



		// ROW 2
		newRow();


		// TRK IN FIELD

		String trk_in_str = "unk";
		trk_in = new JLabel("trk in = " + trk_in_str, JLabel.RIGHT);
	    c.gridx = nextCol();
	    c.gridy = ROW;
		c.gridwidth = 2;
		contentPane.add(trk_in, c);
		blankSpace();


		// TRK OUT FIELD

	    c.gridx = nextCol();
	    c.gridy = ROW;
		String trk_out_str = "unk";
		trk_out = new JLabel("trk out = " + trk_out_str, JLabel.RIGHT);
		contentPane.add(trk_out, c);
		c.gridwidth = 1;
		blankSpace();






		
		// ROW 6
		newRow();

		// PREVIOUS BUTTON
		JButton prevB = new JButton("Previous");
		prevB.requestFocus();
		prevB.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				//viewplans.logGuiAction("Edit prevB");				
				// f.pln(" $$$$$$$$$$$$$$$$$$$$ REPAINT $$$$$$$$$$$$$$$");
				if (pickWp > 0) {
					pickWp = pickWp - stepSize;
					// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
					//viewplans.updateDisplays();
					//					viewplans.frame.repaint();
					//					viewplans.Zframe.repaint();
				}
				update();
			}// actionPerformed
		});
		// contentPane.add(new JLabel("",JLabel.RIGHT));
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(prevB, c);


		t_stepSize = new JTextField(8);
		t_stepSize.requestFocus();
		t_stepSize.setEditable(true);
		t_stepSize.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				//int ac = pickAc;
				stepSize = Integer.parseInt(t_stepSize.getText());
				update(); // because "gs into" field must be recalculated
			}// actionPerformed
		});		
		
		//f.pln(" $$$$$$$$$$$$$$ NavPointViewer Constructor: viewplans.currentStep = "+viewplans.currentStep);
		stepSize = 1; //viewplans.getDisplayInterval();
		t_stepSize.setText(""+stepSize);                        

	    c.gridx = nextCol();
	    c.gridy = ROW;
	    c.insets = new Insets(10,10,0,0);   // RWB ??
		contentPane.add(t_stepSize, c);


		// NEXT BUTTON

		JButton nextB = new JButton("Next");
		nextB.requestFocus();
		nextB.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				//viewplans.logGuiAction("Edit nextB");				
				// f.pln(" $$$$$$$$$$$$$$$$$$$$ REPAINT $$$$$$$$$$$$$$$");
				if (pickWp < viewplans.get(pickAc).size() - 1) {
					pickWp =(pickWp + stepSize);
					// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
					//viewplans.updateDisplays();
					//					viewplans.frame.repaint();
					//					viewplans.Zframe.repaint();
				}
				update();
			}// actionPerformed
		});
		// contentPane.add(new JLabel("",JLabel.RIGHT));
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(nextB, c);

		
		// REFRESH BUTTON

		JButton refreshB = new JButton("ReFresh");
		refreshB.requestFocus();
		refreshB.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				//viewplans.logGuiAction("Edit refreshB");				
				// f.pln(" $$$$$$$$$$$$$$$$$$$$ REPAINT $$$$$$$$$$$$$$$");
				update();
				//MenuActions.stepToHere.() ;
				//viewplans.updateDisplays();
			}// actionPerformed
		});
		// contentPane.add(new JLabel("",JLabel.RIGHT));
	    c.gridx = nextCol();
	    c.gridy = ROW;
		contentPane.add(refreshB, c);


		//pack();
		scroll.revalidate();

	} // EditFrame method

//
//
//
//	// update text fields of Edit Window from fPlan[pickAc]
	public void update() {
		inUpdate = true;
		int ac = pickAc;
		int pick = pickWp;

		boolean planOK = ac < viewplans.size() && ac >= 0;
		boolean pointOK = planOK && pick >= 0 && pick < viewplans.get(ac).size();
		t_x.setEnabled(pointOK);
		t_y.setEnabled(pointOK);
		t_z.setEnabled(pointOK);
		t_t.setEnabled(pointOK);
		t_gs_in.setEnabled(pointOK);
		t_gs_out.setEnabled(pointOK);
		t_vs_in.setEnabled(pointOK);
		t_vs_out.setEnabled(pointOK);
		typebox.setEnabled(pointOK);
		mutabilitybox.setEnabled(pointOK);
		tcpbox.setEnabled(pointOK);
		minorvbox.setEnabled(pointOK);

		// f.pln(" ++++++++++++++ ac, vStrat.plans.size() = "+ac+" < "+vStrat.plans.size());
		if (planOK) {
			Plan iFP = viewplans.get(ac);
			setTitle("NavPointViewer " + iFP.getName() + " (" + ac + ") PT=" + pick);
			if (pointOK) {
				String msg = "Selected = (" + ac + "," + pick + ")";
				if (pick > 0) {
					int prevPt = pick - 1; // iFP.lastReal(pick-1);
					if (prevPt < 0)
						prevPt = 0;
					msg = msg + "     Prev = (" + ac + "," + prevPt + ") @ "
					+ Fm2(iFP.point(prevPt).time());
				}
				if (pick + 1 < iFP.size()) {
					int nextReal = iFP.nextPtOrEnd(pick);
					msg = msg + "   Next = (" + ac + "," + nextReal + ") @ "
					+ Fm2(iFP.point(nextReal).time());
				}
				if (ac == 0) {
					msg = msg + "  (Ownship)";
				}
				
				NavPoint np = iFP.point(pick); 
				TcpData tcp = iFP.getTcpData(pick);
				
				if (tcp.isTCP()) {
					msg = "src=("+tcp.getSourcePosition().toStringNP(2)+","+Fm2(tcp.getSourceTime())+")";
					if (tcp.isBOT()) {
						msg += " R="+Fm2(Units.to(unitsHDist, iFP.turnRadius(pick)))+unitsHDist;						
					} else if (tcp.isBGS()) {
							msg += Fm2(Units.to(unitsHDist, iFP.gsAccel(pick)))+unitsHDist;						
					} else if (tcp.isBVS()) {
						msg += Fm2(Units.to(unitsHDist, iFP.vsAccel(pick)))+unitsHDist;						
					}
					//msg += " Vin=("+iFP.velocityInit(pick).toStringNP(2)+")";
				}

				if (isLatLon()) {
					t_x.setText(Fm3(Units.to(unitsAngles, np.x()))); // +" nm");
					t_y.setText(Fm3(Units.to(unitsAngles, np.y()))); // +" nm");
					xlabel.setText("EW ("+unitsAngles+")");
					ylabel.setText("NS ("+unitsAngles+")");
					//				} else if (viewplans.getSmallScale()) {
					//					t_x.setText(Fm3(Units.to(_m, np.x()))); // +" nm");
					//					t_y.setText(Fm3(Units.to(_m, np.y()))); // +" nm");
					//					xlabel.setText("X (m) ");
					//					ylabel.setText("Y (m) ");
				} else {
					t_x.setText(Fm3(Units.to(unitsHDist, np.x()))); // +" nm");
					t_y.setText(Fm3(Units.to(unitsHDist, np.y()))); // +" nm");
					xlabel.setText("X ("+unitsHDist+") ");
					ylabel.setText("Y ("+unitsHDist+") ");
					//				}
					//				if (viewplans.getSmallScale()) {
					//					t_z.setText(Fm3(Units.to(_m, np.z()))); // +"  ft");
					//					zlabel.setText("Alt (m) ");
					//} else {
				}
				t_z.setText(Fm3(Units.to(unitsVDist, np.z()))); // +"  ft");
				zlabel.setText("Alt ("+unitsVDist+") ");
				//}
				t_t.setText(Fm2(Units.to(unitsTime, np.time()))); // +" sec");
				tlabel.setText("Time ("+unitsTime+") ");
				double gs_in_val = calcGSin();
				String gs_in_str = "unk";
				if (gs_in_val >= 0) {
					gs_in_str = Fm2(Units.to("kn", gs_in_val));
//					if (viewplans.getSmallScale())
//						gs_in_str = Fm2(Units.to(_mps, gs_in_val));
				}
				t_gs_in.setText(gs_in_str); // +" sec");

				double gs_out_val = calcGSout();
				String gs_out_str = "unk";
				if (gs_out_val >= 0) {
					gs_out_str = Units.str("kn", gs_out_val);
//					if (viewplans.getSmallScale())
//						gs_out_str = Fm2(Units.to(_mps, gs_out_val))+" m/s";
				}
				//gs_out.setText("[gs out = " + gs_out_str + "]");
				t_gs_out.setText(gs_out_str); // +" sec");

				
				double vs_in_val = calcVSin();
				String vs_in_str = "unk";
				if (vs_in_val > -100000000) {
					vs_in_str = Fm2(Units.to("fpm", vs_in_val));
//					if (viewplans.getSmallScale())
//						vs_in_str = Fm2(Units.to(_mps, vs_in_val));
				}
				t_vs_in.setText(vs_in_str); // +" sec");


				double vs_out_val = calcVSout();
				String vs_out_str = "unk";
				if (vs_out_val > -100000000) {
					vs_out_str = Units.str("fpm", vs_out_val);
//					if (viewplans.getSmallScale())
//						vs_out_str = Fm2(Units.to(_mps, vs_out_val)) + " m/s";
				}
				//vs_out.setText("[vs out = " + vs_out_str + "]");
				t_vs_out.setText(vs_out_str); // +" sec");


				double trk_in_val = calcTrkIn();
				String trk_in_str = "unk";
				if (trk_in_val >= 0)
					trk_in_str = Fm2(Units.to(unitsAngles, trk_in_val))+ " "+unitsAngles;
				trk_in.setText("[trk in = " + trk_in_str +"]"); // \u00B0

				double trk_out_val = calcTrkOut();
				String trk_out_str = "unk";
				if (trk_out_val >= 0)
					trk_out_str = Fm2(Units.to(unitsAngles, trk_out_val))+ " "+unitsAngles;
				trk_out.setText("[trk out = " + trk_out_str +"]"); // \u00B0
				}
			}
		
		t_stepSize.setText(""+stepSize);

		
		inUpdate = false;
//		vStrat.updateDisplays();
//		//		vStrat.frame.repaint();
		repaint();
//		//		// VZP.reScale();
//		//		vStrat.Zframe.repaint();
		scroll.revalidate();
	}//update
	
	private boolean isLatLon() {
		if (viewplans.size() == 0) return true;
		return viewplans.get(0).isLatLon();
	}

	double calcTrkIn() {
		double rtn = -9000;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick > 0 && pick < iFP.size()) {
				if (iFP.size() > 1) {
					Velocity v = iFP.finalVelocity(pick - 1);
					rtn = v.compassAngle();
				}// if
			}// if
		}// if
		return rtn;
	}// calcTDist

	double calcTrkOut() {
		double rtn = -9000;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick >= 0 && pick < iFP.size() - 1) {
				if (iFP.size() > 1) { // compute gs
					Velocity v = iFP.initialVelocity(pick);
					rtn = v.compassAngle();
				}// if
			}// if
		}// if
		return rtn;
	}// calcTDist

	private int ROW = 0;
	private int COL = 0;
	private int LASTCOL = 0;

	private void newRow() {
		ROW++;
		COL = 0;
	}


	private void blankSpace() {
		COL++;
	}
//	private void blankSpace(int n) {
//		COL = COL + n;
//	}

	private int nextCol() {
		if (COL > LASTCOL) LASTCOL = COL;
		return COL++;
	}

	double calcGSin() {
		double rtn = -1;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick > 0)
				rtn = iFP.averageVelocity(pick - 1).gs(); // iFP.velocityTrkGsVs(pick-1).y;
		}// if
		return rtn;
	}// calcGSin

	double calcGSout() {
		double rtn = -1;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick < iFP.size() - 1)
				rtn = iFP.averageVelocity(pick).gs(); // iFP.velocityTrkGsVs(pick).y;
		}// if
		// f.pln("$$$ Edit.calcGSout pick,rtn = "+pick+", "+Units.to(_kn,rtn));
		return rtn;
	}// calcGSout

	double calcVSin() {
		double rtn = -100000001;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick < iFP.size()) {
				if (iFP.size() > 1 && pick >= 1) { // compute gs
					NavPoint WP = iFP.point(pick);
					int prevPt = pick - 1; // iFP.lastReal(pick-1);
					if (prevPt < 0)
						prevPt = 0;
					NavPoint prevWP = iFP.point(prevPt);
					// f.pln(" >>>>> prevWP = "+ViewPanel.sStr(prevWP));
					double vdist = WP.z() - prevWP.z();
					rtn = vdist / (WP.time() - prevWP.time());
				}// if
			}// if
		}// if
		return rtn;
	}// calcVSin

	double calcVSout() {
		double rtn = -100000001;
		if (pickAc < viewplans.size()) {
			Plan iFP = viewplans.get(pickAc);
			int pick = pickWp;
			if (pick + 1 < iFP.size()) {
				if (iFP.size() > 1 && pick >= 0) { // compute gs
					NavPoint WP = iFP.point(pick);
					NavPoint nextWP = iFP.point(iFP.nextPtOrEnd(pick));
					// f.pln(" >>>>> nextWP = "+ViewPanel.sStr(nextWP));
					double vdist = nextWP.z() - WP.z();
					rtn = vdist / (nextWP.time() - WP.time());
				}// if
			}// if
		}// if
		return rtn;
	}// calcVSout
//
//
//
//
//
//	void commitX() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (vStrat.plans.isLatLon()) {
//			double val = Util.to_pi(Units.parse(vStrat.unitsAngles, "rad", t_x.getText()));
//			nwp = nwp.mkLon(val);
//			vStrat.logGuiAction("Edit lon "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkLon(val));
//		} else if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_x.getText()));
//			nwp = nwp.mkX(val);
//			vStrat.logGuiAction("Edit x "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkX(val));
//		} else {
//			double val = Units.parse(vStrat.unitsHDist, "m", t_x.getText());
//			nwp = nwp.mkX(val);
//			vStrat.logGuiAction("Edit x "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkX(val));
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//	void commitsrcX() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (vStrat.plans.isLatLon()) {
//			double val = Util.to_pi(Units.parse(vStrat.unitsAngles, "rad", t_srcx.getText()));
////			nwp = nwp.mkLon(val);
//			vStrat.logGuiAction("Edit lon src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkLon(val));
//		} else if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_srcx.getText()));
////			nwp = nwp.mkX(val);
//			vStrat.logGuiAction("Edit x src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkX(val));
//		} else {
//			double val = Units.parse(vStrat.unitsHDist, "m", t_srcx.getText());
////			nwp = nwp.mkX(val);
//			vStrat.logGuiAction("Edit x src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkX(val));
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//	
//	double to_halfPi(double r) {
//		if (r > 0) return Util.min(r, Math.PI/2);
//		else return Util.max(r, -Math.PI/2);
//	}
//
//
//	void commitY() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (vStrat.plans.isLatLon()) {
//			double val = to_halfPi(Units.parse(vStrat.unitsAngles, "rad", t_y.getText()));
//			nwp = nwp.mkLat(val);
//			vStrat.logGuiAction("Edit lat "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkLat(val));
//		} else if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_y.getText()));
//			nwp = nwp.mkY(val);
//			vStrat.logGuiAction("Edit y "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkY(val));
//		} else {
//			nwp = nwp.mkY(Units.parse(vStrat.unitsHDist, "m", t_y.getText()));
//			vStrat.logGuiAction("Edit y "+nwp.y());		
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//
//	void commitsrcY() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (vStrat.plans.isLatLon()) {
//			double val = to_halfPi(Units.parse(vStrat.unitsAngles, "rad", t_srcy.getText()));
//			vStrat.logGuiAction("Edit lat src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkLat(val));
//		} else if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_srcy.getText()));
//			vStrat.logGuiAction("Edit y src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkY(val));
//		} else {
//			double val = Units.parse(vStrat.unitsHDist, "m", t_srcy.getText());
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkY(val));
//			vStrat.logGuiAction("Edit y src "+nwp.y());		
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//
//	
//	void commitZ() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_z.getText()));
//			nwp = nwp.mkZ(val);
//			vStrat.logGuiAction("Edit alt "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkZ(val));
//		} else {
//			double val = Units.parse(vStrat.unitsVDist, "m", t_z.getText());
//			nwp = nwp.mkZ(val);
//			vStrat.logGuiAction("Edit z "+val);		
////			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkZ(val));
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//
//	void commitsrcZ() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		if (viewplans.getSmallScale()) {
//			double val = Units.from(_m,Double.parseDouble(t_srcz.getText()));
//			vStrat.logGuiAction("Edit alt src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkZ(val));
//		} else {
//			double val = Units.parse(vStrat.unitsVDist, "m", t_srcz.getText());
//			vStrat.logGuiAction("Edit z src "+val);		
//			nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkZ(val));
//		}
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//	
//	void commitT() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val = Units.parse(vStrat.unitsTime, "s", t_t.getText());
//		vStrat.logGuiAction("Edit t "+val);		
//		nwp = nwp.makeTime(val);
//		if (modifySourceValues) nwp = nwp.makeSourceTime(val);
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//
//	void commitsrcT() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val = Units.parse(vStrat.unitsTime, "s", t_srct.getText());
//		vStrat.logGuiAction("Edit t src "+val);		
////		nwp = nwp.makeTime(val);
//		nwp = nwp.makeSourceTime(val);
//		ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//
//	}
//
//
//	void commitvintrk() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val = Util.to_pi(Units.parse(vStrat.unitsAngles, "rad", t_vintrk.getText()));
//		vStrat.logGuiAction("Edit vin trk "+val);		
//		nwp = nwp.makeVelocityIn(nwp.velocityIn().mkTrk(val));
//
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//
//	void commitvings() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val = Units.parse(vStrat.unitsHSpd, "m/s", t_vings.getText());
//		vStrat.logGuiAction("Edit vin gs "+val);		
//		nwp = nwp.makeVelocityIn(nwp.velocityIn().mkGs(val));
//
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//
//	void commitvinvs() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val = Units.parse(vStrat.unitsVSpd, "m/s", t_vinvs.getText());
//		vStrat.logGuiAction("Edit vin vs "+val);		
//		nwp = nwp.makeVelocityIn(nwp.velocityIn().mkVs(val));
//
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//	void commitaccel() {
//		int ac = viewplans.getPickAc();
//		int wp = viewplans.getPickWp();
//		Plan iFP = vStrat.plans.getPlan(ac);
//		NavPoint nwp = iFP.point(wp);
//		double wpt = nwp.time();
//		double val;
//		if (nwp.isTurn()) {
//			val = Units.parse("deg/s", "rad/s", t_vinvs.getText());
//			vStrat.logGuiAction("Edit omega "+val);		
//		} else {
//			val = Units.parse("m/s^2", "m/s^2", t_vinvs.getText());
//			vStrat.logGuiAction("Edit accel "+val);		
//		}
//		nwp = nwp.makeAccel(val);
//
//		try {
//			int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//			if (ac == 0)
//				vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//			viewplans.setPickWp(pos);
//			// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//			if (vStrat.plans.hasMessage()) {
//				f.pln(vStrat.plans.getMessage());
//			}
//		} catch (MutabilityException e) {
//			f.pln(e.toString());
//		}
//	}
//
//	void commitGSIN() {
//		double nGS;
//		if (viewplans.getSmallScale()) {
//			nGS = Units.from(_mps, Double.parseDouble(t_gs_in.getText()));
//		} else {
//			nGS = Units.parse(vStrat.unitsHSpd, "m/s", t_gs_in.getText());
//		}
//		vStrat.logGuiAction("Edit gsin "+nGS);				
//		double ldist = calcDist();
//		// f.pln(">>> ldist = "+Units.to(_NM,ldist));
//		if (ldist > 0 && nGS > 0) {
//			double dt = ldist / nGS;
//			int ac = viewplans.getPickAc();
//			int wp = viewplans.getPickWp();
//			Plan iFP = vStrat.plans.getPlan(ac);
//			int prevPt = wp - 1; // iFP.lastReal(wp-1);
//			if (prevPt < 0)
//				prevPt = 0;
//			double prevT = iFP.point(prevPt).time();
//			double nt = dt + prevT;
//			double diff = dt - (iFP.point(wp).time()-prevT);
//			NavPoint nwp = iFP.point(wp).makeTime(nt);
//			if (modifySourceValues) nwp = nwp.makeSourceTime(nt);
//			ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//			try {
//				int pos = wp;
//				if (viewplans.getPreserveGS()) { // shift time for all subsequent points
//					if (diff < 0) {
//						pos = vStrat.plans.moveWp(ac, wp, nwp);
//						for (int i = wp+1; i < iFP.size(); i++) {
//							vStrat.plans.moveWp(ac, i, iFP.point(i).makeTime(iFP.getTime(i)+diff));
//						}
//					} else if (diff > 0) {
//						for (int i = iFP.size()-1; i > wp; i--) {
//							vStrat.plans.moveWp(ac, i, iFP.point(i).makeTime(iFP.getTime(i)+diff));
//						}
//						pos = vStrat.plans.moveWp(ac, wp, nwp);
//					}
//				} else {
//					pos = vStrat.plans.moveWp(ac, wp, nwp);
//				}
//				if (ac == 0)
//					vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//				viewplans.setPickWp(pos);
//				// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//				if (vStrat.plans.hasMessage()) {
//					f.pln(vStrat.plans.getMessage());
//				}
//			} catch (MutabilityException e) {
//				f.pln(e.toString());
//			}
//
////			vStrat.updateDisplays();
//			//			vStrat.frame.repaint();
//			//			vStrat.Zframe.repaint();
//			// t_t.setText(Fm1(Units.to(_sec,nt)));
//		} else
//			ViewPanel.error(" $$ Edit.gsInto textfield internal error");
//
//	}
//
//	void commitVSIN() {
//		double nVS;
//		if (viewplans.getSmallScale()) {
//			nVS = Units.from(_mps,Double.parseDouble(t_vs_in.getText()));
//		} else {
//			nVS = Units.parse(vStrat.unitsVSpd, "m/s", t_vs_in.getText());
//		}
//		vStrat.logGuiAction("Edit vsin "+nVS);				
//		double tdist = calcTDist();
//		if (tdist > 0) {
//			double nz = tdist * nVS;
//			int ac = viewplans.getPickAc();
//			int wp = viewplans.getPickWp();
//			Plan iFP = vStrat.plans.getPlan(ac);
//			int prevPt = wp - 1; // iFP.lastReal(wp-1);
//			if (prevPt < 0)
//				prevPt = 0;
//			double prevZ = iFP.point(prevPt).z();
//			nz = nz + prevZ;
//			NavPoint nwp = iFP.point(wp).mkZ(nz);
//			if (modifySourceValues) nwp = nwp.makeSourcePosition(nwp.tcpSourcePosition().mkZ(nz));			
//			double wpt = iFP.point(wp).time();
//			ViewPanel.cStepTime = vStrat.plans.getPlan(ac).point(vStrat.cStep).time();
//			try {
//				int pos = vStrat.plans.moveWp(ac, wpt, nwp);
//				if (ac == 0)
//					vStrat.setCStep(vStrat.plans.getPlan(0).getNearestIndex(ViewPanel.cStepTime));
//				viewplans.setPickWp(pos);
//				// viewplans.setSelWp(ViewPanel.getWp(viewplans.getPickAc(),viewplans.getPickWp()));
//				t_z.setText(Fm1(Units.to(_ft, nz)));
//				if (vStrat.plans.hasMessage()) {
//					f.pln(vStrat.plans.getMessage());
//				}
//			} catch (MutabilityException e) {
//				f.pln(e.toString());
//			}
//
////			vStrat.updateDisplays();
//			//			vStrat.frame.repaint();
//			//			vStrat.Zframe.repaint();
//
//			// vStrat.reSyncPlan();
//		} else
//			ViewPanel.error(" $$ Edit.vsInto textfield internal error");
//
//	}
//
//
//	void commitType() {
//		int ac = viewplans.getPickAc();
//		Plan iFP = vStrat.plans.getPlanRef(ac); // changing the
//		// actual plan
//		String wty = (String)typebox.getSelectedItem();											// actual plan
//		NavPoint np;
//		if (wty.equals("Orig")) {
//			np = iFP.point(viewplans.getPickWp());
//			np = np.makeOriginal();
//			iFP.set(viewplans.getPickWp(), np);
//			// iFP.setType(viewplans.getPickWp(), NavPoint.WayType.Orig);
//		}
//		if (wty.equals("Modified")) {
//			np = iFP.point(viewplans.getPickWp());
//			np = np.makeModified();
//			iFP.set(viewplans.getPickWp(), np);
//			// iFP.setType(viewplans.getPickWp(),
//			// NavPoint.WayType.Modified);
//		}
//		if (wty.equals("Added")) {
//			np = iFP.point(viewplans.getPickWp());
//			np = np.makeAdded();
//			iFP.set(viewplans.getPickWp(), np);
//			// iFP.setType(viewplans.getPickWp(), NavPoint.WayType.Added);
//		}
//		if (wty.equals("Virtual")) {
//			np = iFP.point(viewplans.getPickWp());
//			np = np.makeVirtual();
//			iFP.set(viewplans.getPickWp(), np);
//			// iFP.setType(viewplans.getPickWp(),
//			// NavPoint.WayType.Virtual);
//		}
//		if (wty.equals("AltPreserve")) {
//			np = iFP.point(viewplans.getPickWp());
//			np = np.makeAltPreserve();
//			iFP.set(viewplans.getPickWp(), np);
//			// iFP.setType(viewplans.getPickWp(),
//			// NavPoint.WayType.Virtual);
//		}		
//		vStrat.logGuiAction("Edit type "+wty);				
//	}
//
//	


}// NavPointViewer Class





