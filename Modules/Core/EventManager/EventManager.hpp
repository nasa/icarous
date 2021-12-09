/**
 * @file EventManager.hpp
 * @brief 
 * 
 */

#ifndef EVENT_ACTION_MANAGER_HPP
#define EVENT_ACTION_MANAGER_HPP

#include <list>
#include <functional>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <climits>
#include <memory>
#include <EventHandler.hpp>

/**
 * @brief A heap+set data structure to maintain
 * priority queue while also enabling
 * fast set membership tests
 */
template <class T,class Compare>
class heapset{

  private:
    std::priority_queue<T,std::vector<T>,Compare> priq;
    std::set<T> priset;

  public:

    heapset(Compare comp):priq(comp){};

    /**
     * @brief add item to container 
     * 
     * @param value 
     */
    void push(T& value){
        priq.push(value);
        priset.insert(value);
    }

    /**
     * @brief remove item from the top of the container
     * 
     */
    void pop(){
        const T& val = priq.top();
        priq.pop();
        priset.erase(val);
    }

    /**
     * @brief check if the container is empty 
     * 
     * @return bool 
     */
    bool empty(){
        return priq.empty();
    }

    /**
     * @brief return element at top 
     * 
     * @return const T& 
     */
    const T& top(){
        return priq.top();
    }

    /**
     * @brief Check if container contains the given element 
     * 
     * @param val 
     */
    bool contains(T& val){
        if(priset.find(val) == priset.end()){
            return false;
        }else{
            return true;
        }
    }

    /**
     * @brief clear the container 
     */
    void clear(){
        while(!priq.empty()){
            priq.pop();
        }
        priset.clear();
    }
};

/**
 * @brief Event Management class 
 * 
 * @tparam T 
 */
template <class T>
class EventManagement{

  public:
     EventManagement():activeEventHandlers(handlerComp){};
     /**
      * @brief Add a trigger and attach a corresponding event handler
      * 
      * @param eventName name of event
      * @param priority priorit associated with event
      * @param monitorFunc trigger function
      * @param eventHandler handler to be fired when trigger is activated
      */
     void AddEventHandler(std::string eventName,int priority,std::function<bool(T*)> monitorFunc, std::shared_ptr<EventHandler<T>> eventHandler=nullptr);

     /**
      * @brief Check all the trigger functions fo active triggers 
      */
     void RunEventMonitors(T*);

     /**
      * @brief Execute active handlers 
      */
     void RunEventHandlers(T*);

     /**
      * @brief Run event management 
      */
     void Run(T*);

     /**
      * @brief Reset all triggers and handlers 
      * 
      */
     void Reset();
     
  private:

     /// List of monitor functions
     HandlerComp<T> handlerComp;
     std::map<std::string,std::function<bool(T*)>> events;
     std::map<std::string,std::shared_ptr<EventHandler<T>>> handlers;
     heapset<std::shared_ptr<EventHandler<T>>,HandlerComp<T>> activeEventHandlers;
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

template<class T>
void EventManagement<T>::Reset(){
    events.clear();
    handlers.clear();
    /// clear all handlers from priority queue
    activeEventHandlers.clear();
}

template <class T>
void EventManagement<T>::RunEventMonitors(T* state){
    for(auto &elem: events){
        /// Run the event monitor
        bool val = elem.second(state);

        /// If the event is true
        if(val){
            bool avail = false;
            /// Check if the event handler for this event is currently being executed
            if(activeEventHandlers.contains(handlers[elem.first])){
                avail = true;
            }
            
            /// If the event handler is not part of active handlers, add to active handlers
            if(!avail){
                if(handlers[elem.first] != nullptr){
                    handlers[elem.first]->eventName = elem.first;
                    handlers[elem.first]->execState = EventHandler<T>::NOOP;
                    if(!activeEventHandlers.empty()){
                        /// Get currently executing handler
                        auto currHandler = activeEventHandlers.top();
                        activeEventHandlers.push(handlers[elem.first]);
                        /// If the newly added handler takes more priority, stop the previous handler
                        if(currHandler != activeEventHandlers.top()){
                            currHandler->execState = EventHandler<T>::DONE;
                        }
                    }else{
                        activeEventHandlers.push(handlers[elem.first]);
                    }
                    
                }
            }
        }
    }
}

template<class T>
void EventManagement<T>::RunEventHandlers(T* state){
    if(activeEventHandlers.empty()){
        return;
    }
    auto handler = activeEventHandlers.top();
    bool val = false;
    if(handler->execState == EventHandler<T>::NOOP && handler->eventName != ""){
        /// If this handler is just starting, make sure the trigger is still true.
        /// If trigger is false, remove handler
        if(events[handler->eventName](state)){
           handler->execState = EventHandler<T>::INITIALIZE;

           /// Elevate priority to ensure this handler continues to execute
           /// on the next cycle in case there is another equal priority handler
           handler->priority = handler->defaultPriority + 0.5;
           val = handler->RunEvent(state);
        }else{
           handler->priority = handler->defaultPriority;
           activeEventHandlers.pop();
        }
    }else{
        val = handler->RunEvent(state);
    }

    /// Upon termination, restore priority
    if (val) {
        handler->priority = handler->defaultPriority;
        activeEventHandlers.pop();
    }

    /// Add any children spawned by the current handler to the queue
    while (handler->children.size() > 0){
        activeEventHandlers.push(handler->children.back());
        handler->children.pop_back();
    }
}

template<class T>
void EventManagement<T>::Run(T* state){
    RunEventMonitors(state);
    RunEventHandlers(state);
}

#endif