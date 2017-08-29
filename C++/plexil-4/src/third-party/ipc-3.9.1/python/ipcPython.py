##############################################################################
# PROJECT: IPC (Interprocess Communication) Package
#
# (c) Copyright 2011 Reid Simmons.  All rights reserved.
#
# FILE: ipcPython.py
#
# ABSTRACT: Python-code for interfacing specifically with the C version of IPC
#           Used by SWIG (see ffi/IPC.i)
#
#       $Id: ipcPython.py,v 1.2 2011/08/17 00:47:04 reids Exp $
# $Revision: 1.2 $
#     $Date: 2011/08/17 00:47:04 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# Copyright (c) 2011, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# REVISION HISTORY
# $Log: ipcPython.py,v $
# Revision 1.2  2011/08/17 00:47:04  reids
# Removed mention of IPC_freeData and IPC_freeDataElements.
# Changed the signature of IPC_unmarshall.
#
# Revision 1.1  2011/08/16 16:00:36  reids
# Adding Python interface to IPC
#
################################################################

%pythoncode %{
# ipc passes IPC_WAIT_FOREVER = -1 as an unsigned int which breaks SWIG
IPC_WAIT_FOREVER = 4294967295L
# ipc passes IPC_VARIABLE_LENGTH = -1 as an unsigned int which breaks SWIG
IPC_VARIABLE_LENGTH = 4294967295L
IPC_FIXED_LENGTH = 4294967294L

msgHashTable = {}
fdHashTable = {}
queryHashTable = {}
timerHashTable = {}
hndChangeTable = {}
connectHandlers = []
disconnectHandlers = []
handlerIndex = 0

# Specific exception for internal IPC errors
class IPCError(StandardError) : "Internal IPC error"

def init_python_ipc() :
  global msgHashTable, fdHashTable, queryHashTable, timerHashTable
  global hndChangeTable, connectHandlers, disconnectHandlers

  setExitProc()
  msgHashTable = {}
  fdHashTable = {}
  queryHashTable = {}
  timerHashTable = {}
  hndChangeTable = {}
  connectHandlers = []
  disconnectHandlers = []
  handlerIndex = 0

def boolVal (retval) :
  if (retval == 1) : return True
  else : return False

def findExisting (handler, handlers) :
  for hndData in handlers :
    if (handler == hndData[0]) : return hndData
  return None

def handlerName (msgName, handler) : return msgName+"_"+handler.__name__

#
# Don't exit interpreter on errors -- raise exception, instead
#
def IPC_initialize() :
  init_python_ipc()
  return _IPC.IPC_initialize()

def IPC_connectModule(*args) :
  init_python_ipc()
  _IPC.IPC_connectModule(*args)

def IPC_connect(*args) :
  init_python_ipc()
  _IPC.IPC_connect(*args)

def IPC_connectModuleNoListen(*args) :
  init_python_ipc()
  _IPC.IPC_connectModuleNoListen(*args)

def IPC_connectNoListen(*args) :
  init_python_ipc()
  _IPC.IPC_connectNoListen(*args)

def IPC_isConnected () :
  return boolVal(_IPC.IPC_isConnected())

def IPC_isModuleConnected (*args) :
  return boolVal(_IPC.IPC_isModuleConnected(*args))

def IPC_isMsgDefined (*args) :
  return boolVal(_IPC.IPC_isMsgDefined(*args))

def IPC_publish (msgName, length, content) :
  if (isinstance(content, (tuple, list))) :
    raise "IPC_publish using list/tuples NYI"
  else :
    return _IPC.IPC_publish(msgName, length, content)

def IPC_subscribe (msgName, handler, clientData=None) :
  global msgHashTable
  key = id(handler)
  msgHashTable[key] = (handler, clientData, None, False)
  return subscribe(msgName, handlerName(msgName, handler), key)

def IPC_subscribeData (msgName, handler, clientData=None, dataClass=None) :
  global msgHashTable
  key = id(handler)
  msgHashTable[key] = (handler, clientData, dataClass, True)
  return subscribe(msgName, handlerName(msgName, handler), key)

def IPC_unsubscribe (msgName, handler) :
  global msgHashTable
  key = id(handler)
  msgHashTable[key] = None
  return _IPC_unsubscribe(msgName, handlerName(msgName, handler))  

def IPC_subscribeFD (fd, fdHandler, clientData=None) :
  global fdHashTable
  key= id(fdHandler)
  fdHashTable[key] = (fdHandler, clientData) 
  return subscribeFD(fd, key)

def IPC_unsubscribeFD (fd, handler) :
  try :
    del fdHashTable[id(handler)]
    return unsubscribeFD(fd)
  except :
    print "Ooops: %s not subscribed for fd %d" % (handler.__name__, fd)
    return IPC_Error

def IPC_subscribeConnect (connectHandler, clientData=None) :
  global connectHandlers
  oldLen = len(connectHandlers)
  # Do it this way because multiple handlers can be subscribed
  hndData = findExisting(connectHandler, connectHandlers)
  if (hndData is None) :
    connectHandlers.append([connectHandler, clientData])
  elif (hndData[1] != clientData) :
    print "Resetting client data for connect handler", connectHandler.__name__
    hndData[1] = clientData

  if (oldLen == 0 and len(connectHandlers) == 1) :
    return subscribeConnect()
  elif IPC_isConnected() : return IPC_OK
  else : return IPC_Error

def IPC_subscribeDisconnect (disconnectHandler, clientData=None) :
  global disconnectHandlers
  oldLen = len(disconnectHandlers)
  hndData = findExisting(disconnectHandler, disconnectHandlers)
  if (hndData is None) :
    disconnectHandlers.append([disconnectHandler, clientData])
  elif (hndData[1] != clientData) :
    print "Resetting client data for disconnect handler", \
          disconnectHandler.__name__
    hndData[1] = clientData

  if (oldLen == 0 and len(disconnectHandlers) == 1) :
    return subscribeDisconnect()
  elif IPC_isConnected() : return IPC_OK
  else : return IPC_Error

def IPC_unsubscribeConnect (connectHandler) :
  hndData = findExisting(connectHandler, connectHandlers)
  if (hndData is None) :
    print "IPC_unsubscribeConnect: Connect handler %s not found " % \
    	  connectHandler.__name__
    return IPC_Error
  else :
    connectHandlers.remove(hndData)
    if (len(connectHandlers) == 0) :
      return unsubscribeConnect()
    else : return IPC_OK

def IPC_unsubscribeDisconnect (disconnectHandler) :
  hndData = findExisting(disconnectHandler, disconnectHandlers)
  if (hndData is None) :
    print "IPC_unsubscribeConnect: Disconnect handler %s not found " % \
    	  connectHandler.__name__
    return IPC_Error
  else :
    disconnectHandlers.remove(hndData)
    if (len(disconnectHandlers) == 0) :
      return unsubscribeDisconnect()
    else : return IPC_OK

def IPC_subscribeHandlerChange (msgName, changeHandler, clientData=None) :
  global hndChangeTable
  # Do it this way because multiple handlers can be subscribed for same message
  try :
    msgChangeHandlers = hndChangeTable[msgName]
  except KeyError :
    msgChangeHandlers = []
    hndChangeTable[msgName] = msgChangeHandlers

  oldLen = len(msgChangeHandlers)
  hndData = findExisting(changeHandler, msgChangeHandlers)
  if (hndData is None) :
    msgChangeHandlers.append([changeHandler, clientData])
  elif (hndData[1] != clientData) :
    print "Resetting client data for change handler", changeHandler.__name__
    hndData[1] = clientData

  if (oldLen == 0 and len(msgChangeHandlers) == 1) :
    return subscribeHandlerChange(msgName)
  elif IPC_isConnected() : return IPC_OK
  else : return IPC_Error

def IPC_unsubscribeHandlerChange (msgName, changeHandler) :
  global hndChangeTable
  try :
    msgChangeHandlers = hndChangeTable[msgName]
  except KeyError :
    print "No change handler found for message", msgName
    return IPC_Error

  hndData = findExisting(changeHandler, msgChangeHandlers)
  if (hndData is None) :
    print "No change handler found for message", msgName
    return IPC_Error
  else :
    msgChangeHandlers.remove(hndData);
    if (len(msgChangeHandlers) == 0) :
      return unsubscribeHandlerChange(msgName)
    else : return IPC_OK

def IPC_queryNotify (msgName, length, content, handler, clientData) :
  global queryHashTable, handlerIndex
  
  handlerIndex = handlerIndex + 1
  queryHashTable[handlerIndex] = (handler, clientData, None, False)
  return queryNotify(msgName, length, content, handlerIndex)

def IPC_queryResponse (msgName, length, content, timeout) :
  vc = IPC_VARCONTENT_TYPE()
  replyFormat = FORMATTER_CONTAINER_TYPE()
  ret = queryResponse(msgName, length, content, vc, replyFormat, timeout)
  return (vc.content, ret)

def IPC_queryNotifyVC (msgName, varcontent, handler, clientData) :
  return IPC_queryNotify(msgName, varcontent.length, varcontent.content,
                         handler, clientData)

def IPC_queryResponseVC (msgName, varcontent, timeout) :
  return IPC_queryResponse(msgName, varcontent.length, varcontent.content,
                           timeout)

def IPC_marshall (formatter, data, varcontent) :
  return formatters.marshall(formatter, data, varcontent)

def IPC_unmarshall (formatter, bytearray, oclass=None) :
  return formatters.unmarshall(formatter, bytearray, None, oclass)

def IPC_unmarshallData (formatter, bytearray, object=None, oclass=None) :
  return formatters.unmarshall(formatter, bytearray, object, oclass)

def IPC_publishData (msgName, data) :
  if (data == None) :
    varcontent.length = 0
    varcontent.content = None
    return _IPC.IPC_publish(msgName, 0, None)
  else :
    varcontent = IPC_VARCONTENT_TYPE()
    try :
      if (formatters.marshall(IPC_msgFormatter(msgName), data, varcontent) ==
          IPC_Error) :
        return IPC_Error
      else :
        retVal = _IPC.IPC_publish(msgName, varcontent.length, varcontent.content)
        if (varcontent.content != 0) : IPC_freeByteArray(varcontent.content)
        return retVal
    except IPC_Error : return IPC_Error

def IPC_respondData (msgInstance, msgName, data) :
  varcontent = IPC_VARCONTENT_TYPE()
  try :
    if (formatters.marshall(IPC_msgFormatter(msgName), data, varcontent) ==
        IPC_Error) :
      return IPC_Error
    else :
      retVal = IPC_respondVC(msgInstance, msgName, varcontent)
      if (varcontent.content != 0) : IPC_freeByteArray(varcontent.content)
      return retVal
  except : return IPC_ERROR

def IPC_queryNotifyData (msgName, data, handler, clientData=None,
                         responseClass=None) :
  global queryHashTable, handlerIndex
  
  handlerIndex = handlerIndex + 1
  queryHashTable[handlerIndex] =(handler, clientData, responseClass, True)

  if (data == None) :
    varcontent.length = 0
    varcontent.content = None
    return _IPC.IPC_publish(msgName, 0, None)
  else :
    varcontent = IPC_VARCONTENT_TYPE()
    try :
      if (formatters.marshall(IPC_msgFormatter(msgName), data, varcontent) ==
          IPC_Error) :
        return IPC_Error
      else :
        retVal = queryNotify(msgName, varcontent.length, 
	  	             varcontent.content, handlerIndex)
        if (varcontent.content != 0) : IPC_freeByteArray(varcontent.content)
        return retVal
    except : return IPC_ERROR

def IPC_queryResponseData (msgName, data, timeoutMSecs, responseClass=None) :
  try :
    responseObject = None
    varcontent = IPC_VARCONTENT_TYPE()
    if (formatters.marshall(IPC_msgFormatter(msgName), data, varcontent) !=
        IPC_Error) :
      vc = IPC_VARCONTENT_TYPE()
      replyFormat = FORMATTER_CONTAINER_TYPE()
      ret = queryResponse(msgName, varcontent.length, varcontent.content, 
                          vc, replyFormat, timeoutMSecs)
      if (varcontent.content != 0) : IPC_freeByteArray(varcontent.content)
      if (ret == IPC_OK) :
        (responseObject, ret) = formatters.unmarshall(replyFormat.formatter,
                                                      vc.content,
						      oclass= responseClass)
  except : print exc_info(); return (None, IPC_Error)
  return (responseObject, ret)

# This is pretty arcane...
def IPC_printData (formatter, stream, data) :
  vc = IPC_VARCONTENT_TYPE()
  formatters.marshall(formatter, data, vc)
  return printData(formatter, stream.fileno(), vc)

def IPC_readData (formatter, stream) :
  raise "IPC_readData: Not yet implemented"

def addTimer (tdelay, count, handler, clientData) :
  global timerHashTable, handlerIndex

  handlerIndex = handlerIndex + 1
  timerRef = TIMER_REF_CONTAINER_TYPE()
  retval = addTimerGetRef(tdelay, count, handlerIndex, timerRef)
  if (retval == IPC_OK) :
    timerHashTable[handler] = handlerIndex
    timerHashTable[handlerIndex] = (handler, clientData, timerRef.timerRef)
    return (timerRef.timerRef, retval)
  else : return (None, retval)

def removeTimer (handlerIndex) :
  global timerHashTable
  del timerHashTable[timerHashTable[handlerIndex][0]]
  del timerHashTable[handlerIndex]

def IPC_addTimer (tdelay, count, handler, clientData) :
  global timerHashTable

  # Replace existing timer, if needed
  try:
    hndIndex = timerHashTable[handler]
    removeTimer(hndIndex)
    print "Replacing existing timer for handler", handler.__name__
  except: pass

  (timerRef, retval) = addTimer(tdelay, count, handler, clientData)
  return retval

def IPC_addTimerGetRef (tdelay, count, handler, clientData) :
  return addTimer(tdelay, count, handler, clientData)

def IPC_addOneShotTimer (tdelay, handler, clientData) :
  return IPC_addTimer(tdelay, 1, handler, clientData)

def IPC_addPeriodicTimer (tdelay, handler, clientData) :
  return IPC_addTimer(tdelay, TRIGGER_FOREVER, handler, clientData)

def IPC_removeTimer (handler) :
  global timerHashTable
  try:
    hndIndex = timerHashTable[handler]
    timerData = timerHashTable[hndIndex]
    _IPC.IPC_removeTimerByRef(timerData[2])
    removeTimer(hndIndex)
  except:
    print "Timer for handler (%s) does not exist" % handler.__name__

  return IPC_OK

def IPC_removeTimerByRef (timerRef) :
  global timerHashTable
  for hndIndex in timerHashTable :
    if (isinstance(hndIndex, int)) :
      timerData = timerHashTable[hndIndex]
      if (timerData[2] == timerRef) :
        _IPC.IPC_removeTimerByRef(timerData[2])
        removeTimer(hndIndex)
        return IPC_OK  
  # No matching timerRef found 
  print "Timer with ref %s does not exist" % timerRef
  return IPC_OK

def msgCallbackHandler (msgInstance, byteArray, key) :
  global msgHashTable
  try :
    handlerData = msgHashTable[key]
  except : 
    print "Ooops -- no handler for %s" % IPC_msgInstanceName(msgInstance)
    handlerData = None

  if (not handlerData is None) :
    try: 
      if (handlerData[3]) : # Auto-unmarshall
        try :
          (data, retval) = \
             formatters.unmarshall(IPC_msgInstanceFormatter(msgInstance),
                                   byteArray, None, handlerData[2])
        except : retval = IPC_Error
        if (retval != IPC_OK) : raise "msgCallbackHandler: unmarshall failed"
        handlerData[0](msgInstance, data, handlerData[1])
        IPC_freeByteArray(byteArray)
      else :
        handlerData[0](msgInstance, byteArray, handlerData[1])
    except: 
      print "Handler failed: %s: %s" % \
            (exc_info()[0].__name__, exc_info()[1].message)
      raise

def queryCallbackHandler (msgInstance, byteArray, key) :
  global queryHashTable
  try :
    handlerData = queryHashTable[key]
  except : 
    print "Ooops -- no query notification handler for %s" % \
          IPC_msgInstanceName(msgInstance)
    handlerData = None

  if (not handlerData is None) :
    del queryHashTable[key]
    try: 
      if (handlerData[3]) : # Auto-unmarshall
        try : (data, retval) = formatters.unmarshall(IPC_msgInstanceFormatter(msgInstance),
                                                     byteArray, handlerData[2])
        except : retval = IPC_Error
        if (retval != IPC_OK) : raise "queryCallbackHandler: unmarshall failed"
        handlerData[0](msgInstance, data, handlerData[1])
        IPC_freeByteArray(byteArray)
      else :
        handlerData[0](msgInstance, byteArray, handlerData[1])
    except: 
      print "Handler failed: %s: %s" % \
            (exc_info()[0].__name__, exc_info()[1].message)
      raise


def fdCallbackHandler (fd, key) :
  try :
    handlerData = fdHashTable[key]
  except : 
    print "Ooops -- no handler for fd %d" % fd
    handlerData = None

  if (not handlerData is None) :
    handlerData[0](fd, handlerData[1])

def timerCallbackHandler (hndIndex, currentTime, scheduledTime) :
  try:
    timerData = timerHashTable[hndIndex]
    timerData[0](timerData[1], currentTime, scheduledTime)
    numTriggers = maxTriggers(timerData[2])
    if (numTriggers == 0) :
      print "Deleting timer", timerData[0].__name__
      removeTimer(hndIndex)
  except:
    print "Ooops -- no handler for timer", exc_info()

def connectCallbackHandler (moduleName) :
  global connectHandlers
  for handlerData in connectHandlers :
    handlerData[0](moduleName, handlerData[1])

def disconnectCallbackHandler (moduleName) :
  global disconnectHandlers
  for handlerData in disconnectHandlers :
    handlerData[0](moduleName, handlerData[1])

def changeCallbackHandler (msgName, numHandlers) :
  try :
    msgChangeHandlers = hndChangeTable[msgName]
  except KeyError :
    print "Ooops -- no change handlers for message", msgName
    return

  for hndData in msgChangeHandlers :
    hndData[0](msgName, numHandlers, hndData[1])
%}
