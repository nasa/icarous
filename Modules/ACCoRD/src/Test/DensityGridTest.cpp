/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "DebugSupport.h"
#include "DensityGrid.h"
#include "DensityGridAStarSearch.h"
#include "NavPoint.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class DensityGridTest : public ::testing::Test {

public:

protected:
    virtual void SetUp() {
     }
};
	
	TEST_F(DensityGridTest, testEucl0) {
		Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
		Position p2 = Position::makeXYZ(30.0, 20.0, 10000);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p("");
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ "+Units::str("kn", np1.initialVelocity(np2).gs()));
		double squareSize = Units::from("nmi", 10);
		DensityGrid dg = DensityGrid(p, 3, squareSize);
		//fpln(" $$ dg = "+dg);

		EXPECT_EQ(10,dg.sizeX());
		EXPECT_EQ(9,dg.sizeY());
		std::pair<int,int> pp = dg.gridPosition(p1);
		EXPECT_EQ(3,(int)pp.first);
		EXPECT_EQ(3,(int)pp.second);
		pp = dg.gridPosition(p2);
		EXPECT_EQ(6,(int)pp.first);
		EXPECT_EQ(5,(int)pp.second);
		Position p3 = Position::makeXYZ(4.0, 13.0, 10000);
		Position p4 = Position::makeXYZ(11.0, 9.0, 10000);
		Position p5 = Position::makeXYZ(22.0, 15.0, 10000);
		Position p6 = Position::makeXYZ(11.0, 17.0, 10000);
		Position p7 = Position::makeXYZ(29.0, 2.0, 10000);
		pp = dg.gridPosition(p3);
		EXPECT_EQ(3,(int)pp.first);
		EXPECT_EQ(4,(int)pp.second);
		pp = dg.gridPosition(p4);
		EXPECT_EQ(4,(int)pp.first);
		EXPECT_EQ(3,(int)pp.second);
		pp = dg.gridPosition(p5);
		EXPECT_EQ(5,(int)pp.first);
		EXPECT_EQ(4,(int)pp.second);
		pp = dg.gridPosition(p6);
		EXPECT_EQ(4,(int)pp.first);
		EXPECT_EQ(4,(int)pp.second);
		pp = dg.gridPosition(p7);
		EXPECT_EQ(5,(int)pp.first);
		EXPECT_EQ(3,(int)pp.second);

		//		EXPECT_NEAR(squareSize,dg.squareSize(),0.000001);
		EXPECT_NEAR(0.0,dg.startPoint().x(),0.00001);
		EXPECT_NEAR(0.0,dg.startPoint().y(),0.00001);

		EXPECT_NEAR(Units::from("nmi",30),dg.endPoint().x(),0.00001);
		EXPECT_NEAR(Units::from("nmi", 20),dg.endPoint().y(),0.00001);

		// this does not match the grid with the buffers
		for (int i = 0; i < dg.sizeX(); i++) {
			for (int j = 0; j < dg.sizeY(); j++) {
				//fpln(" $$$ dg.getPosition("+i+","+j+") = "+dg.getPosition(i,j));
				EXPECT_NEAR(-Units::from("NM",30.0)+i*squareSize,dg.getPosition(i,j).x(),0.000001);
				EXPECT_NEAR(-Units::from("NM",30.0)+j*squareSize,dg.getPosition(i,j).y(),0.000001);
				EXPECT_NEAR(-Units::from("NM",25.0) + i*squareSize,dg.center(i,j).x(),0.000001);
				EXPECT_NEAR(-Units::from("NM",25.0) + j*squareSize,dg.center(i,j).y(),0.000001);
			}
		}
	}



	TEST_F(DensityGridTest, testEucl2) {
		Position p1 = Position::mkXYZ(0.0, 0.0, 0);
		Position p2 = Position::mkXYZ(5.0, 3.0, 0);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p("'");
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ "+Units::str("kn", np1.initialVelocity(np2).gs()));
		double squareSize = 1.0;
		//fpln(" $$ testEucl2 : squareSize = "+squareSize);
		DensityGrid dg = DensityGrid(p, 0, squareSize);
		//fpln(" $%%$$ dg.bounds = "+dg.bounds);
		//fpln(" $$ testEucl2 : dg = "+dg);
		for (int i = 0; i < dg.sizeX(); i++) {
			for (int j = 0; j < dg.sizeY(); j++) {
				double weight = i + 2*j;
				dg.setWeight(i,j,weight);
				//fpln(" $$$  testEucl2:  dg.getPosition("+i+","+j+") = ("+dg.getPosition(i,j).x()+","+dg.getPosition(i,j).y()+")");
				EXPECT_NEAR(i*squareSize,dg.getPosition(i,j).x(),0.000001);
				EXPECT_NEAR(j*squareSize,dg.getPosition(i,j).y(),0.000001);
				//fpln(" $$$  testEucl2:  dg.center("+i+","+j+") = ("+dg.center(i,j).x()+","+dg.center(i,j).y()+")");
				EXPECT_NEAR(i+0.5,dg.center(i,j).x(),0.000001);
				EXPECT_NEAR(j+0.5,dg.center(i,j).y(),0.000001);
				EXPECT_NEAR(weight,dg.getWeight(i,j),0.000001);
			}
		}
	}


	TEST_F(DensityGridTest, testSimpleEucl) {
		Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
		Position p2 = Position::makeXYZ(0.0, 1000.0, 10000);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p("'");
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ testSimpleEucl: "+Units::str("kn", np1.initialVelocity(np2).gs()));
		double squareSize = Units::from("nmi", 10);
		DensityGrid dg = DensityGrid(p, 0, squareSize);

		EXPECT_EQ(1,dg.sizeX());
		EXPECT_EQ(101,dg.sizeY());

		Position p3 = Position::makeXYZ(0.0, 510.0, 10000);
		Position p4 = Position::makeXYZ(11.0, 610.0, 10000);


		std::pair<int,int> pp = dg.gridPosition(p1);
		EXPECT_EQ(0,(int)pp.first);
		EXPECT_EQ(0,(int)pp.second);
		pp = dg.gridPosition(p2);
		EXPECT_EQ(0,(int)pp.first);
		EXPECT_EQ(100,(int)pp.second);
		pp = dg.gridPosition(p3);
		EXPECT_EQ(0,(int)pp.first);
		EXPECT_EQ(51,(int)pp.second);
		pp = dg.gridPosition(p4);
		EXPECT_EQ(-1,(int)pp.first); // out of bounds
		EXPECT_EQ(-1,(int)pp.second); // out of bounds (in x dimension)
		//		EXPECT_NEAR(squareSize,dg.squareSize(),0.000001);
		EXPECT_NEAR(0.0,dg.startPoint().x(),0.00001);
		EXPECT_NEAR(0.0,dg.startPoint().y(),0.00001);
		EXPECT_NEAR(0.0,dg.endPoint().x(),0.00001);
		EXPECT_NEAR(Units::from("nmi", 1000.0),dg.endPoint().y(),0.00001);
	}




	TEST_F(DensityGridTest, testSimpleLatLon) {
		Position p1 = Position::makeLatLonAlt(0.0, 0.0, 10000);
		Position p2 = Position::makeLatLonAlt(30.0, 30.0, 10000);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p("");
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		double squareSize_d = Units::from("deg",10);
		double squareSize_m = DensityGrid::distEstLatLon(0,squareSize_d);
		//fpln(" $$$$ squareSize_d = "+squareSize_d);
		//fpln(" $$$$ squareSize_m = "+Units::str("NM",squareSize_m));
		DensityGrid dg = DensityGrid(p, 0, squareSize_m);
		//fpln(" $%%$$ dg.bounds = "+dg.bounds);
		//fpln(" $$ testSimpleLatLon : dg = "+dg);
		//fpln("");
		for (int i = 0; i < dg.sizeX(); i++) {
			for (int j = 0; j < dg.sizeY(); j++) {
				double weight = i + 2*j;
				dg.setWeight(i,j,weight);
				//fpln(" $$$  testSimpleLatLon:  dg.getPosition("+i+","+j+") = "+dg.getPosition(i,j));
				EXPECT_NEAR(i*squareSize_d, dg.getPosition(i,j).lon(), 0.01);
				EXPECT_NEAR(j*squareSize_d, dg.getPosition(i,j).lat(), 0.01);
				//fpln(" $$$  testSimpleLatLon:  dg.center("+i+","+j+") = ("+dg.center(i,j).x()+","+dg.center(i,j).y()+")");
				//EXPECT_NEAR(i+0.5,dg.center(i,j).x(),0.000001);
				//EXPECT_NEAR(j+0.5,dg.center(i,j).y(),0.000001);
				EXPECT_NEAR(Units::from("deg",5.0) + i*squareSize_d, dg.center(i,j).x(),0.01);
				EXPECT_NEAR(Units::from("deg",5.0) + j*squareSize_d, dg.center(i,j).y(),0.01);
				EXPECT_NEAR(weight,dg.getWeight(i,j),0.000001);

			}
		}
		Position p3 = Position::makeLatLonAlt(4.0, 13.0, 10000);
		Position p4 = Position::makeLatLonAlt(11.0, 9.0, 10000);
		Position p5 = Position::makeLatLonAlt(22.0, 15.0, 10000);
		Position p6 = Position::makeLatLonAlt(11.0, 17.0, 10000);
		Position p7 = Position::makeLatLonAlt(29.0, 2.0, 10000);
		std::pair<int,int> pp = dg.gridPosition(p3);
		EXPECT_EQ(1,(int)pp.first);
		EXPECT_EQ(0,(int)pp.second);
		pp = dg.gridPosition(p4);
		EXPECT_EQ(0,(int)pp.first);
		EXPECT_EQ(1,(int)pp.second);
		pp = dg.gridPosition(p5);
		EXPECT_EQ(1,(int)pp.first);
		EXPECT_EQ(2,(int)pp.second);
		pp = dg.gridPosition(p6);
		EXPECT_EQ(1,(int)pp.first);
		EXPECT_EQ(1,(int)pp.second);
		pp = dg.gridPosition(p7);
		EXPECT_EQ(0,(int)pp.first);
		EXPECT_EQ(2,(int)pp.second);
	}




	TEST_F(DensityGridTest, testUglyLatLon) {
		Position p1 = Position::makeLatLonAlt(10.0, 10.0, 10000);
		Position p2 = Position::makeLatLonAlt(10.5, 11.6, 10000);
		double base = Units::from("deg", 10.0);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p;
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		double squareSize_d = Units::from("deg",0.1);
		double squareSize_m = DensityGrid::distEstLatLon(0,squareSize_d);
		//		fpln(" $$$$ squareSize_d = "+squareSize_d);
		//		fpln(" $$$$ squareSize_m = "+Units::str("NM",squareSize_m));
		DensityGrid dg = DensityGrid(p, 2, squareSize_m);
		//		fpln(" $%%$$ dg.bounds = "+dg.bounds);
		//		fpln(" $$ testUglyLatLon : dg = "+dg);
		//		fpln("");
		for (int i = 0; i < dg.sizeX(); i++) {
			for (int j = 0; j < dg.sizeY(); j++) {
				double weight = i + 2*j;
				dg.setWeight(i,j,weight);
				//fpln(" $$$  testUglyLatLon:  dg.getPosition("+i+","+j+") = "+dg.getPosition(i,j));
				EXPECT_NEAR(-2*squareSize_d+i*squareSize_d+base, dg.getPosition(i,j).lon(), 0.01);
				EXPECT_NEAR(-2*squareSize_d+j*squareSize_d+base, dg.getPosition(i,j).lat(), 0.01);
				//				fpln(" $$$  testUglyLatLon:  dg.center("+i+","+j+") = ("+dg.center(i,j).x()+","+dg.center(i,j).y()+")");
				EXPECT_NEAR(-2*squareSize_d+Units::from("deg",0.05) + i*squareSize_d+base, dg.center(i,j).x(),0.01);
				EXPECT_NEAR(-2*squareSize_d+Units::from("deg",0.05) + j*squareSize_d+base, dg.center(i,j).y(),0.01);
				EXPECT_NEAR(weight,dg.getWeight(i,j),0.000001);

			}
		}
		Position p3 = Position::makeLatLonAlt(10.1,10.1, 10000);
		Position p4 = Position::makeLatLonAlt(10.34, 11.11 , 10000);
		Position p5 = Position::makeLatLonAlt(10.03, 11.599, 10000);
		Position p6 = Position::makeLatLonAlt(10.48, 10.9999, 10000);
		Position p7 = Position::makeLatLonAlt(10.4999, 11.5999, 10000);
		std::pair<int,int> pp = dg.gridPosition(p3);
		EXPECT_EQ(3,(int)pp.first);
		EXPECT_EQ(3,(int)pp.second);
		pp = dg.gridPosition(p4);
		EXPECT_EQ(13,(int)pp.first);
		EXPECT_EQ(5,(int)pp.second);
		pp = dg.gridPosition(p5);
		EXPECT_EQ(17,(int)pp.first);
		EXPECT_EQ(2,(int)pp.second);
		pp = dg.gridPosition(p6);
		EXPECT_EQ(11,(int)pp.first);
		EXPECT_EQ(6,(int)pp.second);
		pp = dg.gridPosition(p7);
		EXPECT_EQ(17,(int)pp.first);
		EXPECT_EQ(6,(int)pp.second);
	}



	
	TEST_F(DensityGridTest, testOptimalPath) {	
		Position p1 = Position::mkXYZ(0.0, 0.0, 0);
		Position p2 = Position::mkXYZ(5.0, 3.0, 0);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p;
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ "+Units::str("kn", np1.initialVelocity(np2).gs()));
		double squareSize = 1.0;
		DensityGrid dg = DensityGrid(p, 0, squareSize);
		for (int i = 0; i < dg.sizeX(); i++) {
			for (int j = 0; j < dg.sizeY(); j++) {
				//fpln(" $$$  testOptimalPath:  dg.getPosition("+i+","+j+") = ("+dg.getPosition(i,j).x()+","+dg.getPosition(i,j).y()+")");
				EXPECT_NEAR(i*squareSize,dg.getPosition(i,j).x(),0.000001);
				EXPECT_NEAR(j*squareSize,dg.getPosition(i,j).y(),0.000001);
				//fpln(" $$$  testOptimalPath:  dg.center("+i+","+j+") = ("+dg.center(i,j).x()+","+dg.center(i,j).y()+")");
				EXPECT_NEAR(i+0.5,dg.center(i,j).x(),0.000001);
				EXPECT_NEAR(j+0.5,dg.center(i,j).y(),0.000001);
			}
		}	
		EXPECT_EQ(6,dg.sizeX());
		EXPECT_EQ(4,dg.sizeY());



		//		90     90     27     12     91     0   
		//		15     32     11     17     93     16 		
		//		13     12     30     70      2     41 		
		//		1       5      9     50     41     63 

		dg.setWeight(0, 3, 90.0);
		dg.setWeight(1, 3, 90.0);
		dg.setWeight(2, 3, 27.0);
		dg.setWeight(3, 3, 12.0);
		dg.setWeight(4, 3, 91.0);
		dg.setWeight(5, 3, 0.0);

		dg.setWeight(0, 2, 15.0);
		dg.setWeight(1, 2, 32.0);
		dg.setWeight(2, 2, 11.0);
		dg.setWeight(3, 2, 17.0);
		dg.setWeight(4, 2, 93.0);
		dg.setWeight(5, 2, 16.0);

		dg.setWeight(0, 1, 13.0);
		dg.setWeight(1, 1, 12.0);
		dg.setWeight(2, 1, 30.0);
		dg.setWeight(3, 1, 70.0);
		dg.setWeight(4, 1, 2.0);
		dg.setWeight(5, 1, 41.0);

		dg.setWeight(0, 0, 1.0);
		dg.setWeight(1, 0, 5.0);
		dg.setWeight(2, 0, 9.0);
		dg.setWeight(3, 0, 50.0);
		dg.setWeight(4, 0, 41.0);
		dg.setWeight(5, 0, 63.0);


		DensityGridAStarSearch dgs; //  = DensityGridAStarSearch();
		dgs.dirWeight = 0.0;
		dgs.distWeight = 0.0;
		std::vector<std::pair<int,int> > gPath =  dgs.optimalPath(dg);

		//dg.printSearchedWeights();		

		//		for (int i = 0; i < gPath.size(); i++) {
		//			std::pair<int,int> ijPair = gPath.get(i);
		//			fpln(" i = "+i+" ijPair = "+ijPair);
		//		}	

		//  optimal path: (0,0) -> (1,0) -> (2,0) -> (3,0) -> (4,0)	-> (4,1) -> (5,1) -> (5,2) -> (5,3)	

		//dg.printGridPath(gPath);
		
		EXPECT_EQ((unsigned long)7,gPath.size());
		EXPECT_EQ(0,(int) gPath[0].first);
		EXPECT_EQ(0,(int) gPath[0].second);

		EXPECT_EQ(1,(int) gPath[1].first);
		EXPECT_EQ(1,(int) gPath[1].second);

		EXPECT_EQ(2,(int) gPath[2].first);
		EXPECT_EQ(2,(int) gPath[2].second);

		EXPECT_EQ(3,(int) gPath[3].first);
		EXPECT_EQ(2,(int) gPath[3].second);

		EXPECT_EQ(4,(int) gPath[4].first);
		EXPECT_EQ(1,(int) gPath[4].second);

		EXPECT_EQ(5,(int) gPath[5].first);
		EXPECT_EQ(2,(int) gPath[5].second);

		EXPECT_EQ(5,(int) gPath[6].first);
		EXPECT_EQ(3,(int) gPath[6].second);

//		EXPECT_EQ(5,(int) gPath[7].first);
//		EXPECT_EQ(2,(int) gPath[7].second);
//
//		EXPECT_EQ(5,(int) gPath[8].first);
//		EXPECT_EQ(3,(int) gPath[8].second);
	}


	
	TEST_F(DensityGridTest, testOptimalPathLatLon) {
		Position p1 = Position::makeLatLonAlt(10.0, 10.0, 10000);
		Position p2 = Position::makeLatLonAlt(11.1, 11.5, 10000);
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		Plan p;
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		double squareSize_d = Units::from("deg",0.2);
		double squareSize_m = DensityGrid::distEstLatLon(0,squareSize_d);
		DensityGrid dg = DensityGrid(p, 0, squareSize_m);

		//TODO: one larger, remember?  is this what we want?
		EXPECT_EQ(9,dg.sizeX());
		EXPECT_EQ(7,dg.sizeY());

		//		90     90     97     33     91     90    60    80*
		//		15     32     11     17     93     16     0    42
		//		10     70     30     70     42     41    70    23
		//		20     95     59     10     41     23    96    34
		//		20     95     49     55     15     13    97    45
		//		1*     95     39     50     41     63    10     6

		dg.setWeight(0, 5, 90.0);
		dg.setWeight(1, 5, 90.0);
		dg.setWeight(2, 5, 97.0);
		dg.setWeight(3, 5, 33.0);
		dg.setWeight(4, 5, 91.0);
		dg.setWeight(5, 5, 90.0);
		dg.setWeight(6, 5, 60.0);
		dg.setWeight(7, 5, 80.0);

		dg.setWeight(0, 4, 15.0);
		dg.setWeight(1, 4, 32.0);
		dg.setWeight(2, 4, 11.0);
		dg.setWeight(3, 4, 17.0);
		dg.setWeight(4, 4, 93.0);
		dg.setWeight(5, 4, 16.0);
		dg.setWeight(6, 4, 0.0);
		dg.setWeight(7, 4, 42.0);

		dg.setWeight(0, 3, 10.0);
		dg.setWeight(1, 3, 70.0);
		dg.setWeight(2, 3, 30.0);
		dg.setWeight(3, 3, 70.0);
		dg.setWeight(4, 3, 42.0);
		dg.setWeight(5, 3, 41.0);
		dg.setWeight(6, 3, 70.0);
		dg.setWeight(7, 3, 23.0);

		dg.setWeight(0, 2, 20.0);
		dg.setWeight(1, 2, 95.0);
		dg.setWeight(2, 2, 59.0);
		dg.setWeight(3, 2, 10.0);
		dg.setWeight(4, 2, 41.0);
		dg.setWeight(5, 2, 23.0);
		dg.setWeight(6, 2, 96.0);
		dg.setWeight(7, 2, 34.0);

		dg.setWeight(0, 1, 20.0);
		dg.setWeight(1, 1, 95.0);
		dg.setWeight(2, 1, 49.0);
		dg.setWeight(3, 1, 55.0);
		dg.setWeight(4, 1, 15.0);
		dg.setWeight(5, 1, 13.0);
		dg.setWeight(6, 1, 97.0);
		dg.setWeight(7, 1, 45.0);

		dg.setWeight(0, 0, 1.0);
		dg.setWeight(1, 0, 95.0);
		dg.setWeight(2, 0, 39.0);
		dg.setWeight(3, 0, 50.0);
		dg.setWeight(4, 0, 41.0);
		dg.setWeight(5, 0, 63.0);
		dg.setWeight(6, 0, 10.0);
		dg.setWeight(7, 0, 6.0);

		//dg.printWeights();
		//dg.printCorners();

		//fpln(" $$ testOptimalPathLatLon: "+dg.gridPosition(dg.startPoint)+" "+dg.gridPosition(dg.endPoint));

		DensityGridAStarSearch dgs = DensityGridAStarSearch();
		std::vector<std::pair<int,int> > gPath = dgs.optimalPath(dg);
		//		for (int i = 0; i < gPath.size(); i++) {
		//			std::pair<int,int> ijPair = gPath[i);
		//			fpln(" i = "+i+" ijPair = "+ijPair);
		//		}

		//  optimal path: (0,0) -> (0,1) -> (0,2) -> (0,3) -> (0,4) -> (1,4) -> (2,4) -> (3,4) -> (4,3)
		//                -> (5,4) -> (6,4) -> (7,5)

		// NOPE~~~

		// real optimal path: (0,0) -> (0,1) -> (0,2) -> (0,3) -> (1,4) -> (2,4) -> (3,4) -> (4,3) -> (5,4) -> (6,4) -> (7,5)

		// real optimal path: (0,0) -> (0,1) -> (0,2) -> (0,3) -> (0,4) -> (1,4) -> (2,4) -> (3,4) -> (4,4) -> (5,4) -> (6,4) -> (7,4) -> (7,5)

		//dg.printGridPath(gPath);

		EXPECT_EQ((unsigned long)11,gPath.size());
		EXPECT_EQ(0,(int) gPath[0].first);
		EXPECT_EQ(0,(int) gPath[0].second);

		EXPECT_EQ(0,(int) gPath[1].first);
		EXPECT_EQ(1,(int) gPath[1].second);

		EXPECT_EQ(0,(int) gPath[2].first);
		EXPECT_EQ(2,(int) gPath[2].second);

		EXPECT_EQ(0,(int) gPath[3].first);
		EXPECT_EQ(3,(int) gPath[3].second);

		EXPECT_EQ(1,(int) gPath[4].first);
		EXPECT_EQ(4,(int) gPath[4].second);

		EXPECT_EQ(2,(int) gPath[5].first);
		EXPECT_EQ(4,(int) gPath[5].second);

		EXPECT_EQ(3,(int) gPath[6].first);
		EXPECT_EQ(4,(int) gPath[6].second);

		EXPECT_EQ(4,(int) gPath[7].first);
		EXPECT_EQ(3,(int) gPath[7].second);

		EXPECT_EQ(5,(int) gPath[8].first);
		EXPECT_EQ(4,(int) gPath[8].second);

		EXPECT_EQ(6,(int) gPath[9].first);
		EXPECT_EQ(4,(int) gPath[9].second);

		EXPECT_EQ(7,(int) gPath[10].first);
		EXPECT_EQ(5,(int) gPath[10].second);

//		EXPECT_EQ(7,(int) gPath[11].first);
//		EXPECT_EQ(4,(int) gPath[11].second);
//
//		EXPECT_EQ(7,(int) gPath[12].first);
//		EXPECT_EQ(5,(int) gPath[12].second);



		dg.setProximityWeights((std::vector<std::pair<int,int> >) gPath, 1.0, true);
		//dg.printWeights();

	}



	TEST_F(DensityGridTest, testGridPath) {
		Position p1 = Position::mkXYZ(0.0, 0.0, Units::from("ft",10000));
		Position p2 = Position::mkXYZ(5000.0, 3000.0, Units::from("ft",12000));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,100.0);
		Plan p;
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ testGridPath: gs = "+Units::str("kn", np1.initialVelocity(np2].gs()));
		double squareSize = 1000.0;
		//fpln(" $$ testGridPath : squareSize = "+squareSize);
		DensityGrid dg = DensityGrid(p, 0, squareSize);
		std::vector<std::pair<int,int> > gPath = dg.gridPath(p);
		//		for (int i = 0; i < gPath.size(); i++) {
		//			std::pair<int,int> ijPair = gPath[i);
		//			fpln(" i = "+i+" ijPair = "+ijPair);
		//		}
		double gs = Units::from("kn", 400);
		double vs = np1.initialVelocity(np2).vs();
		//fpln(" $$ vs = "+Units::str("fpm", vs));
		Plan plan = dg.gridPathToPlan(gPath, gs, vs);
		//DebugSupport.dumpPlan(plan,"testGridPath");

		EXPECT_EQ((unsigned long)8,gPath.size());

		EXPECT_EQ(0,(int) gPath[0].first);
		EXPECT_EQ(0,(int) gPath[0].second);

		EXPECT_EQ(1,(int) gPath[1].first);
		EXPECT_EQ(0,(int) gPath[1].second);

		EXPECT_EQ(1,(int) gPath[2].first);
		EXPECT_EQ(1,(int) gPath[2].second);

		EXPECT_EQ(2,(int) gPath[3].first);
		EXPECT_EQ(1,(int) gPath[3].second);

		EXPECT_EQ(3,(int) gPath[4].first);
		EXPECT_EQ(1,(int) gPath[4].second);

		EXPECT_EQ(3,(int) gPath[5].first);
		EXPECT_EQ(2,(int) gPath[5].second);

		EXPECT_EQ(4,(int) gPath[6].first);
		EXPECT_EQ(2,(int) gPath[6].second);

		EXPECT_EQ(5,(int) gPath[7].first);
		EXPECT_EQ(3,(int) gPath[7].second);
	}


	TEST_F(DensityGridTest, testSetProximityWeights) {
		Position p1 = Position::mkXYZ(0.0, 0.0, Units::from("ft",10000));
		Position p2 = Position::mkXYZ(5000.0, 3000.0, Units::from("ft",12000));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,100.0);
		Plan p;
		p.addNavPoint(np1);
		p.addNavPoint(np2);
		//fpln(" $$ gs = "+Units::str("kn", np1.initialVelocity(np2).gs()));
		double squareSize = 1000.0;
		//fpln(" $$ testGridPath : squareSize = "+squareSize);
		DensityGrid dg = DensityGrid(p, 0, squareSize);
		std::vector<std::pair<int,int> > gPath = dg.gridPath(p);
		dg.setProximityWeights(gPath, 1.0, true);
		//dg.printWeights();
	}



	//	
	//	void testSetProximityWeights2() {	
	//		PolyReader r = PolyReader("../tests/Poly001.txt");
	//		//EXPECT_EQ(1, r.size());
	//        //fpln(" r.plans = "+r.plans);
	//        //fpln(" r.paths = "+r.paths);
	//        Plan ownship = r.plans.get(0);
	//		double squareSize = Units::from("nmi", 10);
	//		DensityGrid dg = DensityGrid(ownship, 3, squareSize);
	//		std::vector<std::pair<int,int> > gPath = dg.gridPath(ownship);
	//		dg.printGridPath(gPath);
	//		dg.setProximityWeights(gPath, 1.0, true);		
	//		//dg.printWeights();
	//	}
	//	
	//	
	//	
	//	void setPolyWeights() {	
	//		PolyReader r = PolyReader("../tests/Poly001.txt");
	//		//EXPECT_EQ(1, r.size());
	//        //fpln(" r.plans = "+r.plans);
	//        //fpln(" r.paths = "+r.paths);
	//        Plan ownship = r.plans.get(0);
	//		double squareSize = Units::from("nmi", 10);
	//		DensityGrid dg = DensityGrid(ownship, 3, squareSize);
	////		fpln("  $$$$$$$$ dg.size() = "+dg.size());
	////		fpln("  $$$$$$$$ dg = "+dg);
	////		fpln("");
	//		std::vector<std::pair<int,int> > gPath = dg.gridPath(ownship);
	//		//dg.printGridPath(gPath);
	//		dg.setPolyWeights(610,r.paths);
	//		
	//		//dg.printWeights();
	//		//DebugSupport.dumpDensityGrid(dg, "grid");
	//		
	//	}
	//
	//
	//	
	//	void movingPolyGrid() {	
	//		PolyReader r = PolyReader("../tests/Poly001.txt");
	//		//EXPECT_EQ(1, r.size());
	//        fpln(" r.plans = "+r.plans);
	//        fpln(" r.paths = "+r.paths);
	//        //fpln("-----------------");
	//        
	//        Plan ownship = r.plans.get(0);
	//        double gs = ownship.averageGroundSpeed();
	//        Arraystd::vector<PolyPath> paths = Arraystd::vector<PolyPath>();
	//        paths.add(r.getPolyPath(0));
	//		double squareSize = Units::from("nmi", 1);
	//		DensityGridMovingPolys dg = DensityGridMovingPolys(ownship, 3, squareSize, gs, paths, null);
	//		
	//		//fpln("Grid="+dg.sizeX()+" "+dg.sizeY()+" = "+dg.size());
	//		
	//		dg.setWeights(1.0);
	//		std::vector<std::pair<int,int> > origpath = dg.gridPath(ownship);
	//		//dg.printGridPath(origpath);
	//		dg.setProximityWeights(origpath, 1.0, true);
	//		std::vector<std::pair<int,int> > gPath = dg.optimalPath();
	//		//dg.printGridPath(gPath);
	//
	//		//fpln(f.Fobj(gPath));
	//		
	//		Plan plan2 = dg.gridPathToPlan(gPath,gs,0.0,false);
	//		
	//		//DebugSupport.dumpPlan(plan2, "gridplan");
	//	}
	//
	//	
	//	void movingPolyGridNasty() {	
	//		PolyReader r = PolyReader("../tests/Poly005.txt");
	//		//EXPECT_EQ(1, r.size());
	//        
	//        Plan ownship = r.plans.get(0);
	//        double gs = ownship.averageGroundSpeed();
	//        Arraystd::vector<PolyPath> paths = Arraystd::vector<PolyPath>();
	//        for (int i = 0; i < r.polySize(); i++) {
	//        	paths.add(r.getPolyPath(i));
	////        	paths.add(r.getPolyPath(i).simplify(Units::from("nmi", 1.0)));
	//        }
	//		double squareSize = Units::from("nmi", 20);
	//		DensityGridMovingPolysEst dg = DensityGridMovingPolysEst(ownship, 3, squareSize, gs, paths, null);
	//		dg.snapToStart();
	//
	//		//fpln("Grid="+dg.sizeX()+" "+dg.sizeY()+" = "+dg.size());
	//		
	//		dg.setWeights(1.0);
	//		std::vector<std::pair<int,int> > origpath = dg.gridPath(ownship);
	//		//dg.printGridPath(origpath);
	//		dg.setProximityWeights(origpath, 1.0, true);
	//
	////		DebugSupport.dumpDensityGrid(dg, "gridcorners");
	//
	////		DebugSupport.dumpDensityGrid(dg, "gridcorners", 850.0);
	//
	//		long starttime = System.currentTimeMillis();
	//		
	//		std::vector<std::pair<int,int> > gPath = dg.optimalPath();
	//		
	//		long endtime = System.currentTimeMillis();
	//		
	//		//dg.printGridPath(gPath);
	//
	//		//fpln(f.Fobj(gPath));
	//		
	//		//fpln("Total time = "+(endtime-starttime)/1000.0);
	//		
	//		Plan plan2 = dg.gridPathToPlan(gPath,gs,0.0,false);
	//		
	//		//DebugSupport.dumpPlan(plan2, "gridplannasty");
	//	}


	//	
	//	void movingPolyGridNastyEst() {	
	//		PolyReader r = PolyReader("../tests/Poly005.txt");
	//		//EXPECT_EQ(1, r.size());
	//        
	//        Plan ownship = r.plans.get(0);
	//        double gs = ownship.averageGroundSpeed();
	//        Arraystd::vector<PolyPath> paths = Arraystd::vector<PolyPath>();
	//        for (int i = 0; i < r.polySize(); i++) {
	//        	paths.add(r.getPolyPath(i));
	//        }
	//		double squareSize = Units::from("nmi", 11);
	//		DensityGridMovingPolysEst2 dg = DensityGridMovingPolysEst2(ownship, 3, squareSize, gs, paths);
	//		dg.snapToStart();
	//		
	//		fpln("Grid="+dg.sizeX()+" "+dg.sizeY()+" = "+dg.size());
	//		
	//		dg.resetWeights(1.0);
	//		std::vector<std::pair<int,int> > origpath = dg.gridPath(ownship);
	//		DensityGrid::printGridPath(origpath);
	//		dg.setProximityWeights(origpath);
	//		
	//		long starttime = System.currentTimeMillis();
	//		
	//		std::vector<std::pair<int,int> > gPath = dg.optimalPath();
	//		
	//		long endtime = System.currentTimeMillis();
	//		
	//		DensityGrid::printGridPath(gPath);
	//
	//		fpln(f.Fobj(gPath));
	//		
	//		fpln("Total time = "+(endtime-starttime)/1000.0);
	//		
	//		Plan plan2 = dg.gridPathToPlan(gPath,gs,0.0);
	//		DebugSupport.dumpDensityGrid(dg, "gridcorners");
	//		DebugSupport.dumpPlan(plan2, "gridplannasty");
	//	}
	//	






//	
//	void polySimplifier() {	
//		PolyReader r = PolyReader("../tests/Poly005.txt");
//		//fpln(r.getMessage());
//		EXPECT_FALSE(r.hasMessage());
//		PolyPath polypath0 = r.getPolyPath(0);
//		SimplePoly poly0 = polypath0.getPolyRef(0);
//		//fpln(" $$$$ poly0 = "+poly0.size());
//		DebugSupport.dumpPoly(poly0,"poly0");
//		SimplePoly poly1 = poly0.buildContainingPoly(11);
//		SimplePoly poly2 = poly1.buildContainingPoly(5);
//		SimplePoly poly3 = poly2.buildContainingPoly(7);
//		//fpln(" $$$$ poly1 = "+poly1.size());
//		////fpln(" $$$$ poly2 = "+poly2.size());
//		//fpln(" $$$$ poly3 = "+poly3.size());
//		//DebugSupport.dumpSimplePoly(poly3,"poly3");
//	}	

//	
//	void sweeTest() {
//		BoundingRectangle box = BoundingRectangle();
//
//		Position v1 = Position::makeXYZ(0, 0, 0);
//		Position v2 = Position::makeXYZ(11.0,11.0, 0);
//
//		box.add(v1);
//		box.add(v2);
//
//		System.out.println("Bounding rectangle:"+box.toString());
//
//		NavPoint start = NavPoint::makeXYZ(1,1,0,0);
//		Position end   = Position::makeXYZ(10.0,10.0,0.0);
//
//		DensityGrid grid = DensityGrid(box, start, end, 1, Units::from("nmi",1.0), false);
//
////		grid.setWeights(1.0);
//		
//		std::vector<std::pair<int,int> > path = grid.optimalPath();
//		
//		grid.printGridPath(path);
//
//	}

