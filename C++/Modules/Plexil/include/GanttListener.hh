/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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
* By Madan, Isaac A.
* Updated by Cao, Yichuan
*/

#ifndef PLEXIL_GANTT_LISTENER_HH
#define PLEXIL_GANTT_LISTENER_HH

#include "ExecListener.hh"

#include <map>
#include <sstream>

namespace PLEXIL
{
   // Provides output from execution useful for debugging a Plexil plan.
   class GanttListener : public ExecListener
   {
   public:
      GanttListener();
      GanttListener(pugi::xml_node const xml);
      virtual ~GanttListener();
      // Capture and report about useful node state transitions.
      virtual void implementNotifyNodeTransition (NodeState prevState,
                                                  Node *node) const;
   private:
      struct NodeObj {
         double start;
         double end;
         double duration;
         std::string name;
         std::string type;
         std::string val;
         std::string parent;
         int id;
         std::string localvariables;
         std::string children;
         std::vector<std::string> localvarsvector;
         NodeObj (double start_val, 
                  double end_val,
                  double duration_val,
                  const std::string& id,  
                  const std::string& type_id, 
                  const std::string& val_str,
                  const std::string& parent_str, 
                  int id_val, 
                  const std::string& loc_var,
                  const std::string& child_str, 
                  std::vector<std::string>& loc_var_vec
                  )
                  :start(start_val),
                  end(end_val),
                  duration(duration_val), 
                  name(id),
                  type(type_id),
                  val(val_str),
                  parent(parent_str),
                  id(id_val),
                  localvariables(loc_var),
                  children(child_str),
                  localvarsvector(loc_var_vec)
                  { }
      };

      // Disallow copy, and assignment
      GanttListener(const GanttListener&);
      GanttListener& operator=(const GanttListener&);

      void initializeMembers();
      void setGanttDir();
      void setCurrDir();
      pid_t setPID();
      void setUniqueFileName();

      void createHTMLFile();
      void deliverJSONAsFile();
      void deliverPartialJSON();

      NodeObj createNodeObj(Node *nodeId);
      void getFinalLocalVar(Node *nodeId);

      void processLocalVar(const std::vector<std::string>& prevLocalVarsVector, 
                           const std::vector<std::string>& thisLocalVarsVectorValues, 
                           const std::vector<std::string>& thisLocalVarsVectorKeys);

      void processTempValsForNode(Node *nodeId);
      void produceSingleJSONObj();
      void createJSONStream();
      void generateTempOutputFiles();
      void generateFinalOutputFiles();
      void findNode(Node *nodeId);
      void processOutputData(Node *nodeId);

      int m_uniqueFileName;
      int m_pid;
      std::string m_HTMLFilePath;
      std::string m_HTMLFilePathForJSON;
      bool m_outputFinalJSON; // this prevent writing to the JSON file unnecessarily
      std::vector<NodeObj> m_nodes;
      std::ostringstream m_fullTemplate; // JSON object stream
      int m_index; // index of node vector
      bool m_outputHTML; // control if output HTML, keep cost down
      std::string m_plexilGanttDirectory;
      std::string m_currentWorkingDir;
      double m_startTime;
      std::map<Node *, int> m_stateMap, m_counterMap;
      std::string m_parent;
      bool m_planFailureState; // when node fail, parial JSON generation should be suppressed
      bool m_continueOutputingData; // when I/O error occurs, write no file
      int m_actualId;
   };
}

extern "C" 
{
   void initGanttListener();
}


#endif // PLEXIL_GANTT_LISTENER_HH
