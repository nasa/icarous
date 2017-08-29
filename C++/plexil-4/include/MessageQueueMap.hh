/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universities Space Research Association nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * MessageQueueMap.hh
 *
 *  Created on: Feb 19, 2010
 *      Author: jhogins
 */

#ifndef MESSAGEQUEUEMAP_H_
#define MESSAGEQUEUEMAP_H_

#include "ThreadMutex.hh"
#include "ExecListener.hh"
#include "AdapterExecInterface.hh"
#include <map>

namespace PLEXIL 
{
  class MessageQueueMap
  {
  public:

    MessageQueueMap(AdapterExecInterface& execInterface, bool allowDuplicateMessages = true);

    virtual ~MessageQueueMap();

    /**
     * @brief Adds the given recipient to the queue to receive the given message.
     * If a recipient already exists for this message, messages will be handed out in the order
     * of the adding of the recipients.
     * @param message The message the recipient is waiting for
     * @param cmd Pointer to the command requesting the message.
     */
    void addRecipient(const std::string &message, Command *cmd);

    /**
     * @brief Removes the given recipient waiting on the given message string associated with
     * the given acknowledgment.
     * @param message The message the recipient is waiting for
     * @param cmd Pointer to the command requesting the message.
     */
    void removeRecipient(const std::string &message, Command const *cmd);

    /**
     * @brief Removes all recipients waiting on the given message string.
     */
    void clearRecipientsForMessage(const std::string &message);

    /**
     * @brief Adds the given message to its queue. If there is a recipient
     * waiting for the message, it is sent immediately.
     * @param message The message string to be added
     */
    void addMessage(const std::string &message);

    /**
     * @brief Adds the given message with the given parameters to its queue.
     * If there is a recipient waiting for the message, it is sent immediately.
     * @param message The message string to be added
     * @param params The parameters that are to be sent with the message
     */
    void addMessage(const std::string& message, const Value& param);

    /**
     * @brief Sets the flag that determines whether or not incoming messages
     * with duplicate strings are queued. If true, all incoming messages are
     * put into the queue. Oldest instances of the message are distributed first.
     * If false, new messages with duplicate strings replace older ones; this
     * will remove all oldest duplicates from the queue immediately as well
     * as set the behavior for future messages.
     * @param flag If false, duplicates will be replaced with the newest
     * message. If true, duplicates are queued.
     */
    void setAllowDuplicateMessages(bool flag);

    /**
     * @brief Returns whether or not incoming messages
     * with duplicate strings are queued. If true, all incoming messages are
     * put into the queue. Oldest instances of the message are distributed first.
     * If false, new messages with duplicate strings replace older ones.
     * @return The boolean flag.
     */
    bool getAllowDuplicateMessages();

  private:

    //forward declarations
    typedef std::vector<Value> MessageQueue;

    //* @brief Data structure containing an acknowledgment expression and a destination expression, for identifying and storing recipients
    struct Recipient
    {
      Command *m_cmd;

      Recipient(Command *cmd)
      : m_cmd(cmd)
      {}

      Recipient(const Recipient& rec)
      : m_cmd(rec.m_cmd)
      {}

      ~Recipient() {}
    };

    typedef std::vector<Recipient> RecipientQueue;

    //* @brief Data queue structure associating a message string with a queue of recipients and a queue of messages. Only one queue should have items at a time. */
    struct PairingQueue
    {
      std::string m_name;
      bool m_allowDuplicateMessages;
      RecipientQueue m_recipientQueue;
      MessageQueue m_messageQueue;
      PairingQueue(const std::string &name, bool allowDuplicateMessages)
        : m_name(name),
          m_allowDuplicateMessages(allowDuplicateMessages),
          m_recipientQueue(),
          m_messageQueue() {}
      ~PairingQueue() {}
    };

    /**
     * @brief Private function that returns the queue for the given message. Creates a new queue
     * if one does not already exist.
     */
    PairingQueue* getQueue(const std::string& message);

    /**
     * @brief Resolves matches between messages and recipients. Should be called whenever updates occur to a queue.
     */
    void updateQueue(PairingQueue* queue);

    //* @brief Map holding all message queues
    std::map<std::string, PairingQueue*> m_map;

    //* @brief Semaphore for return values from LookupNow
    ThreadMutex m_mutex;

    //* @brief The interface
    AdapterExecInterface& m_execInterface;

    //* @brief If true, all messages are put into the queue. If false, messages with duplicate strings replace older ones.
    bool m_allowDuplicateMessages;
  };
}

#endif /* MESSAGEQUEUEMAP_H_ */
