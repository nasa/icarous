#ifndef WP2PLAN_HPP
#define WP2PLAN_HPP

#include <string>
#include <list>
#include "Plan.h"
#include "Interfaces.h"

typedef std::vector<std::vector<std::function<double(double)>>> trajTimeFunction;

void ConvertWPList2Plan(larcfm::Plan* fp,const std::string &plan_id, const std::list<waypoint_t> &waypoints, 
                        const double initHeading,bool repair,double repairTurnRate);
void GetWaypointFromPlan(const larcfm::Plan* fp,const int id,waypoint_t &wp);
trajTimeFunction ConvertEUTL2TimeFunction(const larcfm::Plan* fp);
#endif