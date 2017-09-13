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

#include "GanttListener.hh"

#include "AdapterExecInterface.hh"
#include "AdapterFactory.hh"
#include "Debug.hh"
#include "ExecListenerFactory.hh"
#include "Expression.hh"
#include "Node.hh"
#include "NodeVariableMap.hh"

#include <fstream>
#include <cmath>
#include <ctime>
#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#ifdef WINDOWS
   #include <direct.h>
   #define GetCurrentDir _getcwd
#else
   #include <unistd.h>
   #define GetCurrentDir getcwd
#endif

using std::cout;
using std::cin;
using std::ofstream;
using std::endl;
using std::string;
using std::vector;
using std::map;

namespace PLEXIL
{
   // For now, use the DebugMsg facilities (really intended for debugging the
   // *executive* and not plans) to display messages of interest.  Later, a more
   // structured approach including listener filters and a different user
   // interface may be in order.
   void GanttListener::initializeMembers()
   {
      m_outputFinalJSON = true;
      m_outputHTML = true;
      m_planFailureState = false;
      m_startTime = -1;
      m_actualId = 0;
      m_continueOutputingData = true;
      m_fullTemplate << "var rawPlanTokensFromFile=\n[\n";
      setCurrDir();
      setGanttDir();
      setUniqueFileName();
   }

   GanttListener::GanttListener() 
   { 
      initializeMembers();
   }

   GanttListener::GanttListener(pugi::xml_node const xml) : ExecListener(xml)
   {  
      initializeMembers(); 
   }

   GanttListener::~GanttListener() { }

   void GanttListener::setGanttDir()
   {
      /** get PLEXIL_HOME **/
      string pPath;
      const string ganttLocation = "/viewers/gantt/";
      
      pPath = getenv ("PLEXIL_HOME");
      if (pPath == "")
      {
         m_continueOutputingData = false;
         debugMsg("GanttViewer:printErrors", "PLEXIL_HOME is not defined");
      }
      /** get Viewer directory under PLEXIL_HOME **/
      m_plexilGanttDirectory = pPath + ganttLocation;
   }

   void GanttListener::setCurrDir()
   {
      char * buffer;
      if (!(buffer = getcwd(NULL, FILENAME_MAX)))
      {
         m_continueOutputingData = false;
         debugMsg("GanttViewer:printErrors", "Output path is not defined");
      }
      else
      {
         m_currentWorkingDir = buffer;
         free(buffer);
      }
   }

   pid_t GanttListener::setPID()
   {
      return m_pid = getpid();
   }
   
   void GanttListener::setUniqueFileName()
   {
      m_uniqueFileName = setPID();
   }

   /** generate the HTML file once a plan's execution started and 
   that connects to necessary Javascript and produced JSON **/
   void GanttListener::createHTMLFile()
   {
      std::ostringstream htmlFilePath, tokenFileName;
      htmlFilePath << m_currentWorkingDir << "/" << 
         "gantt_" << m_uniqueFileName << "_" << m_nodes[0].name << ".html";
      m_HTMLFilePath = htmlFilePath.str(); 
      tokenFileName << "json/" << m_uniqueFileName << "_" << m_nodes[0].name << ".js";
      const string myTokenFileName = tokenFileName.str(); 
      const string br = "\n ";

      ofstream myfile;
      myfile.open(m_HTMLFilePath.c_str());
      if (myfile.fail())
      {
         m_continueOutputingData = false;
         debugMsg("GanttViewer:printErrors", "Failed to create HTML file!");
         return;
      }
      else
      {
         myfile << 
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 " <<
            "Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">" << br <<
            "<html lang=\"en\"> " << br <<
            "<head> " << br <<
            "<meta http-equiv=\"Content-Type\" " <<
            "content=\"text/html; charset=utf-8\"> " << br <<
            "<title>" << m_nodes[0].name << " - " << "Gantt Temporal Plan Viewer</title> " << 
            br << "<meta name=\"author\" content=\"By Madan, Isaac " <<
            "A. (ARC-TI); originally authored by " <<
            "Swanson, Keith J. (ARC-TI)\"> " << br << br <<
            "<!-- jQuery is required --> " << br <<
            "<script src=\"" << m_plexilGanttDirectory << "jq/jquery-1.6.2.js\" " <<
            "type=\"text/javascript\"></script> " << br <<
            "<link type=\"text/css\" href=\"" << m_plexilGanttDirectory << 
            "jq/jquery-ui-1.8.15.custom.css\" " <<
            "rel=\"Stylesheet\" /> " << br <<
            "<script type=\"text/javascript\" src=\"" << m_plexilGanttDirectory <<
            "jq/jquery-ui-1.8.15.custom.min.js\"></script> " << br << br <<
            "<!-- Load data locally --> " << br <<
            "<script src=\"" << m_currentWorkingDir << "/" << myTokenFileName << 
            "\" type=\"text/javascript\"></script> " << br << br <<
            "<!-- Application code --> " << br <<      
            "<script src=\"" << m_plexilGanttDirectory <<
            "addons.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "getAndConvertTokens.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "showTokens.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "detailsBox.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "grid.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "sizing.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "main.js\" type=\"text/javascript\"></script> " << br <<
            "<script src=\"" << m_plexilGanttDirectory <<
            "shortcuts.js\" type=\"text/javascript\"></script> " << br << br <<
            "<!-- My styles --> " << br <<
            "<link rel=\"stylesheet\" href=\"" << m_plexilGanttDirectory << 
            "styles.css\" type=\"text/css\"> " << br <<
            "</head> \n <body> " << br << br <<
            "<!-- Layout --> " << br <<
            "<div id=\"footer\"></div> " << br <<
            "<div id=\"mod\"></div> " << br <<
            "<div id=\"gantt\"></div> " << br <<
            "</body> " << br <<
            "</html>"
            << std::flush;
         myfile.close();
      }
      
      m_HTMLFilePathForJSON = "\n \n var myHTMLFilePathString =\"" + m_HTMLFilePath + "\";";
      debugMsg("GanttViewer:printProgress", "HTML file written to "+ m_HTMLFilePath);
   }

   /** generate the JSON tokens file at the end of a plan's execution
   so that it can be parsed by Javascript in the Viewer **/
   void GanttListener::deliverJSONAsFile()
   {
      const string myCloser = "];";
      const string json_folder_path = m_currentWorkingDir + "/" + "json";
      std::ostringstream ss;
      string outputFileName;
      if (m_outputFinalJSON)
      {
         if (access(json_folder_path.c_str(), 0) != 0)
         {
            mkdir(json_folder_path.c_str(), S_IRWXG | S_IRGRP | 
               S_IROTH | S_IRUSR | S_IRWXU);
         }
         ss << m_currentWorkingDir << "/" <<
            "json/" << m_uniqueFileName << "_" << m_nodes[0].name << ".js";
         ofstream myfile;
         outputFileName = ss.str(); 
         myfile.open(outputFileName.c_str());
         if (myfile.fail())
         {
            m_continueOutputingData = false;
            debugMsg("GanttViewer:printErrors", "Failed to create JSON file!");
            return;
         }
         else
         {
            myfile << m_fullTemplate.str() << myCloser << m_HTMLFilePathForJSON;
            myfile.close();
            m_outputFinalJSON = false;
         }
      }
      debugMsg("GanttViewer:printProgress", 
         "JSON tokens file written to "+ outputFileName);
   }

   /** generate the JSON tokens file during a plan's execution
   so that it can be parsed by Javascript in the Viewer **/
   void GanttListener::deliverPartialJSON() 
   {
      const string myCloser = "];";
      const string json_folder_path = m_currentWorkingDir + "/" + "json";
      std::ostringstream ss;
      string outputFileName;
      if (access(json_folder_path.c_str(), 0) != 0)
      {
         mkdir(json_folder_path.c_str(), S_IRWXG | S_IRGRP | 
            S_IROTH | S_IRUSR | S_IRWXU);
      }
      ss << m_currentWorkingDir << "/" <<
            "json/" << m_uniqueFileName << "_" << m_nodes[0].name << ".js";
      ofstream myfile;
      outputFileName = ss.str(); 
      myfile.open(outputFileName.c_str());
      if (myfile.fail())
      {
         m_continueOutputingData = false;
         debugMsg("GanttViewer:printErrors", "Failed to create JSON file!");
         return;
      }
      else
      {
         myfile << m_fullTemplate.str() << myCloser << m_HTMLFilePathForJSON;
         myfile.close();
      }
      debugMsg("GanttViewer:printProgress", 
         "JSON tokens file written to "+ outputFileName);
   }

   static string getLocalVarInExecStateFromMap(Node *nodeId, 
                                               vector<string>& myLocalVariableMapValues)
   {
      NodeVariableMap const *tempLocalVariablesMap = nodeId->getVariableMap();
      if (!tempLocalVariablesMap || tempLocalVariablesMap->empty())
      {
         return std::string();
      }
      std::ostringstream myLocalVars;
      for (NodeVariableMap::const_iterator it = tempLocalVariablesMap->begin();
         it != tempLocalVariablesMap->end(); ++it) 
      {
        const string& tempNameString = it->first;
        string tempValueString = it->second->valueString();
        myLocalVariableMapValues.push_back(tempValueString);
        //filter out local variables that are 'state' key  or 'UNKNOWN' value
        if (tempNameString != "state" && tempValueString != "UNKNOWN")
          myLocalVars << "<br><i>" << tempNameString << "</i>" 
                      << " = " << tempValueString << ", ";
      }
      return myLocalVars.str();
   }

   static string getChildNode(Node *nodeId)
   {
      std::ostringstream myChildNode;
      //get child nodes
      const vector<Node *>& tempChildList = nodeId->getChildren();
      if (tempChildList.size() == 0) 
      {
         return std::string();
      }
      else
      {
         for (vector<Node *>::const_iterator i = tempChildList.begin(); 
            i != tempChildList.end(); i++) 
           myChildNode << (*i)->getNodeId() << ", ";
      }
      return myChildNode.str();
   }
   
   /*
    * The job of createNodeObj() is to get the current time from the operating system 
    * so that GanttListener will then have the information regarding when this node begins 
    * executing. createNodeObj() will then determine this node's parent-child relations. 
    * Once these steps are completed, such information are stored in a NodeObj structure, 
    * which is defined inside GanttListener class.
    */
   GanttListener::NodeObj GanttListener::createNodeObj(Node *nodeId)
   {
      vector<string> myLocalVariableMapValues;

      //startTime is when first node executes
      if (m_startTime == -1) {
         m_startTime = nodeId->getCurrentStateStartTime();
      }

      m_parent.clear();

      double myStartValdbl = ((nodeId->getCurrentStateStartTime()) - m_startTime) * 100;

      if (nodeId->getParent())
         m_parent = nodeId->getParent()->getNodeId();
      if (m_parent.empty()) {
         m_parent = nodeId->getNodeId();
      }

      m_actualId++; //actualId ensures that looping nodes have the same ID for each token

      //determine if a node looping; assign prior 
      // ID for loops and a new one for non loops
      m_stateMap[nodeId] += 1;
      if(m_stateMap[nodeId] > 1) 
         m_actualId = m_counterMap[nodeId];
      else
         m_counterMap[nodeId] = m_actualId;

      //get local variables from map in state 'EXECUTING'
      string myLocalVars = getLocalVarInExecStateFromMap(nodeId, myLocalVariableMapValues);
      string myChildren = getChildNode(nodeId); //get child nodes
      return NodeObj(myStartValdbl,
                     -1,
                     -1,
                     nodeId->getNodeId(), 
                     nodeTypeString(nodeId->getType()),
                     nodeStateName(nodeId->getState()),
                     m_parent,
                     m_actualId,
                     myLocalVars,
                     myChildren,
                     myLocalVariableMapValues);
   }

   static string boldenFinalString(const vector<string>& prevLocalVarsVector, 
                                   const vector<string>& thisLocalVarsVectorValues,
                                   const vector<string>& thisLocalVarsVectorKeys,
                                   int i)
   {
      std::ostringstream tempFullString;
      //bolden final local variable values that changed during execution of node
      if(prevLocalVarsVector[i] != thisLocalVarsVectorValues[i]) 
      {
         tempFullString << "<i>" <<  
            thisLocalVarsVectorKeys[i] << "</i>" << " = " << 
            prevLocalVarsVector[i] << " --><strong><font color=\"blue\"> " << 
            thisLocalVarsVectorValues[i] << "</strong></font>";
      }
      else 
      {
         tempFullString << "<i>" <<  thisLocalVarsVectorKeys[i] << 
            "</i>" << " = " << prevLocalVarsVector[i] << " --> " <<
            thisLocalVarsVectorValues[i];
      }
      return tempFullString.str(); 
   }

   void GanttListener::processLocalVar(const vector<string>& prevLocalVarsVector, 
                                       const vector<string>& thisLocalVarsVectorValues, 
                                       const vector<string>& thisLocalVarsVectorKeys)
   {
      //make sure all local variable vectors are filled
      int smallerSize;
      vector<string> fullStrings;
      std::ostringstream ss;                   // nodeobj

      if(prevLocalVarsVector.size() > 1 && 
         thisLocalVarsVectorKeys.size() > 1 && 
         thisLocalVarsVectorValues.size() > 1) 
      {
         if(prevLocalVarsVector.size() < thisLocalVarsVectorKeys.size())
         {  
            smallerSize = prevLocalVarsVector.size();
         }
         else
         { 
            smallerSize = thisLocalVarsVectorKeys.size();
         }
         for(int i = 0; i < smallerSize; i++) 
         {
            //filter out local variables that are UNKNOWN at 
            // beginning of execution and at end of execution
            if(prevLocalVarsVector[i] != "UNKNOWN" || 
               thisLocalVarsVectorValues[i] != "UNKNOWN")
            { 
               fullStrings.push_back(boldenFinalString(prevLocalVarsVector, 
                  thisLocalVarsVectorValues, thisLocalVarsVectorKeys, i)); // performance issue
            }
         }
         for(size_t i = 0; i < fullStrings.size(); i++)
         {
            ss << "<br>" << fullStrings[i] << ", ";
            m_nodes[m_index].localvariables = ss.str();
         }
      }
      else 
      {
         m_nodes[m_index].localvariables = "";
      }
   }

   void GanttListener::getFinalLocalVar(Node *nodeId)
   {
      NodeVariableMap const *tempLocalVariableMapAfter = nodeId->getVariableMap();
      vector<string> prevLocalVarsVector = m_nodes[m_index].localvarsvector;
      vector<string> thisLocalVarsVectorKeys;
      vector<string> thisLocalVarsVectorValues;

      if(!m_nodes[m_index].localvariables.empty() &&
         m_nodes[m_index].localvarsvector.size() > 0) 
      {
         if (!tempLocalVariableMapAfter || tempLocalVariableMapAfter->empty())
         {
            m_nodes[m_index].localvariables = "";
         }
         else {
           for (NodeVariableMap::const_iterator it = tempLocalVariableMapAfter->begin(); 
                it != tempLocalVariableMapAfter->end(); it++) 
             {
               thisLocalVarsVectorKeys.push_back(it->first);
               thisLocalVarsVectorValues.push_back(it->second->valueString());
             }
         }
         processLocalVar(prevLocalVarsVector, thisLocalVarsVectorValues, 
                         thisLocalVarsVectorKeys);
      }
      else 
      {
         m_nodes[m_index].localvariables = "";
      }
   }

   void GanttListener::processTempValsForNode(Node *nodeId)
   {
      m_parent.clear();
      m_nodes[m_index].end = ((nodeId->getCurrentStateStartTime()) - m_startTime) * 100;
      m_nodes[m_index].duration = m_nodes[m_index].end - m_nodes[m_index].start;
      //doesn't exist until node is finished     
      if (nodeId->getParent()) {
         m_parent = nodeId->getParent()->getNodeId();
      }
      if(m_parent.empty()) {
         m_parent = m_nodes[m_index].name;
      }
      //get final values for local variables
      getFinalLocalVar(nodeId);
   }

   void GanttListener::produceSingleJSONObj()
   {
      //add '[' and ']' before and after duration and start to add uncertainty to those values
      //setup JSON object to be added to array
      m_fullTemplate << "{\n'id': " << m_nodes[m_index].id << ",\n'type':'" 
         << m_nodes[m_index].name 
         << "',\n'parameters': [\n{\n'name': 'entityName',\n'type': 'STRING',\n'value':'"
         << m_nodes[m_index].type <<
         "'\n},\n{\n'name': 'full type',\n'type': 'STRING',\n'value': '"
         << m_nodes[m_index].parent 
         << "." << m_nodes[m_index].name
         << "'\n},\n{\n'name': 'state',\n'type': 'STRING',\n'value':"
         << " 'ACTIVE'\n},\n{\n'name': 'object',\n'value': 'OBJECT:"
         << m_nodes[m_index].parent
         << "(6)'\n},\n{\n'name': 'duration',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].duration
         << "'\n},\n{\n'name': 'start',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].start
         << "'\n},\n{\n'name': 'end',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].end
         << "'\n},\n{\n'name': 'value',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].val
         << "'\n},\n{\n'name': 'children',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].children
         << "'\n},\n{\n'name': 'localvariables',\n'type': 'INT',\n'value': '"
         << m_nodes[m_index].localvariables <<"'\n}\n]\n},\n";
   }

   void GanttListener::createJSONStream()  
   {
      //add temp value to node
      m_nodes[m_index].parent = m_parent;
      produceSingleJSONObj();
   }

   void GanttListener::generateTempOutputFiles()
   {
      if (m_outputHTML == true)
      {
         createHTMLFile();
         m_outputHTML = false;
      }
      deliverPartialJSON();
      
      debugMsg("GanttViewer:printProgress", 
         "finished gathering partial data; JSON and HTML stored");
   }

   void GanttListener::generateFinalOutputFiles()
   {
      if (m_nodes[m_index].id == 1)
      { 
         if (m_outputHTML == true)
         {
            createHTMLFile();
            m_outputHTML = false;
         }
         deliverJSONAsFile();
      }
      else
      {  
         if (m_planFailureState == false)
         {
            generateTempOutputFiles();
         }
      }
      debugMsg("GanttViewer:printProgress", 
            "finished gathering data; JSON and HTML stored");
   }

   void GanttListener::findNode(Node *nodeId)
   {
      // find the node it corresponds to in nodes vector
      int i, size;
      string tempId = nodeId->getNodeId();
      string const &tempType = nodeTypeString(nodeId->getType());
      string tempParent = "invalid_parent_id";
      if(nodeId->getParent()) {
         tempParent = nodeId->getParent()->getNodeId();
      }
      size = m_nodes.size();
      for(i = 0; i < size; i++) // tree search
      {  
         if(tempParent != "invalid_parent_id") 
         {
            if(tempId == m_nodes[i].name && tempType == m_nodes[i].type && 
               tempParent == m_nodes[i].parent) 
            {
               m_index = i;
            }
         }
         else 
         {
            m_index = 0;
            break;
         }
      }
   }

   /*
    * It will use this information to first locate the node from the vector of NodeObj with 
    * findNode(). Then, processOutputData() will call processTempValsForNode() to update the 
    * time this node stopped execution, and calculate the duration of execution. 
    * Once these are done, processOutputData() will call createJSONStream(), which will produce 
    * a single JSON object by calling produceSingleJSONObj().
    * Finally, when the JSON object is ready, it will be send to output via a call to 
    * generateFinalOutputFiles() from processOutputData(). Note, however, that 
    * generateFinalOutputFiles() has internal logics that prevent writing unnecessary file 
    * repeatedly, and it supports writing data to JSON output even with partially executed plans 
    * and failed plans. 
    */
   void GanttListener::processOutputData(Node *nodeId)
   {
      findNode(nodeId);
      processTempValsForNode(nodeId); 
      createJSONStream();

      if (m_continueOutputingData == true)
      {
         generateFinalOutputFiles();
      }

      debugMsg("GanttViewer:printProgress", "Token added for node " +
               nodeTypeString(nodeId->getType()) + "." + nodeId->getNodeId());
   }   

   /** executed when nodes transition state
   *  resets the start time so it can be used in temporal calculations,
   *  grabs info from nodes in executing state,
   *  grabs info from nodes in finished state,
   *  nodes info is stored in each node's NodeObj struct
   **/
   void GanttListener::implementNotifyNodeTransition(NodeState /* prevState */, 
                                                     Node *nodeId) const
   {  
      static GanttListener myListener;
      //get state
      const NodeState& newState = nodeId->getState();
      switch (newState) {
         case EXECUTING_STATE:
            myListener.m_nodes.push_back(myListener.createNodeObj(nodeId));
            break;
         case FAILING_STATE:
            myListener.m_planFailureState = true;
            // fall through to FINISHED_STATE
         case FINISHED_STATE:
            myListener.processOutputData(nodeId);
            break;
         default:
            break;
      }
   }

   extern "C" {
      void initGanttListener() {
         REGISTER_EXEC_LISTENER(GanttListener, "GanttListener");
      }
   }
}
