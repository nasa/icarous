/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: ipc.h
 *
 * ABSTRACT: External header file for IPC, defining API and 
 *           predefined constants.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipc.h,v $
 * Revision 2.13  2011/04/21 18:17:48  reids
 * IPC 3.9.0:
 * Added NoListen options to IPC_connect, to indicate that module will not
 *   periodically listen for messages.
 * Bug where having a message id of 0 or 1 interfaces with direct message
 *   functionality.
 * Extended functionality of "ping" to handle race condition with concurrent
 *   listens.
 * Fixed bug in how IPC_listenWait was implemented (did not necessarily
 *   respect the timeout).
 * Fixed conditions under which module listens for handler updates.
 *
 * Revision 2.12  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.11  2008/07/16 00:09:03  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.10  2001/05/30 19:36:18  reids
 * Added conditional flag to re-enable compilation under Windows.
 *
 * Revision 2.9  2001/03/06 00:20:16  trey
 * added IPC_addTimerGetRef() and IPC_removeTimerByRef() functions
 *
 * Revision 2.8  2001/02/09 16:24:21  reids
 * Added IPC_getConnections to return list of ports that IPC is listening to.
 * Added IPC_freeDataElements to free the substructure (pointers) of a struct.
 *
 * Revision 2.7  2001/01/31 17:54:12  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.6  2001/01/12 15:53:45  reids
 * Added IPC_delayResponse to enable responding to a query outside of the
 *   message handler.
 *
 * Revision 2.5  2001/01/10 15:32:50  reids
 * Added the function IPC_subscribeData that automatically unmarshalls
 *   the data before invoking the handler.
 *
 * Revision 2.4  2000/07/27 16:59:10  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.3  2000/07/19 20:56:19  reids
 * Added IPC_listenWait
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.5.2.8  1997/02/26 04:17:53  reids
 * Added IPC_isMsgDefined.
 *
 * Revision 1.5.2.7  1997/01/27 20:40:30  reids
 * Implement a function to check whether a given format string matches the
 *   one registered for a given message.
 *
 * Revision 1.5.2.6  1996/12/18 15:27:21  reids
 * Renamed EXTERN_FUNCTION to IPC_EXTERN_FUNCTION to avoid name conflicts
 *
 * Revision 1.5.2.5  1996/11/22 20:18:10  rouquett
 * forgot that semicolon in the c++ EXTERN_FUNCTION
 *
 * Revision 1.5.2.4  1996/11/22 19:03:42  rouquett
 * get rid of the annoying warnings with c++
 *
 * Revision 1.5.2.3  1996/10/28 22:25:15  reids
 * Added IPC_unmarshallData.
 *
 * Revision 1.5.2.2  1996/10/18 18:06:54  reids
 * Better error checking and reporting (IPC_errno, IPC_perror).
 * Added IPC_initialize so that you don't have to connect with central to
 *   do marshalling.
 * Added IPC_setVerbosity to set the level at which messages are reported.
 *
 * Revision 1.5.2.1  1996/10/14 03:54:42  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.5  1996/05/26 04:11:48  reids
 * Added function IPC_dataLength -- length of byte array assd with msgInstance
 *
 * Revision 1.4  1996/05/09 01:01:32  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/03 02:45:48  reids
 * Made ipc.h C++ compatible.
 * Made IPC_listenClear wait a bit before returning, to catch pending msgs.
 * Made IPC_connect try a few times to connect with the central server.
 *
 * Revision 1.2  1996/03/06 20:20:44  reids
 * Version 2.3 adds two new functions: IPC_defineFormat and IPC_isConnected
 *
 * Revision 1.1  1996/03/03 04:36:19  reids
 * First release of IPC files.  Corresponds to IPC Specifiction 2.2, except
 * that IPC_readData is not yet implemented.  Also contains "cover" functions
 * for the xipc interface.
 *
 ****************************************************************/

#ifndef _IPC_H
#define _IPC_H

#include <stdio.h>

/*****************************************************************
 *                DEFINES -- CONSTANTS AND MACROS
 *****************************************************************/

#define IPC_WAIT_FOREVER (0xFFFFFFFF) /* Same as X_IPC's WAITFOREVER */

/* Using MAXINT directly produces a compiler warning */
#define IPC_VARIABLE_LENGTH (0xFFFFFFFF)
#define IPC_FIXED_LENGTH    (IPC_VARIABLE_LENGTH-1)

/*****************************************************************
 *                EXTERNAL TYPE DEFINITIONS
 *****************************************************************/

#ifdef macintosh
#pragma export on
#endif

typedef enum { IPC_Error, IPC_OK, IPC_Timeout
	     } IPC_RETURN_TYPE;

typedef enum { IPC_No_Error, IPC_Not_Connected, IPC_Not_Initialized,
	       IPC_Message_Not_Defined,
	       IPC_Not_Fixed_Length, IPC_Message_Lengths_Differ,
	       IPC_Argument_Out_Of_Range, IPC_Null_Argument,
	       IPC_Illegal_Formatter, IPC_Mismatched_Formatter,
	       IPC_Wrong_Buffer_Length, IPC_Communication_Error
	     } IPC_ERROR_TYPE;

#define NUM_ERRORS (1+(int)IPC_Communication_Error)

typedef enum { IPC_Silent, IPC_Print_Warnings, IPC_Print_Errors,
		 IPC_Exit_On_Errors
	     } IPC_VERBOSITY_TYPE;

typedef void *BYTE_ARRAY;

typedef struct { unsigned int length;
		 BYTE_ARRAY content;
	       } IPC_VARCONTENT_TYPE, *IPC_VARCONTENT_PTR;

typedef struct _X_IPC_REF *MSG_INSTANCE;

typedef struct _X_IPC_CONTEXT *IPC_CONTEXT_PTR;

typedef const struct _FORMAT_TYPE *FORMATTER_PTR;

typedef void (*HANDLER_TYPE)(MSG_INSTANCE msgInstance, BYTE_ARRAY callData,
			     void *clientData);

typedef void (*HANDLER_DATA_TYPE)(MSG_INSTANCE msgInstance, void *callData,
				  void *clientData);

typedef void (*FD_HANDLER_TYPE)(int fd, void *clientData);

typedef void (*CONNECT_HANDLE_TYPE)(const char *moduleName, void *clientData);

typedef void (*CHANGE_HANDLE_TYPE)(const char *msgName, int numHandlers, 
				   void *clientData);

/*****************************************************************
 *                EXTERNAL GLOBAL VARIABLES
 *****************************************************************/

extern IPC_ERROR_TYPE IPC_errno;

/*****************************************************************
 *                PUBLIC FUNCTIONS
 *****************************************************************/

/*****************************************************************
 *                BASIC IPC INTERFACE
 *****************************************************************/

/*  EXTERN_FUNCTION( rtn name, (arg types) );
 *	Macro to make external function declarations portable between 
 *      the major C dialects: C, ANSI C, and C++.
 */

#if defined(__cplusplus) /* C++ */
#define IPC_EXTERN_FUNCTION( rtn, args ) rtn args
#elif defined(__STDC__) || defined(__GNUC__) /* ANSI C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn args
#elif defined(__TURBOC__) /* ANSI C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn args
#else /* "Regular" C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn()
#endif

#if defined(__cplusplus) /* C++ */
extern "C" {
#endif

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_initialize,
		     (void));

/* Added by TNgo, 5/22/97 */
IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_connectModule,
		     (const char *taskName, const char *serverName));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_connect,
		     (const char *taskName));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_connectModuleNoListen,
		     (const char *taskName, const char *serverName));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_connectNoListen,
		     (const char *taskName));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_disconnect,
		     (void));

/* Returns TRUE (1) if the module is currently connected to the IPC server */
IPC_EXTERN_FUNCTION (int IPC_isConnected,
		     (void));

/* Returns TRUE (1) if the named module is currently connected to IPC */
/* Returns -1 on error */
IPC_EXTERN_FUNCTION (int IPC_isModuleConnected,
		     (const char *moduleName));

#if defined(WIN32)
#include <winsock.h>
#else
#include <sys/select.h>
#endif

/* Returns the fd_set mask of the current IPC socket connections */
IPC_EXTERN_FUNCTION (fd_set IPC_getConnections, (void));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_defineMsg,
		     (const char *msgName, 
		      unsigned int length,
		      const char *formatString));

IPC_EXTERN_FUNCTION (int IPC_isMsgDefined,
		     (const char *msgName));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_publish,
		     (const char *msgName,
		      unsigned int length,
		      BYTE_ARRAY content));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_publishVC,
		     (const char *msgName,
		      IPC_VARCONTENT_PTR varcontent));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_publishFixed,
		     (const char *msgName,
		      BYTE_ARRAY content));

IPC_EXTERN_FUNCTION (const char *IPC_msgInstanceName,
		     (MSG_INSTANCE msgInstance));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribe,
		     (const char *msgName,
		      HANDLER_TYPE handler,
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribeData,
		     (const char *msgName,
		      HANDLER_DATA_TYPE handler,
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unsubscribe,
		     (const char *msgName, 
		      HANDLER_TYPE handler));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribeFD,
		     (int fd, 
		      FD_HANDLER_TYPE handler,
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unsubscribeFD,
		     (int fd,
		      FD_HANDLER_TYPE handler));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_listen,
		     (unsigned int timeoutMSecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_listenClear,
		     (unsigned int timeoutMSecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_listenWait,
		     (unsigned int timeoutMSecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_handleMessage,
		     (unsigned int timeoutMSecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_dispatch,
		     (void));

IPC_EXTERN_FUNCTION (unsigned int IPC_dataLength,
		     (MSG_INSTANCE msgInstance));

IPC_EXTERN_FUNCTION (void IPC_perror,
		     (const char *msg));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_setCapacity,
		     (int capacity));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_setMsgQueueLength,
		     (const char *msgName, int queueLength));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_setMsgPriority,
		     (const char *msgName, int priority));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_setVerbosity,
		     (IPC_VERBOSITY_TYPE verbosity));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribeConnect,
		     (CONNECT_HANDLE_TYPE handler, void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribeDisconnect,
		     (CONNECT_HANDLE_TYPE handler, void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unsubscribeConnect,
		     (CONNECT_HANDLE_TYPE handler));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unsubscribeDisconnect,
		     (CONNECT_HANDLE_TYPE handler));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_subscribeHandlerChange,
		     (const char *msgName, CHANGE_HANDLE_TYPE handler,
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unsubscribeHandlerChange,
		     (const char *msgName, CHANGE_HANDLE_TYPE handler));

IPC_EXTERN_FUNCTION (int IPC_numHandlers, (const char *msgName));

/*****************************************************************
 *                QUERY/RESPONSE FUNCTIONS
 *****************************************************************/

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_respond,
		     (MSG_INSTANCE msgInstance,
		      const char *msgName,
		      unsigned int length,
		      BYTE_ARRAY content));

/* If the response (IPC_respond) happens outside of the handler, 
   must call this function from within the handler!! */
IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_delayResponse,
		     (MSG_INSTANCE msgInstance));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryNotify,
		     (const char *msgName,
		      unsigned int length,
		      BYTE_ARRAY content,
		      HANDLER_TYPE handler, 
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryResponse,
		     (const char *msgName, 
		      unsigned int length,
		      BYTE_ARRAY content,
		      BYTE_ARRAY *replyHandle, 
		      unsigned int timeoutMsecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_respondVC,
		     (MSG_INSTANCE msgInstance,
		      const char *msgName,
		      IPC_VARCONTENT_PTR varcontent));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryNotifyVC,
		     (const char *msgName,
		      IPC_VARCONTENT_PTR varcontent,
		      HANDLER_TYPE handler, 
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryResponseVC,
		     (const char *msgName, 
		      IPC_VARCONTENT_PTR varcontent,
		      BYTE_ARRAY *replyHandle, 
		      unsigned int timeoutMsecs));

/*****************************************************************
 *                MARSHALLING FUNCTIONS
 *****************************************************************/

IPC_EXTERN_FUNCTION (FORMATTER_PTR IPC_parseFormat,
		     (const char *formatString));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_defineFormat,
		     (const char *formatName,
		      const char *formatString));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_checkMsgFormats,
		     (const char *msgName,
		      const char *formatString));

IPC_EXTERN_FUNCTION (FORMATTER_PTR IPC_msgFormatter,
		     (const char *msgName));

IPC_EXTERN_FUNCTION (FORMATTER_PTR IPC_msgInstanceFormatter,
		     (MSG_INSTANCE msgInstance));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_marshall,
		     (FORMATTER_PTR formatter,
		      void *dataptr, 
		      IPC_VARCONTENT_PTR varcontent));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unmarshall,
		     (FORMATTER_PTR formatter,
		      BYTE_ARRAY byteArray, 
		      void **dataHandle));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_unmarshallData,
		     (FORMATTER_PTR formatter,
		      BYTE_ARRAY byteArray,
		      void *dataHandle,
		      int dataSize));

IPC_EXTERN_FUNCTION (void IPC_freeByteArray,
		     (BYTE_ARRAY byteArray));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_freeData,
		     (FORMATTER_PTR formatter,
		      void *dataptr));

/* Frees any subelements (pointers, variable-length arrays), but does not 
   free the top level data structure itself.  Useful with IPC_unmarshallData
 */
IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_freeDataElements,
		     (FORMATTER_PTR formatter,
		      void *dataptr));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_publishData,
		     (const char *msgName,
		      void *dataptr));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_respondData,
		     (MSG_INSTANCE msgInstance,
		      const char *msgName,
		      void *dataptr));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryNotifyData,
		     (const char *msgName,
		      void *dataptr,
		      HANDLER_TYPE handler, 
		      void *clientData));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_queryResponseData,
		     (const char *msgName, 
		      void *dataptr,
		      void **replyData, 
		      unsigned int timeoutMsecs));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_printData,
		     (FORMATTER_PTR formatter,
		      FILE *stream, 
		      void *dataptr));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_readData,
		     (FORMATTER_PTR formatter,
		      FILE *stream, 
		      void **dataHandle));

/* Added by TNgo, 5/14/98, to support multiple servers. */
/*****************************************************************
 *                     CONTEXT FUNCTIONS
 *****************************************************************/

IPC_EXTERN_FUNCTION (IPC_CONTEXT_PTR IPC_getContext,
		     (void));

IPC_EXTERN_FUNCTION (IPC_RETURN_TYPE IPC_setContext,
		     (IPC_CONTEXT_PTR context));

/*****************************************************************
 *                     TIMER FUNCTIONS
 *****************************************************************/

#define TRIGGER_FOREVER (-1)

typedef void (*TIMER_HANDLER_TYPE)(void *clientData,
				   unsigned long currentTime, 
				   unsigned long scheduledTime);
typedef void *TIMER_REF;

IPC_RETURN_TYPE IPC_addTimer(unsigned long tdelay, long count,
			     TIMER_HANDLER_TYPE handler, void *clientData);

IPC_RETURN_TYPE IPC_addTimerGetRef(unsigned long tdelay, long count,
				   TIMER_HANDLER_TYPE handler,
				   void *clientData, TIMER_REF *timerRef);

IPC_RETURN_TYPE IPC_addOneShotTimer(long tdelay, TIMER_HANDLER_TYPE handler,
				    void *clientData);

IPC_RETURN_TYPE IPC_addPeriodicTimer(long tdelay, TIMER_HANDLER_TYPE handler,
				     void *clientData);

IPC_RETURN_TYPE IPC_removeTimer(TIMER_HANDLER_TYPE handler);

IPC_RETURN_TYPE IPC_removeTimerByRef(TIMER_REF timerRef);

#if defined(__cplusplus) /* C++ */
}
#endif

#ifdef macintosh
#pragma export off
#endif

#endif /* _IPC_H */
