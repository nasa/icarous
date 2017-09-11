/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#include "StateCacheEntry.hh"

#include "ArrayImpl.hh"
#include "CachedValue.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExternalInterface.hh"
#include "Lookup.hh"
#include "State.hh"

#include <algorithm> // std::find

namespace PLEXIL
{
  StateCacheEntry::StateCacheEntry()
    : m_value(NULL),
      m_lowThreshold(NULL),
      m_highThreshold(NULL)
  {
  }

  // Copy constructor, used ONLY by StateCacheMap
  StateCacheEntry::StateCacheEntry(StateCacheEntry const &orig)
    : m_value(NULL),
      m_lowThreshold(NULL),
      m_highThreshold(NULL)
  {
  }

  StateCacheEntry::~StateCacheEntry()
  {
    delete m_value;
    delete m_lowThreshold;
    delete m_highThreshold;
  }

  ValueType const StateCacheEntry::valueType() const
  {
    if (m_value)
      return m_value->valueType();
    else
      return UNKNOWN_TYPE;
  }

  bool StateCacheEntry::isKnown() const
  {
    if (m_value)
      return m_value->isKnown();
    else
      return false;
  }

  void StateCacheEntry::registerLookup(State const &s, Lookup *l)
  {
    bool unsubscribed = m_lookups.empty();
    m_lookups.push_back(l);
    if (unsubscribed) {
      debugMsg("StateCacheEntry:registerLookup", ' ' << s << " subscribing to interface")
      g_interface->subscribe(s);
    }
    debugMsg("StateCacheEntry:registerLookup",
	     ' ' << s << " now has " << m_lookups.size() << " lookups");
    // Update if stale
    if ((!m_value) || m_value->getTimestamp() < g_interface->getCycleCount()) {
      debugMsg("StateCacheEntry:registerLookup", ' ' << s << " updating stale value")
      g_interface->lookupNow(s, *this);
    }
  }

  void StateCacheEntry::unregisterLookup(State const &s, Lookup *l)
  {
    debugMsg("StateCacheEntry:unregisterLookup", ' ' << s)

    if (m_lookups.empty())
      return; // can't possibly be registered

    // Somewhat likely to remove last item first, so check for that special case.
    // TODO: analyze to see if this is true!
    if (l == m_lookups.back())
      m_lookups.pop_back();
    else {
      std::vector<Lookup *>::iterator it =
        std::find(m_lookups.begin(), m_lookups.end(), l);
      if (it != m_lookups.end())
        m_lookups.erase(it);
      else {
	debugMsg("StateCacheEntry:unregisterLookup", ' ' << s << " lookup not found")
        return;
      }
    }

    if (m_lookups.empty()) {
      debugMsg("StateCacheEntry:unregisterLookup",
	       ' ' << s << " no lookups remaining, unsubscribing");
      g_interface->unsubscribe(s);
      if (m_lowThreshold || m_highThreshold) {
	delete m_lowThreshold;
	delete m_highThreshold;
	m_lowThreshold = m_highThreshold = NULL;
      }
    }
    else if (m_lowThreshold || m_highThreshold) {
      // Check whether thresholds should be updated
      debugMsg("StateCacheEntry:unregisterLookup",
	       ' ' << s << " updating thresholds from remaining " << m_lookups.size() << " lookups");
      updateThresholds(s);
    }
  }

  bool StateCacheEntry::integerUpdateThresholds(State const &s)
  {
    bool hasThresholds = false;
    Integer ihi, ilo;
    Integer newihi, newilo;
    for (std::vector<Lookup *>::const_iterator it = m_lookups.begin();
         it != m_lookups.end();
         ++it) {
      if ((*it)->getThresholds(newihi, newilo)) {
        if (!hasThresholds) {
          hasThresholds = true;
          ilo = newilo;
          ihi = newihi;
        }
        else {
          if (newilo > ilo)
            ilo = newilo;
          if (newihi < ihi)
            ihi = newihi;
        }
      }
    }
    if (hasThresholds) {
      debugMsg("StateCacheEntry:updateThresholds",
               ' ' << s << " resetting thresholds " << ilo << ", " << ihi);
      if (!m_lowThreshold) {
        m_lowThreshold = CachedValueFactory(INTEGER_TYPE);
        m_highThreshold = CachedValueFactory(INTEGER_TYPE);
      }
      unsigned int timestamp = g_interface->getCycleCount();
      m_lowThreshold->update(timestamp, ilo);
      m_highThreshold->update(timestamp, ihi);
      g_interface->setThresholds(s, ihi, ilo);
    }
    return hasThresholds;
  }

  bool StateCacheEntry::realUpdateThresholds(State const &s)
  {
    bool hasThresholds = false;
    Real rhi, rlo;
    Real newrhi, newrlo;
    for (std::vector<Lookup *>::const_iterator it = m_lookups.begin();
	 it != m_lookups.end();
	 ++it) {
      if ((*it)->getThresholds(newrhi, newrlo)) {
	if (!hasThresholds) {
	  hasThresholds = true;
	  rlo = newrlo;
	  rhi = newrhi;
	}
	else {
	  if (newrlo > rlo)
	    rlo = newrlo;
	  if (newrhi < rhi)
	    rhi = newrhi;
	}
      }
    }
    if (hasThresholds) {
      debugMsg("StateCacheEntry:updateThresholds",
	       ' ' << s << " setting thresholds " << rlo << ", " << rhi);
      if (!m_lowThreshold) {
	m_lowThreshold = CachedValueFactory(REAL_TYPE);
	m_highThreshold = CachedValueFactory(REAL_TYPE);
      }
      unsigned int timestamp = g_interface->getCycleCount();
      m_lowThreshold->update(timestamp, rlo);
      m_highThreshold->update(timestamp, rhi);
      g_interface->setThresholds(s, rhi, rlo);
    }
    return hasThresholds;
  }

  void StateCacheEntry::updateThresholds(State const &s)
  {
    // Survey lookups to determine if the thresholds
    // need to be established, changed, or deleted.
    ValueType vtype = m_value->valueType();
    bool hasThresholds = false;

    switch (vtype) {
    case INTEGER_TYPE:
      hasThresholds = integerUpdateThresholds(s);
      break;

      // FIXME: support non-Real date/duration types
    case DATE_TYPE:
    case DURATION_TYPE:

    case REAL_TYPE: {
      hasThresholds = realUpdateThresholds(s);
      break;
    }

    default:
      // this is a plan error
      warn("LookupOnChange: lookup value of type " << valueTypeName(vtype)
	   << " does not allow a tolerance");
      return;
    }
    if (!hasThresholds) {
      debugMsg("StateCacheEntry:updateThresholds",
	       ' ' << s << " no change lookups remaining, clearing thresholds");
      delete m_lowThreshold;
      delete m_highThreshold;
      m_lowThreshold = m_highThreshold = NULL;
    }
  }

  CachedValue const *StateCacheEntry::cachedValue() const
  {
    return m_value;
  }

  bool StateCacheEntry::ensureCachedValue(ValueType v)
  {
    if (m_value) {
      // Check that requested type is consistent with existing
      ValueType ct = m_value->valueType();
      if (ct == v) // usual case, we hope
        return true;
      if (v == UNKNOWN_TYPE) // caller doesn't know or care
        return true;
      if (ct == UNKNOWN_TYPE) {
        // Replace placeholder with correct type
        delete m_value;
        m_value = CachedValueFactory(v);
        return true;
      }
      if (v == INTEGER_TYPE && isNumericType(ct)) // can store an integer in any numeric type
        return true;
      // FIXME implement a real time type
      if (v == REAL_TYPE && (ct == DATE_TYPE || ct == DURATION_TYPE)) // date, duration are real
        return true;

      // Type mismatch
      // FIXME this is likely a plan or interface coding error, handle more gracefully
      debugMsg("StateCacheEntry:update",
               " requested type " << valueTypeName(v)
               << " but existing value is type " << valueTypeName(ct));
      return false;
    }
    else {
      m_value = CachedValueFactory(v);
      return true;
    }
  }

  void StateCacheEntry::setUnknown()
  {
    if (m_value) {
      if (m_value->setUnknown(g_interface->getCycleCount()))
        notify();
    }
  }

  void StateCacheEntry::update(Boolean const &val)
  {
    if (!ensureCachedValue(BOOLEAN_TYPE))
      return;
    if (m_value->update(g_interface->getCycleCount(), val))
      notify();
  }

  void StateCacheEntry::update(Integer const &val)
  {
    if (!ensureCachedValue(INTEGER_TYPE))
      return;
    if (m_value->update(g_interface->getCycleCount(), val))
      notify();
  }

  void StateCacheEntry::update(Real const &val)
  {
    if (!ensureCachedValue(REAL_TYPE))
      return;
    if (m_value->update(g_interface->getCycleCount(), val))
      notify();
  }

  void StateCacheEntry::update(String const &val)
  {
    if (!ensureCachedValue(STRING_TYPE))
      return;
    if (m_value->update(g_interface->getCycleCount(), val))
      notify();
  }

  void StateCacheEntry::update(Value const &val)
  {
    if (!ensureCachedValue(val.valueType()))
      return;
    if (m_value->update(g_interface->getCycleCount(), val))
      notify();
  }

  void StateCacheEntry::updatePtr(String const *valPtr)
  {
    if (!ensureCachedValue(STRING_TYPE))
      return;
    if (m_value->updatePtr(g_interface->getCycleCount(), valPtr))
      notify();
  }

  void StateCacheEntry::updatePtr(BooleanArray const *valPtr)
  {
    if (!ensureCachedValue(BOOLEAN_ARRAY_TYPE))
      return;
    if (m_value->updatePtr(g_interface->getCycleCount(), valPtr))
      notify();
  }

  void StateCacheEntry::updatePtr(IntegerArray const *valPtr)
  {
    if (!ensureCachedValue(INTEGER_ARRAY_TYPE))
      return;
    if (m_value->updatePtr(g_interface->getCycleCount(), valPtr))
      notify();
  }

  void StateCacheEntry::updatePtr(RealArray const *valPtr)
  {
    if (!ensureCachedValue(REAL_ARRAY_TYPE))
      return;
    if (m_value->updatePtr(g_interface->getCycleCount(), valPtr))
      notify();
  }

  void StateCacheEntry::updatePtr(StringArray const *valPtr)
  {
    if (!ensureCachedValue(STRING_ARRAY_TYPE))
      return;
    if (m_value->updatePtr(g_interface->getCycleCount(), valPtr))
      notify();
  }

  void StateCacheEntry::notify() const
  {
    for (std::vector<Lookup *>::const_iterator it = m_lookups.begin();
         it != m_lookups.end();
         ++it)
      (*it)->valueChanged();
  }

}
