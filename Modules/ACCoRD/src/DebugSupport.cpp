/* Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DebugSupport.h"
#include "PlanIO.h"
#include "PlanWriter.h"
#include "string_util.h"
#include <string>
#include <iostream>
#include "format.h"
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

//#ifdef __GLIBC__
//#include <execinfo.h>
//#endif


namespace larcfm {

  using std::string;
  using std::endl;
  using std::cout;

//  std::string DebugSupport::getNameNoPath(std::string filename) {
////	  if (filename == null) {
////		  return null;
////	  }
//	  // replace all \ (dos) with / (unix), then find last
//	  //return filename.substring(filename.replaceAll("\\\\", "/").lastIndexOf("/") + 1);
//	  return "DebugSupport::getNameNoPath: NOT YET IMPLENTED";
//  }


  void DebugSupport::dumpPlan(const Plan& plan, std::string str) {
	  std::string dumpFileName = "1dump_"+str+".txt";
	  std::ofstream pw;
	  pw.open(dumpFileName.c_str(),std::ofstream::out);
	  if (!pw) {
		  cout << "Error opening file " << dumpFileName << endl;
	  }
	  PlanWriter pwr;
	  pwr.open(&pw);

	  pwr.setPrecision(12);
	  pwr.writePlan(plan, true);
	  pwr.close();
	  fpln(" ............. dumpPlan: created file "+dumpFileName);
  }


  void DebugSupport::dumpPlanAndPoly(const Plan& pln, const SimplePoly& sp, std::string str) {
	  std::string fileName = "1dump_"+str+".txt";
	  //std::ofstream pw;
	  //  pw.open(fileName.c_str(),std::ofstream::out);

	  PlanWriter pw;
	  pw.open(fileName);
	  pw.setPolygons(true);
	  bool write_tcp = true;
	  pw.writePlan(pln, write_tcp);
	  PolyPath ppTemp = PolyPath("TEMP");
	  fpln(" $$$$$ dumpPlanAndPoly: sp.size() = "+Fm0(sp.size()));
	  ppTemp.addPolygon(sp,0.0);
	  //fpln(" $$$$$ dumpPlanAndPoly: ppTemp = "+ppTemp.toString());
	  pw.writePolyPath(ppTemp, write_tcp);
	  pw.close();
	  fpln(" ............. dumpPlan: created file "+fileName);
	  return;
  }

  void DebugSupport::dumpPlanAndPolyPaths(const Plan& pln, const std::vector<PolyPath>& paths, std::string str) {
	bool write_tcp = true;
	dumpPlanAndPolyPaths(pln,  paths, write_tcp,  str);
}

 void DebugSupport::dumpPlanAndPolyPaths(const Plan& pln, const std::vector<PolyPath>& paths, bool write_tcp, std::string str) {
	std::string fileName = "1dump_"+str+".txt";
	PlanWriter pw;
	pw.open(fileName);
	pw.setPolygons(true);
	if (paths.size() > 0) {
		 pw.setPolyPathMode(paths[0].getPathMode());
	}
	pw.writePlan(pln, write_tcp);
	for (int i = 0; i < (int) paths.size(); i++) {
	   pw.setPolyPathMode(paths[i].getPathMode());
	   pw.writePolyPath(paths[i], write_tcp);
	}
	pw.close();
    fpln(" ............. dumpPlan: created file "+fileName);
	//return osw.toString();
	return;
}



//public static void dumpPoly(SimplePoly p, String str) {
//	String  dumpFileName = "1dump_"+str+".txt";
//	String s = SimplePolyToOutput(p,str);
//	try {
//		java.io.PrintWriter pw =
//				new java.io.PrintWriter(new java.io.BufferedWriter(new java.io.FileWriter(dumpFileName)));
//		pw.println(s);
//		pw.flush();
//		pw.close();
//	} catch (Exception e) {
//		f.pln(" ERROR: "+e.toString());
//	}
//	f.pln(" ............. dumpSimplePoly: created file "+dumpFileName);
//}



//  void DebugSupport::dumpPlan(const Plan& plan, std::string str) {
//	  std::string  dumpFileName = "1dump_"+str+".txt";
//	  PlanIO::savePlan(plan, dumpFileName);
//	  fpln(" ............. dumpPlan: created file "+dumpFileName);
//  }

  void DebugSupport::dumpAsUnitTest(const Plan& plan) {
	  for (int j = 0; j < plan.size(); j++) {
		  if (plan.isLatLon()) {
			  fp("  Position p"+Fm0(j)+"(LatLonAlt::make(");
			  fpln(Fm6(Units::to("deg",plan.point(j).lat()))
					  +", "+Fm6(Units::to("deg",plan.point(j).lon()))
					  +", "+Fm6(Units::to("ft",plan.point(j).alt()))+"));");
		  } else {
			  fp("  Position p"+Fm0(j)+"  = Position::makeXYZ(");
			  fpln(Fm6(Units::to("nm",plan.point(j).x()))+", "
					  +Fm6(Units::to("nm",plan.point(j).y()))+", "
					  +Fm6(Units::to("ft",plan.point(j).z()))+");");
		  }
	  }
	  for (int j = 0; j < plan.size(); j++) {
		  fp("  NavPoint np"+Fm0(j)+"(p"+Fm0(j)+","+Fm6(plan.point(j).time())+");");
		  fpln("     lpc.add(np"+Fm0(j)+");");
	  }
  }


  void DebugSupport::dumpAsNavPointAsserts(const Plan& plan) {
	  for (int j = 0; j < plan.size(); j++) {
		  NavPoint npj = plan.point(j);
		  fpln("assertEquals("+Units::str("ft",plan.point(j).alt())+",plan.point("+Fm0(j)+").alt(), 0.001);");
		  if (plan.isBOT(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isTurnBegin());");
		  }
//		  if (npj.isTurnMid()) {
//			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isTurnMid());");
//		  }
		  if (plan.isEOT(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isTurnEnd());");
		  }
		  if (plan.isBGS(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isGSCBegin());");
		  }
		  if (plan.isEGS(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isGSCEnd());");
		  }
		  if (plan.isBVS(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isVSCBegin());");
		  }
		  if (plan.isEVS(j)) {
			  fpln("EXPECT_TRUE(plan.point("+Fm0(j)+").isVSCEnd());");
		  }

	  }
  }



  void DebugSupport::halt() {
	  //Thread.dumpStack();
      //halt();
	  std::exit(EXIT_FAILURE);
  }

}
