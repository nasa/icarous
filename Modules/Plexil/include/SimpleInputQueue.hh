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
*/

#ifndef PLEXIL_SIMPLE_INPUT_QUEUE_HH
#define PLEXIL_SIMPLE_INPUT_QUEUE_HH

#include "InputQueue.hh"

namespace PLEXIL
{

  /**
   * @class SimpleInputQueue
   * @brief A simple implementation of the InputQueue API.
   */
  class SimpleInputQueue : public InputQueue
  {
  public:
    SimpleInputQueue();
    virtual ~SimpleInputQueue();

    // Simple query
    bool isEmpty() const;

    //
    // Reader side
    //

    // Get the head of the queue. If empty, returns NULL.
    virtual QueueEntry *get();

    // Flush the queue without examining it.
    virtual void flush();

    // Return an entry to the free list after use.
    virtual void release(QueueEntry *entry);

    //
    // Writer side
    //

    // Get an entry for insertion. Will allocate if none on the free list.
    virtual QueueEntry *allocate();

    // Insert an entry on the queue.
    virtual void put(QueueEntry *entry);

  private:
    // Disallow copy, assign
    SimpleInputQueue(SimpleInputQueue const &);
    SimpleInputQueue &operator=(SimpleInputQueue const &);

    QueueEntry *m_queueGet;
    QueueEntry *m_queuePut;
    QueueEntry *m_freeList;
  };

}

#endif // PLEXIL_SIMPLE_INPUT_QUEUE_HH
