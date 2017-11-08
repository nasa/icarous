//
// Created by Swee Balachandran on 11/6/17.
//

#ifndef CFSADAPTER_HH
#define CFSADAPTER_HH

#include "InterfaceAdapter.hh"
#include "State.hh"
#include "Command.hh"
#include "cfs-data-format.hh"
#include "ThreadSemaphore.hh"
#include "ThreadMutex.hh"

#include <queue>

namespace PLEXIL {

    class CfsAdapter : public InterfaceAdapter
    {
    public:
        CfsAdapter (AdapterExecInterface&, pugi::xml_node const);

        bool initialize();
        bool start();
        bool stop();
        bool reset();
        bool shutdown();

        void lookupNow(State const &state, StateCacheEntry &cacheEntry);
        void subscribe(const State& state);
        void unsubscribe(const State& state);

        void executeCommand(Command *cmd);

        void invokeAbort(Command *cmd);

        void HandleReturnValue(PlexilCommandMsg msg);

        int GetCmdQueueMsg(PlexilCommandMsg& msg);
        int GetLookUpQueueMsg(PlexilCommandMsg& msg);

    private:
        std::queue<PlexilCommandMsg> cmdQueue;
        std::queue<PlexilCommandMsg> lookupQueue;

        //* @brief Semaphore for return values from LookupNow
        ThreadSemaphore m_lookupSem;

        /**
         * @brief Mutex used to hold the processing of incoming return values while commands
         * are being sent and recorded.
         */
        ThreadMutex m_cmdMutex;

        //* @brief Place to store result of current pending LookupNow request
        Value m_pendingLookupResult;

        //* @brief Place to store state of pending LookupNow
        State m_pendingLookupState;

        //* @brief Serial # of current pending LookupNow request, or 0
        uint32_t m_pendingLookupSerial;


        //* brief Cache of command serials and their corresponding ack and return value variables
        typedef std::map<uint32_t, Command *> PendingCommandsMap;

        //* @brief Cache of ack and return value variables for commands we sent
        PendingCommandsMap m_pendingCommands;

        //* @brief Serial # of current pending command
        uint32_t m_pendingCommandSerial;


    };

    extern "C" {
    void initCfsAdapter();
    }

} // namespace PLEXIL


#endif //CFSADAPTER_H
