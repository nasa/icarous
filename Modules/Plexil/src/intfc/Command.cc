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

#include "Command.hh"

#include "Assignable.hh"
#include "ExprVec.hh"
#include "ExternalInterface.hh"
#include "InterfaceError.hh"
#include "PlanError.hh"

namespace PLEXIL
{

  //
  // ResourceSpec implementation
  //
  
  ResourceSpec::ResourceSpec()
    : nameExp(NULL),
      priorityExp(NULL),
      lowerBoundExp(NULL),
      upperBoundExp(NULL),
      releaseAtTermExp(NULL),
      nameIsGarbage(false),
      priorityIsGarbage(false),
      lowerBoundIsGarbage(false),
      upperBoundIsGarbage(false),
      releaseIsGarbage(false)
  {
  }
  
  ResourceSpec::ResourceSpec(ResourceSpec const &orig)
    : nameExp(orig.nameExp),
      priorityExp(orig.priorityExp),
      lowerBoundExp(orig.lowerBoundExp),
      upperBoundExp(orig.upperBoundExp),
      releaseAtTermExp(orig.releaseAtTermExp),
      nameIsGarbage(orig.nameIsGarbage),
      priorityIsGarbage(orig.priorityIsGarbage),
      lowerBoundIsGarbage(orig.lowerBoundIsGarbage),
      upperBoundIsGarbage(orig.upperBoundIsGarbage),
      releaseIsGarbage(orig.releaseIsGarbage)
  {
  }
  
  ResourceSpec &ResourceSpec::operator=(ResourceSpec const &orig)
  {
    nameExp = orig.nameExp;
    priorityExp = orig.priorityExp;
    lowerBoundExp = orig.lowerBoundExp;
    upperBoundExp = orig.upperBoundExp;
    releaseAtTermExp = orig.releaseAtTermExp;
    nameIsGarbage = orig.nameIsGarbage;
    priorityIsGarbage = orig.priorityIsGarbage;
    lowerBoundIsGarbage = orig.lowerBoundIsGarbage;
    upperBoundIsGarbage = orig.upperBoundIsGarbage;
    releaseIsGarbage = orig.releaseIsGarbage;

    return *this;
  }

  void ResourceSpec::setNameExpression(Expression *e, bool isGarbage)
  {
    nameExp = e;
    nameIsGarbage = isGarbage;
  }

  void ResourceSpec::setPriorityExpression(Expression *e, bool isGarbage)
  {
    priorityExp = e;
    priorityIsGarbage = isGarbage;
  }

  void ResourceSpec::setLowerBoundExpression(Expression *e, bool isGarbage)
  {
    lowerBoundExp = e;
    lowerBoundIsGarbage = isGarbage;
  }

  void ResourceSpec::setUpperBoundExpression(Expression *e, bool isGarbage)
  {
    upperBoundExp = e;
    upperBoundIsGarbage = isGarbage;
  }

  void ResourceSpec::setReleaseAtTerminationExpression(Expression *e, bool isGarbage)
  {
    releaseAtTermExp = e;
    releaseIsGarbage = isGarbage;
  }

  ResourceSpec::~ResourceSpec()
  {
    cleanUp();
  }

  void ResourceSpec::cleanUp()
  {
    if (nameIsGarbage)
      delete nameExp;
    nameExp = NULL;

    if (priorityIsGarbage)
      delete priorityExp;
    priorityExp = NULL;

    if (lowerBoundIsGarbage)
      delete lowerBoundExp;
    lowerBoundExp = NULL;

    if (upperBoundIsGarbage)
      delete upperBoundExp;
    upperBoundExp = NULL;

    if (releaseIsGarbage)
      delete releaseAtTermExp;
    releaseAtTermExp = NULL;
  }

  void ResourceSpec::activate()
  {
    nameExp->activate();
    priorityExp->activate();
    if (lowerBoundExp)
      lowerBoundExp->activate();
    if (upperBoundExp)
      upperBoundExp->activate();
    if (releaseAtTermExp)
      releaseAtTermExp->activate();
  }

  void ResourceSpec::deactivate()
  {
    nameExp->deactivate();
    priorityExp->deactivate();
    if (lowerBoundExp)
      lowerBoundExp->deactivate();
    if (upperBoundExp)
      upperBoundExp->deactivate();
    if (releaseAtTermExp)
      releaseAtTermExp->deactivate();
  }

  Command::Command(std::string const &nodeName)
    : m_next(NULL),
      m_ack(*this),
      m_abortComplete("abortComplete"),
      m_command(),
      m_nameExpr(NULL),
      m_dest(NULL),
      m_argVec(NULL),
      m_resourceList(NULL),
      m_resourceValueList(NULL),
      m_commandHandle(NO_COMMAND_HANDLE),
      m_active(false),
      m_commandFixed(false),
      m_commandIsConstant(false),
      m_resourcesFixed(false),
      m_resourcesAreConstant(false),
      m_nameIsGarbage(false),
      m_destIsGarbage(false),
      m_checkedConstant(false)
  {
    m_ack.setName(nodeName);
  }

  Command::~Command() 
  {
    cleanUp();
  }

  void Command::cleanUp()
  {
    if (m_nameIsGarbage) {
      delete m_nameExpr;
      m_nameIsGarbage = false;
    }
    m_nameExpr = NULL;

    delete m_argVec;
    m_argVec = NULL;

    if (m_destIsGarbage) {
      delete m_dest;
      m_destIsGarbage = false;
    }
    m_dest = NULL;

    if (m_resourceList) {
      for (ResourceList::iterator it = m_resourceList->begin();
           it != m_resourceList->end();
           ++it)
        it->cleanUp();
      delete m_resourceList;
      m_resourceList = NULL;
      delete m_resourceValueList;
      m_resourceValueList = NULL;
    }
  }

  void Command::setDestination(Expression *dest, bool isGarbage)
  {
    m_dest = dest;
    m_destIsGarbage = isGarbage;
  }

  void Command::setNameExpr(Expression *nameExpr, bool isGarbage)
  {
    m_nameExpr = nameExpr;
    m_nameIsGarbage = isGarbage;
  }

  void Command::setResourceList(ResourceList *l)
  {
    if (m_resourceList && m_resourceList != l) // unlikely, but...
      delete m_resourceList;

    m_resourceList = l;
    m_resourcesAreConstant = false; // must check
  }

  void Command::setArgumentVector(ExprVec *vec)
  {
    if (m_argVec)
      delete m_argVec;
    m_argVec = vec;
  }

  State const &Command::getCommand() const
  {
    assertTrue_1(m_commandFixed);
    return m_command;
  }

  std::string const &Command::getName() const
  {
    assertTrue_1(m_commandFixed);
    return m_command.name();
  }

  std::vector<Value> const &Command::getArgValues() const
  {
    assertTrue_1(m_commandFixed);
    return m_command.parameters();
  }

  const ResourceValueList &Command::getResourceValues() const
  {
    static ResourceValueList const sl_emptyList;

    assertTrue_1(m_resourcesFixed);
    if (!m_resourceList)
      return sl_emptyList;
    else
      return *m_resourceValueList;
  }

  Expression *Command::getDest()
  {
    if (m_dest)
      return m_dest;
    else
      return NULL;
  }

  bool Command::isCommandConstant() const
  {
    if (!m_nameExpr->isConstant())
      return false;
    if (m_argVec)
      for (size_t i = 0; i < m_argVec->size(); ++i)
        if (!(*m_argVec)[i]->isConstant())
          return false;
    return true;
  }

  bool Command::areResourcesConstant() const
  {
    if (!m_resourceList)
      return true;

    for (ResourceList::const_iterator it = m_resourceList->begin();
         it != m_resourceList->end();
         ++it) {
      if (!it->nameExp->isConstant()
          || !it->priorityExp->isConstant()
          || (it->lowerBoundExp && !it->lowerBoundExp->isConstant())
          || (it->upperBoundExp && !it->upperBoundExp->isConstant())
          || (it->releaseAtTermExp && !it->releaseAtTermExp->isConstant()))
        return false;
    }

    return true;
  }

  void Command::fixValues() 
  {
    assertTrue_1(m_active);
    std::string const *name;
    m_nameExpr->getValuePointer(name);
    m_command.setName(*name);
    
    if (m_argVec) {
      size_t n = m_argVec->size();
      m_command.setParameterCount(n);
      for (size_t i = 0; i < n; ++i)
        m_command.setParameter(i, (*m_argVec)[i]->toValue());
    }

    m_commandFixed = true;
  }

  void Command::fixResourceValues()
  {
    check_error_1(m_active);
    if (!m_resourceList)
      return;
    
    size_t n = m_resourceList->size();
    if (!m_resourceValueList)
      m_resourceValueList = new ResourceValueList(n);

    for (size_t i = 0; i < n; ++i) {
      ResourceSpec const &spec = (*m_resourceList)[i];
      ResourceValue &resValue = (*m_resourceValueList)[i];
      checkPlanError(spec.nameExp->getValue(resValue.name),
                     "Command resource name expression has unknown or invalid value");
      checkPlanError(spec.priorityExp->getValue(resValue.priority),
                     "Command resource priority expression has unknown or invalid value");

      if (spec.lowerBoundExp) {
        checkPlanError(spec.lowerBoundExp->getValue(resValue.lowerBound),
                       "Command resource lower bound expression has unknown or invalid value");
      }
      else
        resValue.lowerBound = 1.0;

      if (spec.upperBoundExp) {
        checkPlanError(spec.upperBoundExp->getValue(resValue.upperBound),
                       "Command resource upper bound expression has unknown or invalid value");
      }
      else 
        resValue.upperBound = 1.0;

      if (spec.releaseAtTermExp) {
        checkPlanError(spec.releaseAtTermExp->getValue(resValue.releaseAtTermination),
                       "Command resource lower bound expression has unknown or invalid value");
      }
      else
        resValue.releaseAtTermination = true;
    }
    m_resourcesFixed = true;
  }

  void Command::activate()
  {
    check_error_1(!m_active);
    check_error_1(m_nameExpr);

    m_commandHandle = NO_COMMAND_HANDLE;
    m_ack.activate();
    m_abortComplete.activate();

    // Will always be false on first activation
    // and if relevant parameters are not constants
    if (!m_commandIsConstant)
      m_commandFixed = false;
    if (!m_resourcesAreConstant)
      m_resourcesFixed = false;

    if (m_dest)
      m_dest->activate();

    // If command fixed, these are already active
    if (!m_commandFixed) {
      m_nameExpr->activate();
      if (m_argVec)
        m_argVec->activate();
    }

    // If resources fixed, they are already active
    if (m_resourceList && !m_resourcesFixed)
      if (m_resourceList)
        for (ResourceList::iterator it = m_resourceList->begin();
             it != m_resourceList->end();
             ++it)
          it->activate();

    // Check for constancy at first activation
    if (!m_checkedConstant) {
      m_commandIsConstant = isCommandConstant();
      if (m_resourceList)
        m_resourcesAreConstant = areResourcesConstant();
      else
        m_resourcesAreConstant = m_resourcesFixed = true; // because there aren't any
      m_checkedConstant = true;
    }

    m_active = true;
  }

  void Command::execute()
  {
    check_error_1(m_active);
    if (!m_commandFixed)
      fixValues();
    if (!m_resourcesFixed)
      fixResourceValues();
    g_interface->enqueueCommand(this);
  }

  void Command::setCommandHandle(CommandHandleValue handle)
  {
    if (!m_active)
      return;
    checkInterfaceError(handle > NO_COMMAND_HANDLE && handle < COMMAND_HANDLE_MAX,
                        "Invalid command handle value");
    m_commandHandle = handle;
    m_ack.valueChanged();
  }

  void Command::returnValue(Value const &val)
  {
    if (!m_active || !m_dest)
      return;
    m_dest->asAssignable()->setValue(val);
  }

  void Command::abort()
  {
    check_error_1(m_active);
    // Handle stupid unit test
    if (g_interface) {
      g_interface->abortCommand(this);
    }
  }

  void Command::acknowledgeAbort(bool ack)
  {
    // Ignore late or erroneous acks
    if (!m_active)
      return;
    m_abortComplete.setValue(ack);
  }

  void Command::deactivate() 
  {
    check_error_1(m_active);
    m_active = false;

    if (m_commandHandle != COMMAND_DENIED)
      g_interface->releaseResourcesForCommand(this);

    m_abortComplete.deactivate();
    m_ack.deactivate();

    if (m_dest)
      m_dest->deactivate();

    // Don't deactivate if resources are constant
    if (m_resourceList) {
      if (!m_resourcesAreConstant) {
        for (ResourceList::iterator it = m_resourceList->begin();
             it != m_resourceList->end();
             ++it)
          it->deactivate();
        m_resourcesFixed = false;
      }
    }

    // Don't deactivate if command is constant
    if (!m_commandIsConstant) {
      m_nameExpr->deactivate();
      if (m_argVec)
        m_argVec->deactivate();
      m_commandFixed = false;
    }
  }

}
