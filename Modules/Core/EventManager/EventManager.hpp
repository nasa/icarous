#ifndef EVENT_ACTION_MANAGER_HPP
#define EVENT_ACTION_MANAGER_HPP

#include <list>
#include <functional>
#include <map>
#include <string>
#include <climits>
#include <memory>
#include <EventHandler.hpp>

template <class T>
class EventManagement{

  public:
     EventManagement(){};
     void AddEventHandler(std::string eventName,int priority,std::function<bool(T*)> monitorFunc, std::shared_ptr<EventHandler<T>> eventHandler=nullptr);
     void RunEventMonitors(T*);
     void RunEventHandlers(T*);
     void Run(T*);
     
  private:

     // List of monitor functions
     std::map<std::string,std::function<bool(T*)>> events;
     std::map<std::string,std::shared_ptr<EventHandler<T>>> handlers;
     std::list<std::shared_ptr<EventHandler<T>>>activeEventHandlers;
};

template <class T>
void EventManagement<T>::AddEventHandler(std::string eventName,int priority,std::function<bool(T*)> monitorFunc,std::shared_ptr<EventHandler<T>> eventHandler){
    events[eventName] = monitorFunc;
    if(eventHandler != nullptr){
        eventHandler->priority = priority;
        eventHandler->defaultPriority = priority;
        handlers[eventName] = eventHandler;
    }
}

template <class T>
void EventManagement<T>::RunEventMonitors(T* state){
    for(auto &elem: events){
        // Run the event monitor
        bool val = elem.second(state);

        // If the event is true
        if(val){
            bool avail = false;
            // Check if the event handler for this event is currently being executed
            for(auto &activehdl: activeEventHandlers){
                EventHandler<T>* hdl = handlers[elem.first].get();
                if (hdl != nullptr){
                    if (activehdl.get() == hdl) {
                        avail = true;
                        break;
                    }
                }
            }
            // If the event handler is not part of active handlers, add to active handlers
            if(!avail){
                if(handlers[elem.first] != nullptr){
                    handlers[elem.first]->eventName = elem.first;
                    handlers[elem.first]->execState = EventHandler<T>::NOOP;
                    activeEventHandlers.push_back(handlers[elem.first]);
                    auto cmp = [] (std::shared_ptr<EventHandler<T>> h1,std::shared_ptr<EventHandler<T>> h2) {
                        return h1->priority >= h2->priority;
                    };
                    auto currHandler = activeEventHandlers.front();
                    activeEventHandlers.sort(cmp);
                    if(currHandler != activeEventHandlers.front()){
                        currHandler->execState = EventHandler<T>::DONE;
                    }
                }
            }
        }
    }
}

template<class T>
void EventManagement<T>::RunEventHandlers(T* state){
    for (auto &handler : activeEventHandlers) {
        bool val = true;
        // If this handler is just starting, 
        if(handler->execState == EventHandler<T>::NOOP && handler->eventName != ""){
            // make sure the trigger is still true
            // if trigger is false, remove handler
            if(events[handler->eventName](state)){
               handler->execState = EventHandler<T>::INITIALIZE;
               handler->priority = handler->defaultPriority + 0.5;
               val = handler->RunEvent(state);
            }
        }else{
            val = handler->RunEvent(state);
        }
        if (val) {
            handler->priority = handler->defaultPriority;
            activeEventHandlers.pop_front();
        }

        while (handler->children.size() > 0){
            activeEventHandlers.push_front(handler->children.back());
            handler->children.pop_back();
        }
        break;
    }
}

template<class T>
void EventManagement<T>::Run(T* state){
    RunEventMonitors(state);
    RunEventHandlers(state);
}

#endif