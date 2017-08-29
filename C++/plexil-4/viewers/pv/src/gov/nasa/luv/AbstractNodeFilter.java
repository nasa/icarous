/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

package gov.nasa.luv;

import java.util.Vector;

/** 
 * The AbstractNodeFilter class is an abstract class that provides methods for
 * filtering a Plexil Node. 
 */

public abstract class AbstractNodeFilter
{
      /** active filters */
      
      static Vector<AbstractNodeFilter> filters = new 
         Vector<AbstractNodeFilter>();

      /** filter event listeners */

      Vector<Listener> listeners = new Vector<Listener>();

      /** enabled state of this filter */
      
      private boolean enabled;
      
      /** 
       * Constructs an AbstractNodeFilter with the specified flag to enable
       * or disable filtering.
       *
       * @param enabled set enabled state of this filter
       */
      
      public AbstractNodeFilter(boolean enabled)
      {
         this.enabled = enabled;
         addFilter(this);
      }
      
      /** 
       * Specifies whether or not to enable or disable this filter. 
       *
       * @param enabled set enabled state of this filter
       */
      
      public void setEnabled(boolean enabled)
      {
         this.enabled = enabled;
         for (Listener listener: listeners)
            listener.filterChanged(this);
      }

      /** 
       * Adds the specified listener to set of active listeners.
       *
       * @param listener listener to add
       */
      
      public void addListener(Listener listener)
      {
         listeners.add(listener);
      }
      
      /** 
       * Removes the specified listener to set of active listeners.
       *
       * @param listener listener to remove
       */
      
      public void removeListener(Listener listener)
      {
         listeners.remove(listener);
      }
      
      /** 
       * Adds the specified filter to set of active filters.
       *
       * @param filter filter to add
       */
      
      public static void addFilter(AbstractNodeFilter filter)
      {
         filters.add(filter);
      }
      
      /** 
       * Removes the specified filter to set of active filters.
       *
       * @param filter filter to remove
       */
      
      public static void removeFilter(AbstractNodeFilter filter)
      {
         filters.remove(filter);
      }

      /** 
       * Tells whether the state of this filter is enabled.
       *
       * @return enabled state of this filter
       */
      
      public boolean isEnabled()
      {
         return enabled;
      }
      
      /** 
       * Determines if the specified Plexil Node is filtered by any of the filters
       * in the set of all filters.
       *
       * @param node node to test
       * 
       * @return filtered state of the Plexil Node
       */

      public static boolean isNodeFiltered(Node node)
      {
         for (AbstractNodeFilter filter: filters)
            if (filter.isEnabled() && filter.isFiltered(node))
               return true;

         return false;
      }
      
      /**
       * Tells whether the specified Plexil Node is filtered.
       * 
       * @param node node to test
       *
       * @return filtered state of the Plexil Node
       */
      
      public abstract boolean isFiltered(Node node);
      
      /** 
       * The Listener class is an abstract class that provides methods for 
       * handling filter change events.
       */

      public static abstract class Listener
      {
            /** 
             * Handles filter change event.
             *
             * @param filter enabled filter
             */
            
            public abstract void filterChanged(AbstractNodeFilter filter);
      }

      /** 
       * The Adapter class is an abstract class that provides methods for
       * for a filter change listener.
       */

      public static class Adapter extends Listener
      {
            /** {@inheritDoc} */

            @Override public void filterChanged(AbstractNodeFilter filter) {}
      }
}
