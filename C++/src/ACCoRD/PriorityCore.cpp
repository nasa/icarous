/* Core Priority Rules Computation 
 * Authors: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "PriorityCore.h"
#include "Util.h"

namespace larcfm {
	//double PriorityCore::CRUISE_DEF;
	//double PriorityCore::VERTICAL_DEADBAND;
	//double PriorityCore::LATERAL_DEADBAND;

	bool PriorityCore::isDescending(const Vect3& v){
		return v.z <=  -CRUISE_DEF - VERTICAL_DEADBAND;
	}
	
	bool PriorityCore::isNotDescending(const Vect3& v) {
		return v.z >= -CRUISE_DEF + VERTICAL_DEADBAND;
	}
	
	bool PriorityCore::isCruising(const Vect3& v) {
		return std::abs(v.z) <= CRUISE_DEF - VERTICAL_DEADBAND;
	}
	
    bool PriorityCore::isAscending(const Vect3& v) {
    	return v.z >= CRUISE_DEF + VERTICAL_DEADBAND;
    }
    bool PriorityCore::isNotAscending(const Vect3& v) {
      	return v.z <= CRUISE_DEF - VERTICAL_DEADBAND;
    }
    
    bool PriorityCore::isNotCruising(const Vect3& v) {
    	return (isDescending(v) || isAscending(v));
    	}
    
    bool PriorityCore::PossiblyDescendingORCruising(const Vect3& v) {
    	return (std::abs(v.z + CRUISE_DEF))< VERTICAL_DEADBAND;
    }
    
    bool PriorityCore::PossiblyCruisingORAscending(const Vect3& v) {
    	return (std::abs(v.z - CRUISE_DEF)) < VERTICAL_DEADBAND;
    }
    
//    /**
//     * 
//     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
//     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
//     * @return
//     */
//    bool ModeCloseOneWay(const Vect3& ao_V, const Vect3&  ai_V){
//        return  ( (isDescending(ao_V) && !isNotDescending(ai_V)) ||
//        		  (isCruising(ao_V) && !isNotCruising(ai_V))  ||
//        		  (isAscending(ao_V) && !isNotAscending(ai_V))
//        		);	
//    }
//    
//    /**
//     * 
//     * @param ao_V  a 3D vector representing ownship aircraft velocity vector
//     * @param ai_V   a 3D vector representing traffic  aircraft velocity vector
//     * @return
//     */
//    bool ModeClose(const Vect3& ao_V, const Vect3& ai_V){
//    	return (ModeCloseOneWay(ao_V,ai_V) || ModeCloseOneWay(ai_V,ao_V));
//    }

    bool PriorityCore::HorizontalRulesApply(const Vect3& ao_V, const Vect3& ai_V) {
    	return ( (isCruising(ao_V) && isCruising(ai_V)) ||
    			 (isDescending(ao_V) && isDescending(ai_V)) ||
    			 (isAscending(ao_V) && isAscending(ai_V))
    			);	
    }
    
    bool PriorityCore::VerticalRulesApply(const Vect3&  ao_V, const Vect3& ai_V) {
    	return ( (isDescending(ao_V) && isNotDescending(ai_V)) ||
    			(isDescending(ai_V) && isNotDescending(ao_V)) ||
    			(isCruising(ao_V) && isNotCruising(ai_V)) ||
    			(isCruising(ai_V) && isNotCruising(ao_V)) ||
    			(isAscending(ao_V) && isNotAscending(ai_V)) ||
    			(isAscending(ai_V) && isNotAscending(ao_V)) 
    			);
    }
    
   double PriorityCore::VelComponent(const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi) {
       Vect2 w;
       Vect2 z;
       w = (so.Sub(si)).vect2();
       z = (vo.Add(vi)).vect2();
       if ( ! w.isZero()) {
           return w.Scal(1.0/w.norm()).dot(z);
       } else {
           return 0.0;
       }
   }
    

     double PriorityCore::VelComponentAt(const double t, const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi) {
       Vect3 w;
       Vect3 z;
       w = vo.Scal(t);
       z = vi.Scal(t);
       return VelComponent(so.Add(w), si.Add(z), vo, vi);
  }
    
  double PriorityCore::EntryComponent(const Vect3& so, const Vect3& si, const Vect3& vo, const Vect3& vi) {
       Vect3 w;
       Vect3 z;
       double x;
       double timeat;
        w = vo.Sub(vi);
     z = so.Sub(si);
     if ((w.vect2()).norm() < LATERAL_RELATIVE_DEADBAND) {
       return 0;
     }
     else{
       x = CD2D::tcpa(z.vect2(), vo.vect2(), vi.vect2(),0,PINFINITY);
       timeat = Util::max(0.0, x-TC);
       return VelComponentAt(timeat,so,si,vo,vi); 
     }
   }   

   bool PriorityCore::HorizontalDeadband(const Vect3& ao_S, const Vect3& ao_V, const Vect3& ai_S, const Vect3& ai_V) {
       
        return (std::abs(EntryComponent(ao_S, ai_S, ao_V, ai_V) ) < LATERAL_DEADBAND);
    }
    
   bool PriorityCore::VerticalDeadband(const Vect3&  ao_V, const Vect3& ai_V){
     return ( (PossiblyDescendingORCruising(ao_V) && ! isAscending(ai_V)) ||
         (PossiblyCruisingORAscending(ao_V) && ! isDescending(ai_V)) ||
         (PossiblyDescendingORCruising(ai_V) && ! isAscending(ao_V)) ||
         (PossiblyCruisingORAscending(ai_V) && ! isDescending(ao_V))
     );
   }
  
//  /**
//   * 
//   * @param ao_S a 3D vector representing the position vector of ownship
//   * @param ao_V a 3D veector representing the velocity vector of the ownship
//   * @param ai_S a 3D vector representing the position vector of traffic 
//   * @param ai_V a 3D vector representing the velocity vector of traffic 
//   * @return
//   */
//  bool TotalDeadband(const Vect3& ao_S, const Vect3& ao_V, const Vect3& ai_S, const Vect3& ai_V){
//	   return ( VerticalDeadband(ao_V,ai_V) || VerticalDeadband(ai_V,ao_V) ||
//			   (HorizontalRulesApply(ao_V, ai_V) && HorizontalDeadband(ao_S,ao_V,ai_S,ai_V))
//			   );
//  }
  
	bool PriorityCore::lateral_only(const Vect3&  so, const Vect3& vo, const Vect3& si, const Vect3& vi) {
	   return (!HorizontalDeadband(so,vo,si,vi)  && (EntryComponent(so,si,vo,vi) > 0));
	}
	
	bool PriorityCore::check(const Vect3& so, const Vect3& vo, bool mo, const Vect3& si, const Vect3& vi, bool mi) {
	      if ( mo ) { 
	    	  return true;
	      } else if ( mi ) {
	    	  return false;
	      } else if ( VerticalDeadband(vo,vi) ) {
	    	  return false;
	      } else if ( VerticalRulesApply(vo, vi) ) {
	    	  return (vo.z < vi.z);
	      } else if ( HorizontalRulesApply(vo, vi) ) {
	    	  return lateral_only(so, vo,si,vi);
	      } else {
	    	  return true;
	      }
	     
	}
	
	
}
