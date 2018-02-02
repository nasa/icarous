/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: priorityTest.c
 *
 * ABSTRACT: Test the prioritized messages feature of IPC 
 *           (only works for centrally-routed messages).
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: priorityTest.c,v $
 * Revision 2.4  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2008/07/16 00:08:48  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.2  2000/01/27 20:47:11  reids
 * Removed compiler warnings for RedHat Linux
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ipc.h"
#if !defined(__sgi) && !defined(WIN32) && !defined(REDHAT_52)
#define UNUSED_PRAGMA
#endif

#define TASK_NAME "priority-test"

#define MSG0 "msg0"
#define MSG1 "msg1"
#define MSG2 "msg2"
#define MSG3 "msg3"
#define MSG4 "msg4"
#define MSG5 "msg5"

#define MSG2_PRIORITY 2
#define MSG3_PRIORITY 3
#define MSG4_PRIORITY 4
#define MSG5_PRIORITY 5

#define MSG_FORMAT "string"

static void msgHandler (MSG_INSTANCE msgRef, 
			BYTE_ARRAY callData, void *clientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(clientData)
#endif
  char **strPtr;

  IPC_unmarshall(IPC_msgInstanceFormatter(msgRef), callData,
		 (void **)(void *)&strPtr);
  printf("Handling %s: Receiving %s\n", IPC_msgInstanceName(msgRef), *strPtr);
  free(callData);
  free(*strPtr); free(strPtr);
}


#if defined(VXWORKS)
void priorityTest(void)
#else
int main (void)
#endif
{
  char *str;

  /* Connect to the central server */
  IPC_connect(TASK_NAME);

  /* Define the messages */
  IPC_defineMsg(MSG0, IPC_VARIABLE_LENGTH, MSG_FORMAT);
  IPC_defineMsg(MSG1, IPC_VARIABLE_LENGTH, MSG_FORMAT);
  IPC_defineMsg(MSG2, IPC_VARIABLE_LENGTH, MSG_FORMAT);
  IPC_defineMsg(MSG3, IPC_VARIABLE_LENGTH, MSG_FORMAT);
  IPC_defineMsg(MSG4, IPC_VARIABLE_LENGTH, MSG_FORMAT);
  IPC_defineMsg(MSG5, IPC_VARIABLE_LENGTH, MSG_FORMAT);

  /* Subscribe to them all */
  IPC_subscribe(MSG0, msgHandler, NULL);
  IPC_subscribe(MSG1, msgHandler, NULL);
  IPC_subscribe(MSG2, msgHandler, NULL);
  IPC_subscribe(MSG3, msgHandler, NULL);
  IPC_subscribe(MSG4, msgHandler, NULL);
  IPC_subscribe(MSG5, msgHandler, NULL);

  /* Set the priorities (msg0 and msg1 are "default" -- lowest) */
  IPC_setMsgPriority(MSG2, MSG2_PRIORITY);
  IPC_setMsgPriority(MSG3, MSG3_PRIORITY);
  IPC_setMsgPriority(MSG4, MSG4_PRIORITY);
  IPC_setMsgPriority(MSG5, MSG5_PRIORITY);

  /* Publish first in "normal" order -- mainly to cache msg definitions */
  str = "Ask not what"; IPC_publishData(MSG5, &str);
  str = "your country can"; IPC_publishData(MSG4, &str);
  str = "do for you."; IPC_publishData(MSG3, &str);
  str = "Ask what you can do"; IPC_publishData(MSG2,&str );
  str = "for your country."; IPC_publishData(MSG1, &str);

  /* Publish msg0 to make the module active */
  str = "Start here"; IPC_publishData(MSG0, &str);

  /* Publish in "random" order -- but they should get received in order 5-1 */
  str = "Ask not what"; IPC_publishData(MSG5, &str);
  str = "your country can"; IPC_publishData(MSG4, &str);
  str = "for your country."; IPC_publishData(MSG1, &str);
  str = "do for you."; IPC_publishData(MSG3, &str);
  str = "Ask what you can do"; IPC_publishData(MSG2,&str );

  printf("\n");
  IPC_dispatch();

  /* Should never reach here, but just in case */
  IPC_disconnect();
#if !defined(VXWORKS)
  return 0;
#endif
}
