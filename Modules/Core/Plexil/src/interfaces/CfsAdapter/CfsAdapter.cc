//
// Created by Swee Balachandran on 11/7/17.
//

#include "CfsAdapter.hh"

#include "AdapterConfiguration.hh"
#include "AdapterFactory.hh"
#include "AdapterExecInterface.hh"
#include "Command.hh"
#include "Lookup.hh"
#include "StateCacheEntry.hh"
#include "Debug.hh"
#include "plan-utils.hh"
#include "ArrayImpl.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Error.hh>
#include <cstring>
#include "cfs-data-format.hh"

namespace PLEXIL {

    
    CfsAdapter::CfsAdapter(AdapterExecInterface& execInterface,
                                 pugi::xml_node const configXml) :
            InterfaceAdapter(execInterface, configXml),
            m_lookupSem(),
            m_cmdMutex(),
            m_pendingLookupResult(),
            m_pendingLookupState(),
            m_pendingLookupSerial(0),
            m_pendingCommandSerial(0)
    {
        debugMsg("CfsAdapter", " created.");
       
    }

    bool CfsAdapter::initialize()
    {
        // Register with AdapterExecInterface
        g_configuration->defaultRegisterAdapter(this);
        debugMsg("CfsAdapter:initialize", " succeeded");
        return true;
    }

    bool CfsAdapter::start()
    {
        debugMsg("CfsAdapter", " started.");
        return true;
    }

    bool CfsAdapter::stop()
    {
        debugMsg("CfsAdapter", " stopped.");
        return true;
    }

    bool CfsAdapter::reset()
    {
        debugMsg("CfsAdapter", " reset.");
        return true;
    }

    bool CfsAdapter::shutdown()
    {
        debugMsg("CfsAdapter", " shut down.");
        return true;
    }

    void CfsAdapter::lookupNow(State const &state, StateCacheEntry &entry)
    {
        const std::string& stateName = state.name();
        const std::vector<Value>& params = state.parameters();
        size_t nParams = params.size();
        debugMsg("CfsAdapter:lookupNow",
                 " for state " << stateName
                               << " with " << nParams << " parameters");

        //send lookup message
        m_pendingLookupResult.setUnknown();
        //decide to direct or publish lookup
        {
            ThreadMutexGuard g(m_cmdMutex);
            m_pendingLookupState = state;
            m_pendingLookupState.setName(stateName);
            m_pendingLookupSerial++;
            
            PlexilMsg lookupMsg;

            lookupMsg.mType =_LOOKUP_;

            memcpy(lookupMsg.name,stateName.c_str(),stateName.size()+1);

            lookupQueue.push(lookupMsg);
        }

        // Wait for results
        // N.B. shouldn't have to worry about signals causing wait to be interrupted -
        // ExecApplication blocks most of the common ones
        int errnum = m_lookupSem.wait();
        assertTrueMsg(errnum == 0,
                      "CfsAdapter::lookupNow: semaphore wait failed, result = " << errnum);

        entry.update(m_pendingLookupResult);
        m_execInterface.handleValueChange(state, m_pendingLookupResult);
        m_execInterface.notifyOfExternalEvent();
        
        // Clean up
        {
            ThreadMutexGuard g(m_cmdMutex);
            m_pendingLookupSerial = 0;
            m_pendingLookupState = State();
        }
        m_pendingLookupResult.setUnknown();
    }

    void CfsAdapter::subscribe(const State&)
    {
    }

    void CfsAdapter::unsubscribe(const State&)
    {
    }

    void CfsAdapter::executeCommand(Command * cmd)
    {
        std::string const &name = cmd->getName();
        debugMsg("CfsAdapter:executeCommand", " for \"" << name << "\"");
        std::vector<Value> const &args = cmd->getArgValues();
        if (!args.empty())
        debugMsg("CfsAdapter:executeCommand", " first parameter is \""
                << args.front()
                << "\"");

        //lock mutex to ensure no return values are processed while the command is being
        //sent and logged
        ThreadMutexGuard guard(m_cmdMutex);
        //decide to direct or publish command

        m_pendingCommandSerial++;
        PlexilMsg commandMsg;
        commandMsg.mType = _COMMAND_;
        commandMsg.id = (int)m_pendingCommandSerial;
        memcpy(commandMsg.name,name.c_str(),name.size()+1);
        memset(commandMsg.buffer,0,PLEXIL_MSG_BUFFER);
        int count = 0;
        char* b = commandMsg.buffer;

        size_t nParams = args.size();

        for (size_t i = 0; i < nParams; ++i) {
            Value val = args[i];
            if(val.isKnown()){
                b = val.serialize(b);
                count += val.serialSize();
                assertTrueMsg(count < PLEXIL_MSG_BUFFER,"Too many function arguments, increase PLEXIL_MSG_BUFFER");
            }
        }

        cmdQueue.push(commandMsg);

        m_pendingCommands[m_pendingCommandSerial] = cmd;
        // store ack
        m_execInterface.handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
        m_execInterface.notifyOfExternalEvent();
        debugMsg("CfsAdapter:executeCommand", " command \"" << name << "\" sent.");
    }

    void CfsAdapter::HandleReturnValue(PlexilMsg *msg) {

        //lock mutex to ensure all sending procedures are complete.
        ThreadMutexGuard guard(m_cmdMutex);
        if (msg->mType == _LOOKUP_RETURN_) {
            // LookupNow for which we are awaiting data
            debugMsg("CfsAdapter:handleReturnValuesSequence",
                     " processing value(s) for a pending LookupNow");
            m_pendingLookupResult = ParseReturnValue(msg);
            // *** TODO: check for error
            m_lookupSem.post();
            return;
        }

        PendingCommandsMap::iterator cit = m_pendingCommands.find(msg->id);
        if (cit != m_pendingCommands.end()) {
            Command *cmd = cit->second;
            assertTrue_1(cmd);

            if (!cmd->isActive()) {
                debugMsg("CfsAdapter:handleReturnValuesSequence",
                         " ignoring return value for inactive command");
                m_pendingCommands.erase(msg->id);
                return;
            }
            debugMsg("CfsAdapter:handleReturnValuesSequence",
                     " processing command return value for command " << cmd->getName());
            m_execInterface.handleCommandReturn(cmd, ParseReturnValue(msg));
            m_execInterface.notifyOfExternalEvent();
        }
        else {
            debugMsg("CfsAdapter:handleReturnValuesSequence",
                     " no lookup or command found for sequence");
        }

    }

    void CfsAdapter::invokeAbort(Command *cmd)
    {

    }

    int CfsAdapter::GetCmdQueueMsg(PlexilMsg *msg) {
        //lock mutex to ensure all writing procedures are complete.
        ThreadMutexGuard guard(m_cmdMutex);
        if (cmdQueue.size() == 0){
            return -1;
        }

        PlexilMsg queueMsg = cmdQueue.front();
        cmdQueue.pop();

        memcpy(msg,&queueMsg,sizeof(queueMsg));
        return cmdQueue.size();
    }

    int CfsAdapter::GetLookUpQueueMsg(PlexilMsg *msg) {
        //lock mutex to ensure all writing procedures are complete.
        ThreadMutexGuard guard(m_cmdMutex);
        if (lookupQueue.size() == 0){
            return -1;
        }

        PlexilMsg queueMsg = lookupQueue.front();
        lookupQueue.pop();

        memcpy(msg,&queueMsg,sizeof(queueMsg));

        return lookupQueue.size();
    }

    Value CfsAdapter::ParseReturnValue(PlexilMsg *msg){
        Value val;
        val.deserialize(msg->buffer);
        return val;
    }

    void CfsAdapter::HandleValueChange(State state, Value val){
        m_execInterface.handleValueChange(state, val);
        m_execInterface.notifyOfExternalEvent();
        return; 
    }

    extern "C" {
    void initCfsAdapter() {
        REGISTER_ADAPTER(CfsAdapter, "CfsAdapter");
    }
    }

} // namespace PLEXIL
