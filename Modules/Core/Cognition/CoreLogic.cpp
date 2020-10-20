#include "Triggers.hpp" // Trigger function definitions
#include "Handlers.hpp" // Handler object definitions

// Event handler mappings
void Cognition::InitializeEventHandlers(){

   /**
    * AddEventHandler 
    * This is a simple framework to associate events (or triggers)
    * with a suitable action (or handler).
    * inputs:
    *   - event name: string
    *   - priority: integer
    *   - event function: [state->bool] 
    *   - handler object: EventHandler<state> 
    * Here 'state' is any data type. 
    * 
    * Describe all the events that should be detected and 
    * provide appropriate handlers to deal with the respective event.
    * The provided priority is used to determine which event is addressed first.
    **/

   // Nominal mission related triggers
   eventMng.AddEventHandler("Takeoff",1,TakeoffTrigger,&takeoffHandler);
   eventMng.AddEventHandler("NominalDeparture",1,NominalDepartureTrigger,&engageNominalPlan);
   eventMng.AddEventHandler("PrimaryPlanComplete",1,PrimaryPlanCompletionTrigger,&landHandler);
   eventMng.AddEventHandler("Merging",3,MergingActivityTrigger,&mergingHandler);
   eventMng.AddEventHandler("SecondaryPlanComplete",1,SecondaryPlanCompletionTrigger,&engageNominalPlan);

   // Conflict related triggers
   eventMng.AddEventHandler("FenceConflict",1,FenceConflictTrigger,&returnToNextFeasibleWP);
   eventMng.AddEventHandler("TrafficConflict1",2,TrafficConflictVectorResTrigger,&trafficConflictHandler);
   eventMng.AddEventHandler("TrafficConflict2",2,TrafficConflictPathResTrigger,&returnToMission);
   eventMng.AddEventHandler("FlightPlanDeviation",1,FlightPlanDeviationTrigger,&returnToMission);

   // Ditching related triggers
   eventMng.AddEventHandler("TrafficConflict3",4,TrafficConflictDitchTrigger,&requestDitchSite);
   eventMng.AddEventHandler("Ditching",5,DitchingTrigger,&proceedToDitchSite);
   eventMng.AddEventHandler("TODReached",1,DitchSiteTODTrigger,&proceedFromTODtoLand);

}