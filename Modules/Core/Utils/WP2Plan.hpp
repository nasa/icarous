#ifndef WP2PLAN_HPP
#define WP2PLAN_HPP

#include <string>
#include <list>
#include "Plan.h"
#include "Interfaces.h"

void ConvertWPList2Plan(larcfm::Plan* fp,const std::string &plan_id, const std::list<waypoint_t> &waypoints, const double initHeading,bool repair);
void GetWaypointFromPlan(const larcfm::Plan* fp,const int id,waypoint_t &wp);
#endif