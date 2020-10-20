#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

template <class T>
class EventHandler{

public:
    typedef enum{
       NOOP,INITIALIZE,EXECUTE,TERMINATE,DONE
    }execState_e;

    typedef enum{
      SUCCESS, RESET, INPROGRESS
    }retVal_e;

    EventHandler(){execState = NOOP;};
    virtual retVal_e Initialize(T* state){return SUCCESS;};
    virtual retVal_e Execute(T* state){return SUCCESS;};
    virtual retVal_e Terminate(T* state){return SUCCESS;};
    bool RunEvent(T* state);
    void ExecuteHandler(EventHandler<T>* hdl);
    std::string eventName;
    execState_e execState;
    std::list<EventHandler<T>*> children;
    unsigned int priority;
};

template <class T>
void EventHandler<T>::ExecuteHandler(EventHandler<T>* hdl){
    hdl->priority = priority;
    hdl->execState = INITIALIZE;
    children.push_back(hdl);
}

template <class T>
bool EventHandler<T>::RunEvent(T* state){
    switch (execState) {
        case INITIALIZE: {
            retVal_e val = Initialize(state);
            if ( val == SUCCESS) {
                execState = EXECUTE;
            }
            break;
        }

        case EXECUTE: {
            retVal_e val = Execute(state);
            if (val == SUCCESS) {
                execState = TERMINATE;
            }else if(val == RESET){
                execState = INITIALIZE;
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