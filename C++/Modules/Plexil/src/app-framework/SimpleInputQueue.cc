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

#include <cstddef> // for NULL

#include "SimpleInputQueue.hh"

#include "Error.hh"
#include "QueueEntry.hh"

namespace PLEXIL
{
  SimpleInputQueue::SimpleInputQueue()
    : InputQueue(),
      m_queueGet(NULL),
      m_queuePut(NULL),
      m_freeList(NULL)
  {
  }

  SimpleInputQueue::~SimpleInputQueue()
  {
    m_queuePut = NULL;
    while (m_queueGet) {
      QueueEntry *temp = m_queueGet;
      m_queueGet = temp->next;
      delete temp;
    }
    while (m_freeList) {
      QueueEntry *temp = m_freeList;
      m_freeList = temp->next;
      delete temp;
    }
  }

  bool SimpleInputQueue::isEmpty() const
  {
    return m_queueGet == NULL;
  }

  QueueEntry *SimpleInputQueue::allocate()
  {
    QueueEntry* result = m_freeList;
    if (result)
      m_freeList = result->next;
    else
      result = new QueueEntry;
    return result;
  }

  void SimpleInputQueue::release(QueueEntry *entry)
  {
    assertTrue_1(entry);
    entry->reset(); // ??
    entry->next = m_freeList;
    m_freeList = entry;
  }

  void SimpleInputQueue::put(QueueEntry *entry)
  {
    assertTrue_1(entry);
    entry->next = NULL;
    if (m_queuePut)
      m_queuePut->next = entry;
    m_queuePut = entry;
    if (!m_queueGet)
      m_queueGet = entry;
  }

  QueueEntry *SimpleInputQueue::get()
  {
    if (!m_queueGet)
      return NULL; // empty
    QueueEntry *result = m_queueGet;
    m_queueGet = result->next;
    if (!m_queueGet) { // queue now empty
      assertTrue_1(m_queuePut == result); // consistency check
      m_queueGet = m_queuePut = NULL;
    }
    return result;
  }

  void SimpleInputQueue::flush()
  {
    QueueEntry *temp;
    while ((temp = m_queueGet)) {
      m_queueGet = temp->next;
      release(temp);
    }
    m_queuePut = NULL;
  }

} // namespace PLEXIL
