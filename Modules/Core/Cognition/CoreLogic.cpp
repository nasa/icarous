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
   eventMng.AddEventHandler("Takeoff",1,TakeoffTrigger,new TakeoffPhaseHandler);
   eventMng.AddEventHandler("NominalDeparture",1,NominalDepartureTrigger,new EngageNominalPlan);
   eventMng.AddEventHandler("PrimaryPlanComplete",1,PrimaryPlanCompletionTrigger,new LandPhaseHandler);
   eventMng.AddEventHandler("Merging",3,MergingActivityTrigger,new MergingHandler);
   eventMng.AddEventHandler("SecondaryPlanComplete",1,SecondaryPlanCompletionTrigger,new EngageNominalPlan);

   // Conflict related triggers
   eventMng.AddEventHandler("FenceConflict",1,FenceConflictTrigger,new ReturnToNextFeasibleWP);
   eventMng.AddEventHandler("TrafficConflict1",2,TrafficConflictVectorResTrigger,new TrafficConflictHandler);
   eventMng.AddEventHandler("TrafficConflict2",2,TrafficConflictPathResTrigger,new ReturnToMission);
   eventMng.AddEventHandler("FlightPlanDeviation",1,FlightPlanDeviationTrigger,new ReturnToMission);

   // Ditching related triggers
   eventMng.AddEventHandler("TrafficConflict3",4,TrafficConflictDitchTrigger,new RequestDitchSite);
   eventMng.AddEventHandler("Ditching",5,DitchingTrigger,new ProceedToDitchSite);
   eventMng.AddEventHandler("TODReached",1,DitchSiteTODTrigger,new ProceedFromTODtoLand);

}