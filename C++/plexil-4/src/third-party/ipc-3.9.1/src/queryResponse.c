/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: queryResponse.c
 *
 * ABSTRACT: Implement several variations of query/response functions for
 *           the IPC, using (modified) X_IPC library.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: queryResponse.c,v $
 * Revision 2.5  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2002/01/03 20:52:16  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.3  2001/01/12 15:53:45  reids
 * Added IPC_delayResponse to enable responding to a query outside of the
 *   message handler.
 *
 * Revision 2.2  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.6  1997/01/27 20:09:55  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.4  1997/01/11 01:21:19  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.3.2.3.6.1  1996/12/24 14:41:45  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.3.2.3  1996/10/22 18:49:45  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.3.2.2  1996/10/18 18:10:22  reids
 * Better error checking and handling.
 *
 * Revision 1.3.2.1  1996/10/02 20:58:40  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.3  1996/05/24 20:01:43  rouquett
 * swapped include order between ipc.h globalM.h for solaris compilation
 *
 * Revision 1.2  1996/04/24 19:13:52  reids
 * Changes to support vxworks version.
 *
 * Revision 1.1  1996/03/03 04:36:22  reids
 * First release of IPC files.  Corresponds to IPC Specifiction 2.2, except
 * that IPC_readData is not yet implemented.  Also contains "cover" functions
 * for the xipc interface.
 *
 ****************************************************************/

#include "ipc.h"
#include "globalM.h"
#include "ipcPriv.h"

/****************************************************************
 *                FORWARD DECLARATIONS
 ****************************************************************/

static void queryReplyHandler (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			       void *clientData);
static BOOLEAN testQueryReplyData(QUERY_REPLY_PTR queryReply,
				  QUERY_NOTIFICATION_PTR element);

IPC_RETURN_TYPE IPC_respond (MSG_INSTANCE msgInstance, const char *msgName,
			     unsigned int length, BYTE_ARRAY content)
{
  MSG_PTR msg;
  void *replyData;
  IPC_VARCONTENT_TYPE vc;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!msgInstance) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!X_IPC_CONNECTED()) {
    RETURN_ERROR(IPC_Not_Connected);
  } else {
    msg = x_ipc_msgFind(msgName);
    if (!msg) {
      RETURN_ERROR(IPC_Message_Not_Defined);
    } else if (ipcDataToSend(msg->msgData->msgFormat, msgName, 
			     length, content, &replyData, &vc) != IPC_OK) {
      PASS_ON_ERROR();
    } else {
      return ipcReturnValue(x_ipc_sendResponse(msgInstance, msg,
					       (char *)replyData,
					       ReplyClass, NULL,
					       msgInstance->responseSd));
    }
  }
}

/* If the response (IPC_respond) happens outside of the handler, 
   must call this function from within the handler!! */
IPC_RETURN_TYPE IPC_delayResponse (MSG_INSTANCE msgInstance)
{
  if (!msgInstance) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!msgInstance->msg) {
    RETURN_ERROR(IPC_Argument_Out_Of_Range);
  } else {
    msgInstance->responded = -1;
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_queryNotify (const char *msgName,
				 unsigned int length, BYTE_ARRAY content,
				 HANDLER_TYPE handler, void *clientData)
{
  MSG_PTR msg;
  void *queryData;
  IPC_VARCONTENT_TYPE vc;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!X_IPC_CONNECTED()) {
    RETURN_ERROR(IPC_Not_Connected);
  } else {
    msg = x_ipc_msgFind(msgName);
    if (msg == NULL) {
      RETURN_ERROR(IPC_Message_Not_Defined);
    } else if (ipcDataToSend(msg->msgData->msgFormat, msgName, 
			     length, content, &queryData, &vc) != IPC_OK) {
      PASS_ON_ERROR();
    } else {
      return ipcReturnValue(x_ipc_queryNotifySend(msg, msgName, queryData,
						  (REPLY_HANDLER_FN)handler, 
						  C_LANGUAGE, clientData));
    }
  }
}

IPC_RETURN_TYPE _IPC_queryResponse (const char *msgName, 
				    unsigned int length, BYTE_ARRAY content,
				    BYTE_ARRAY *replyHandle,
				    FORMATTER_PTR *replyFormatter,
				    unsigned int timeoutMsecs)
{
  unsigned long quitTime = WAITFOREVER, now;
  QUERY_REPLY_TYPE queryReplyData;
  QUERY_NOTIFICATION_PTR queryNotif;
  IPC_RETURN_TYPE retVal = IPC_OK;

  queryReplyData.handled = FALSE;
  queryReplyData.data = NULL;
  queryReplyData.formatter = (FORMATTER_PTR)NULL;

  if (timeoutMsecs != IPC_WAIT_FOREVER) {
    /* When to timeout */
    now = x_ipc_timeInMsecs();
    quitTime = timeoutMsecs + now;
  }

  /* Send the query, and set up a handler to catch the result and
     squirrel away the response data in "queryReplyData" */
  if (IPC_queryNotify(msgName, length, content, queryReplyHandler, 
		      &queryReplyData) == IPC_Error) {
    PASS_ON_ERROR();
  } else {
    /* Check if completed, because in threaded IPC, another thread could
       have handled the notification already */
    if (!queryReplyData.handled) {
      while ((retVal = IPC_listen(timeoutMsecs)) == IPC_OK &&
	     !queryReplyData.handled) {
	/* Handled a message, but not the one we are waiting for.
	   If still have time, reset the timeout and try again */
	if (timeoutMsecs != IPC_WAIT_FOREVER) {
	  now = x_ipc_timeInMsecs();
	  if (quitTime < now) {
	    retVal = IPC_Timeout;
	    break;
	  } else {
	    timeoutMsecs = quitTime - now;
	  }
	}
      }
    }
    if (retVal != IPC_OK) {
      /* Clean up */

      LOCK_CM_MUTEX;
      queryNotif = 
	((QUERY_NOTIFICATION_PTR)
	 x_ipc_listMemReturnItem((LIST_ITER_FN)testQueryReplyData,
				 (char *)&queryReplyData,
				 GET_C_GLOBAL(queryNotificationList)));
      x_ipc_listDeleteItem((void *)queryNotif,
			   GET_C_GLOBAL(queryNotificationList));
      UNLOCK_CM_MUTEX;
      x_ipcRefFree(queryNotif->ref);
      x_ipcFree((void *)queryNotif);
    }

#ifdef LISP
  LOCK_M_MUTEX;
  if (replyHandle == (void **)LISP_DATA_FLAG()) {
    (*GET_M_GLOBAL(lispQueryResponseGlobal))((char *)queryReplyData.data,
					     queryReplyData.formatter);
    UNLOCK_M_MUTEX;
  } else
#endif /* LISP */
    {
#ifdef LISP
      UNLOCK_M_MUTEX;
#endif /* LISP */
      *replyHandle = queryReplyData.data;
      if (replyFormatter) *replyFormatter = queryReplyData.formatter;
    }

    return retVal;
  }
}

IPC_RETURN_TYPE IPC_queryResponse (const char *msgName, 
				   unsigned int length, BYTE_ARRAY content,
				   BYTE_ARRAY *replyHandle,
				   unsigned int timeoutMsecs)
{
  return _IPC_queryResponse(msgName, length, content, 
			    replyHandle, NULL, timeoutMsecs);
}

IPC_RETURN_TYPE IPC_respondVC (MSG_INSTANCE msgInstance, const char *msgName,
			       IPC_VARCONTENT_PTR varcontent)
{
  if (!varcontent) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    return IPC_respond(msgInstance, msgName, 
		       varcontent->length, varcontent->content);
  }
}

IPC_RETURN_TYPE IPC_queryNotifyVC (const char *msgName,
				   IPC_VARCONTENT_PTR varcontent,
				   HANDLER_TYPE handler, 
				   void *clientData)
{
  if (!varcontent) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    return IPC_queryNotify(msgName, varcontent->length, varcontent->content,
			   handler, clientData);
  }
}

IPC_RETURN_TYPE IPC_queryResponseVC (const char *msgName, 
				     IPC_VARCONTENT_PTR varcontent,
				     BYTE_ARRAY *replyHandle, 
				     unsigned int timeoutMsecs)
{
  if (!varcontent) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    return _IPC_queryResponse(msgName, varcontent->length, varcontent->content,
			      replyHandle, NULL, timeoutMsecs);
  }
}

/****************************************************************
 *                INTERNAL FUNCTIONS 
 ****************************************************************/

/* When a response to the message comes in, set the fields of the
   queryReplyData to indicate that the message has been handled */

static void queryReplyHandler (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			       void *clientData)
{
  QUERY_REPLY_PTR queryReplyData = (QUERY_REPLY_PTR)clientData;

  queryReplyData->handled = TRUE;
  queryReplyData->data = callData;
  queryReplyData->formatter = IPC_msgInstanceFormatter(msgRef);
}

static BOOLEAN testQueryReplyData (QUERY_REPLY_PTR queryReply,
				   QUERY_NOTIFICATION_PTR element)
{
  return (queryReply == element->clientData);
}
