/**
 * @file CoreLogic.cpp
 * @brief Mapping between triggers and handlers
 * 
 * @details 
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
 */

#include "Triggers.hpp" // Trigger function definitions
#include "Handlers.hpp" // Handler object definitions

/**
 * @brief Initialize event handlers
 */
void Cognition::InitializeEventHandlers(){


   /// Get input priorities  
   auto inputPriorities  = GetPriorityValues(&cogState.parameters);

   /// Nominal mission related triggers
   eventMng.AddEventHandler("Takeoff",
                            inputPriorities["Takeoff"],
                            TakeoffTrigger,
                            MAKE_HANDLER(TakeoffPhaseHandler));


   eventMng.AddEventHandler("NominalDeparture",
                            inputPriorities["NominalDeparture"],
                            NominalDepartureTrigger,
                            MAKE_HANDLER(EngageNominalPlan));

   eventMng.AddEventHandler("PrimaryPlanComplete",
                             inputPriorities["PrimaryPlanComplete"],
                             PrimaryPlanCompletionTrigger,
                             MAKE_HANDLER(LandPhaseHandler));

   eventMng.AddEventHandler("FlightReplan",
                             inputPriorities["Replanning"],
                             FlightReplanTrigger,
                             MAKE_HANDLER(ReturnToNextFeasibleWP));

   eventMng.AddEventHandler("Merging",
                             inputPriorities["Merging"],
                             MergingActivityTrigger,
                             MAKE_HANDLER(MergingHandler));

   eventMng.AddEventHandler("SecondaryPlanComplete",
                             inputPriorities["SecondaryPlanComplete"],
                             SecondaryPlanCompletionTrigger,
                             MAKE_HANDLER(EngageNominalPlan));

   /// Conflict related triggers
   eventMng.AddEventHandler("FenceConflict",
                             inputPriorities["FenceConflict"],
                             FenceConflictTrigger,
                             MAKE_HANDLER(ReturnToLaunch));
                             //MAKE_HANDLER(ReturnToNextFeasibleWP));

   eventMng.AddEventHandler("TrafficConflict1",
                             inputPriorities["TrafficConflict1"],
                             TrafficConflictVectorResTrigger,
                             MAKE_HANDLER(TrafficConflictHandler));

   eventMng.AddEventHandler("TrafficConflict2",
                             inputPriorities["TrafficConflict2"],
                             TrafficConflictPathResTrigger,
                             MAKE_HANDLER(ReturnToMission));

   eventMng.AddEventHandler("FlightPlanDeviation",
                             inputPriorities["FlightPlanDeviation"],
                             FlightPlanDeviationTrigger,
                             MAKE_HANDLER(ReturnToMission));

   /// Ditching related triggers
   eventMng.AddEventHandler("TrafficConflict3",
                             inputPriorities["TrafficConflict3"],
                             TrafficConflictDitchTrigger,
                             MAKE_HANDLER(RequestDitchSite));

   eventMng.AddEventHandler("Ditching",
                             inputPriorities["Ditching"],
                             DitchingTrigger,
                             MAKE_HANDLER(ProceedToDitchSite));

   eventMng.AddEventHandler("TODReached",
                             inputPriorities["TODReached"],
                             DitchSiteTODTrigger,
                             MAKE_HANDLER(ProceedFromTODtoLand));

}
