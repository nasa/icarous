package gov.nasa.larcfm.IO;


import java.util.List;

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.f;

public class PlanIO {



	/** Write the plan to the given file */
	public static void savePlan(Plan plan, String fileName) {
		if (plan.getName().equals("")) {
			plan.setName("noname");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writePlan(plan,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	/** Write the plans to the given file */
	public static void savePlans(Plan plan1, Plan plan2, String fileName) {
		if (plan1.getName().equals("")) {
			plan1.setName("noname1");
		}
		if (plan2.getName().equals("")) {
			plan2.setName("noname2");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writePlan(plan1,true);
		pw.writePlan(plan2,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	/** Write the plans to the given file */
	public static void savePlans(List<Plan> plans, String fileName) {
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		int count = 0;
		for (Plan plan: plans) {
			if (plan.getName().equals("")) {
				plan.setName("noname"+count);
				count++;
			}
			pw.writePlan(plan,true);
		}
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	/** Write the plan to the given file */
	public static void savePlanWithHdr(Plan plan, String fileName, String hdr) {
		if (plan.getName().equals("")) {
			plan.setName("noname");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writeLn(hdr);
		pw.writePlan(plan,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

}
