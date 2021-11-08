#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>

template <class T>
class EventHandler{

public:
    typedef enum{
       NOOP,INITIALIZE,EXECUTE,TERMINATE,DONE
    }execState_e;

    typedef enum{
      SUCCESS, RESET, INPROGRESS, SHUTDOWN
    }retVal_e;

    EventHandler(){execState = NOOP;};
    virtual retVal_e Initialize(T* state){return SUCCESS;};
    virtual retVal_e Execute(T* state){return SUCCESS;};
    virtual retVal_e Terminate(T* state){return SUCCESS;};
    bool RunEvent(T* state);
    void ExecuteHandler(std::shared_ptr<EventHandler<T>> hdl,std::string eventName,float priorityNew=0);
    std::string eventName;
    execState_e execState;
    std::list<std::shared_ptr<EventHandler<T>>> children;
    float priority;
    float defaultPriority;
    unsigned int id;
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