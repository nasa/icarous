/* Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DebugSupport.h"
#include "PlanIO.h"
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

  std::string DebugSupport::getNameNoPath(std::string filename) {
//	  if (filename == null) {
//		  return null;
//	  }
	  // replace all \ (dos) with / (unix), then find last
	  //return filename.substring(filename.replaceAll("\\\\", "/").lastIndexOf("/") + 1);
	  return "DebugSupport::getNameNoPath: NOT YET IMPLENTED";
  }


  void DebugSupport::dumpPlan(const Plan& plan, std::string str) {
	  std::string dumpFileName = "1dump_"+str+".txt";
	  std::ofstream pw;
	  pw.open(dumpFileName.c_str(),std::ofstream::out);
	  if (!pw) {
		  cout << "Error opening file " << dumpFileName << endl;
	  }
	  pw << plan.getOutputHeader(true) << std::endl;
	  pw << plan.toOutput(0, 12, true, false);
	  pw.close();
	  fpln(" ............. dumpPlan: created file "+dumpFileName);
  }



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
