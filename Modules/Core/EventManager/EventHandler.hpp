/**
 * @file EventHandler.hpp
 * @brief Definition of event handler base class 
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>

template <class T>
class EventHandler{

public:
    /**
     * @enum execState_e  
     * @brief enumerations for the execution state
     */
    typedef enum{
       NOOP,INITIALIZE,EXECUTE,TERMINATE,DONE
    }execState_e;

    /**
     * @brief enumerations for the return state
     */
    typedef enum{
      SUCCESS, RESET, INPROGRESS, SHUTDOWN
    }retVal_e;

    EventHandler(){execState = NOOP;};
    /**
     * @brief perform all initialization activities for a specific task 
     * 
     * @param state 
     * @return retVal_e  
     */
    virtual retVal_e Initialize(T* state){return SUCCESS;};

    /**
     * @brief perform the core activity
     * 
     * @param state 
     * @return retVal_e 
     */
    virtual retVal_e Execute(T* state){return SUCCESS;};

    /**
     * @brief perform termination activities 
     * 
     * @param state 
     * @return retVal_e 
     */
    virtual retVal_e Terminate(T* state){return SUCCESS;};

    /**
     * @brief Run this event. 
     * 
     * @param state 
     * @return true 
     * @return false 
     */
    bool RunEvent(T* state);

    /**
     * @brief Spawn another handler 
     * 
     * @param hdl pointer to the other handler
     * @param eventName 
     * @param priorityNew 
     */
    void ExecuteHandler(std::shared_ptr<EventHandler<T>> hdl,std::string eventName,float priorityNew=0);

    std::string eventName; ///< event name
    execState_e execState; ///< current execution state for this handler
    std::list<std::shared_ptr<EventHandler<T>>> children; ///< list of child handlers
    float priority; ///< current execution priority
    float defaultPriority; ///< user defined priority for this handler
};

template<class T>
struct HandlerComp{
    bool operator()(const std::shared_ptr<EventHandler<T>>& A,const std::shared_ptr<EventHandler<T>>& B){
        return A->priority < B->priority;
    }
};

template <class T>
void EventHandler<T>::ExecuteHandler(std::shared_ptr<EventHandler<T>> hdl,std::string eventName,float priorityNew){
    if (priorityNew > 0){
        hdl->priority = priorityNew;
    }else{
        hdl->priority = priority;
    }
    hdl->execState = INITIALIZE;
    hdl->eventName = eventName;
    children.push_back(hdl);
}

template <class T>
bool EventHandler<T>::RunEvent(T* state){
    switch (execState) {
        case INITIALIZE: {
            retVal_e val = Initialize(state);
            if ( val == SUCCESS) {
                execState = EXECUTE;
            }else if(val == SHUTDOWN){
                execState = DONE;
            }
            break;
        }

        case EXECUTE: {
            retVal_e val = Execute(state);
            if (val == SUCCESS) {
                execState = TERMINATE;
            }else if(val == RESET){
                execState = INITIALIZE;
            }else if(val == SHUTDOWN){
                execState = DONE;
            }
            break;
        }

        case TERMINATE: {
            retVal_e val = Terminate(state);
            if(val == SUCCESS){
                execState = DONE;
            }else if(val == RESET){
                execState = INITIALIZE;
            }
            break;
        }

        default:
           break;
    }

    if(execState == DONE){
       execState = INITIALIZE;
       return true;
    }else{
       return false;
    }
}

#endif