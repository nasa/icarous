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
   auto inputPriorities  = GetPriorityValues();

   // Nominal mission related triggers
   eventMng.AddEventHandler("Takeoff",
                            inputPriorities["Takeoff"],
                            TakeoffTrigger,
                            new TakeoffPhaseHandler);

   eventMng.AddEventHandler("NominalDeparture",
                            inputPriorities["NominalDeparture"],
                            NominalDepartureTrigger,
                            new EngageNominalPlan);

   eventMng.AddEventHandler("PrimaryPlanComplete",
                             inputPriorities["PrimaryPlanComplete"],
                             PrimaryPlanCompletionTrigger,
                             new LandPhaseHandler);

   eventMng.AddEventHandler("Merging",
                             inputPriorities["Merging"],
                             MergingActivityTrigger,
                             new MergingHandler);

   eventMng.AddEventHandler("SecondaryPlanComplete",
                             inputPriorities["SecondaryPlanComplete"],
                             SecondaryPlanCompletionTrigger,
                             new EngageNominalPlan);

   // Conflict related triggers
   eventMng.AddEventHandler("FenceConflict",
                             inputPriorities["FenceConflict"],
                             FenceConflictTrigger,
                             new ReturnToNextFeasibleWP);

   eventMng.AddEventHandler("TrafficConflict1",
                             inputPriorities["TrafficConflict1"],
                             TrafficConflictVectorResTrigger,
                             new TrafficConflictHandler);

   eventMng.AddEventHandler("TrafficConflict2",
                             inputPriorities["TrafficConflict2"],
                             TrafficConflictPathResTrigger,
                             new ReturnToMission);

   eventMng.AddEventHandler("FlightPlanDeviation",
                             inputPriorities["FlightPlanDeviation"],
                             FlightPlanDeviationTrigger,
                             new ReturnToMission);

   // Ditching related triggers
   eventMng.AddEventHandler("TrafficConflict3",
                             inputPriorities["TrafficConflict3"],
                             TrafficConflictDitchTrigger,
                             new RequestDitchSite);

   eventMng.AddEventHandler("Ditching",
                             inputPriorities["Ditching"],
                             DitchingTrigger,
                             new ProceedToDitchSite);

   eventMng.AddEventHandler("TODReached",
                             inputPriorities["TODReached"],
                             DitchSiteTODTrigger,
                             new ProceedFromTODtoLand);

}