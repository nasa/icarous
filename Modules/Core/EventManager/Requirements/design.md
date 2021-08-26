# Event Manager

## What is it?
The event manager is a header only template based library that provides a framework to define triggers that indiciate the onset of a specific event of interest. The framework also enables associating a trigger to a specific handler. In otherwords, the event manager triggers specific handler functions when an event is detected by the trigger. The event manager is an expressive framework to construct reactive (autonomous) systems by defining triggers and associating triggers with specific behaviors via handlers.

## How does it work?
- A trigger is a function that takes a state of type T and returns a boolean. The boolean is true when the event of interest is detected and false otherwise. The state variable captures all the data required by the computations performed by the trigger. 
- Each trigger has a priority value associated with it. This enables the framework to determine which handler to execute first when there are multiple triggers.
- Priorities are chosen to have integer values. However, they are internally represented as a float. The reason for this is explained below.
- The RunEventMonitor function checks for all trigger activiations. 
- Handler functions associated with activated triggers are added to the activeEventHandlers queue if they are not on the queue already. 
- When a new handler is added to the queue, the handlers in the active queue are sorted according to their priority order. 
- The first handler in front of the queue after the sorting is chosen to be executed. This framework only executes one handler at a time.
- If adding a new handler to the queue causing an already executing handler to lose priority, then that old handler is terminated.
- The RunEventHandlers function executes the handler with the highest priority.
- Before starting a queued handler, it's trigger function is evaluated to ensure the triggering event is still active. If the triggering event is not active anymore (because it was resolved by a previous handler or someother reason), the queued handler can be discarded and the next handler in line can be executed.
- Once a handler starts executing, it's priority is raised by 0.5. This is to ensure that the framework will continue executing the current handler in case another equal priority handler is added to the queue of activehandlers. Priority of the handlers are decreased by 0.5 after termination (restoring it back to the user defined priority value).
- Terminated handlers are dequeued.
- Any handler may spawn other handlers (children). These children handlers are also added to the front of the queue of handlers and executed upon termination of the parent handler.



