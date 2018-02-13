/*
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 */
#include "ipc.h"
unsigned long x_ipc_timeInMsecs(void);
#if defined(__sgi) || defined(macintosh) || defined(__APPLE__) || defined(REDHAT_71)
#include <stdlib.h>
#include <string.h>
#if defined(macintosh)
extern int fileno(FILE *);
#endif
#endif
#if !defined(__sgi) && !defined(WIN32) && !defined(REDHAT_52)
#define UNUSED_PRAGMA
#endif

/* A simple macro for converting from seconds to msecs. */
#define MSECS(seconds) (1000*(seconds))

#define TASK_NAME "test1"

#define MSG1 "msg1"
#define MSG2 "msg2"
#define MSG3 "msg3"
#define MSG4 "msg4"
#define MSG5 "msg5"

#define QUERY_MSG "query1"

#define QUERY2_MSG    "query2"
#define RESPONSE2_MSG "response2"

#define INT_FORMAT "int"
#define STRING_FORMAT "string"

typedef struct { int i1;
		 char *str1;
		 double d1;
	       } SAMPLE_TYPE, *SAMPLE_PTR;

#define SAMPLE_FORMAT "{int, string, double}"

typedef float MATRIX_TYPE[2][2];

typedef struct _matrixList { MATRIX_TYPE matrix;
			     char *matrixName;
			     int count;
			     struct _matrixList *next;
			   } MATRIX_LIST_TYPE, *MATRIX_LIST_PTR;
#define MATRIX_LIST_FORMAT "{[float:2,2], string, int, *!}"

#define MATRIX_FORMAT      "[float:2,2]"

static void msg1Handler (MSG_INSTANCE msgRef, 
			 BYTE_ARRAY callData, void *clientData)
{
  printf("msg1Handler: Receiving message %s of %d bytes (%s) [%s]\n", 
	  IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
	 (char *)callData, (char *)clientData);
  IPC_freeByteArray(callData);
}

static void msg2Handler (MSG_INSTANCE msgRef,
			 BYTE_ARRAY callData, void *clientData)
{
  printf("msg2Handler: Receiving message %s of %d bytes (%s) [%s]\n", 
	  IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
	 (char *)callData, (char *)clientData);
  IPC_freeByteArray(callData);
}

/* Unmarshall the byte array and print it out. */
static void msg3Handler (MSG_INSTANCE msgRef,
			 BYTE_ARRAY callData, void *clientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(clientData)
#endif
  FORMATTER_PTR formatter;
  void *dataptr;

  printf("msg3Handler: Receiving message %s: ", IPC_msgInstanceName(msgRef));
  /* Get the formatter for this message instance */
  formatter = IPC_msgInstanceFormatter(msgRef);
  IPC_unmarshall(formatter, callData, &dataptr);
  IPC_printData(formatter, stdout, dataptr);
  /* Free up malloc'd data */
  IPC_freeByteArray(callData);
  IPC_freeData(formatter, dataptr);
}

static void queryHandler (MSG_INSTANCE msgRef,
			  BYTE_ARRAY callData, void *clientData)
{
  char *publishStr = "Published";
  char *responseStr = "Responded";

  printf("queryHandler: Receiving message %s of %d bytes (%s) [%s]\n", 
	  IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
	 (char *)callData, (char *)clientData);
  /* Publish this message -- all subscribers get it */
  printf("\n  IPC_publish(%s, %d, %s)\n", 
	 MSG2, (int)strlen(publishStr)+1, publishStr);
  IPC_publish(MSG2, strlen(publishStr)+1, publishStr);
  /* Respond with this message -- only the query handler gets it */
  printf("\n  IPC_respond(msgRef, %s, %d, %s)\n", 
	 MSG2, (int)strlen(responseStr)+1, responseStr);
  IPC_respond(msgRef, MSG2, strlen(responseStr)+1, responseStr);
  IPC_freeByteArray(callData);
}

/* Handles the response to QUERY_MSG messages */
static void replyHandler (MSG_INSTANCE msgRef,
			  BYTE_ARRAY callData, void *clientData)
{
  printf("replyHandler: Receiving message %s (%s) [%s]\n", 
	 IPC_msgInstanceName(msgRef), (char *)callData, (char *)clientData);
  IPC_freeByteArray(callData);
}

/* Incoming message has a matrix_list format;  The response is a
   message with the first matrix, but each element incremented by one. */

static void query2Handler (MSG_INSTANCE msgRef,
			   void *callData, void *clientData)
{
  MATRIX_LIST_TYPE *matrixList = (MATRIX_LIST_TYPE *)callData;
  FORMATTER_PTR formatter;
  int i, j;

  printf("query2Handler: Receiving message %s [%s]\n", 
	 IPC_msgInstanceName(msgRef),
	 (clientData ? (char *)clientData : "NULL"));

  formatter = IPC_msgInstanceFormatter(msgRef);
  IPC_printData(formatter, stdout, matrixList);

  for (i=0; i<2; i++) for (j=0; j<2; j++) matrixList->matrix[i][j] += 1;

  printf("\n  IPC_respondData(msgRef, %s, matrixList->matrix)\n",
	 RESPONSE2_MSG);
  IPC_respondData(msgRef, RESPONSE2_MSG, &(matrixList->matrix));
  /* Free up malloc'd data */
  IPC_freeData(formatter, matrixList);
}

static unsigned long realStart;

static void timerHandler (char *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{
  printf("Timer %s: Called at %ld (%ld), scheduled at %ld, delta %ld\n",
	 string, currentTime, currentTime - realStart, scheduledTime, 
	 (currentTime - scheduledTime));
}
static void handlerChangeHandler (const char *msgName, int numHandlers, 
				  void *clientData)
{
  printf("There are now %d handlers for message %s\n", numHandlers, msgName);
}

#ifndef VXWORKS
/* Echo the input.  In addition,
 *  typing "q" will quit the program; 
 *  typing "m" will send another message; 
 *  typing "u" will unsubscribe the handler (the program will no
 *    longer listen to input)
 */
static void stdinHnd (int fd, void *clientData)
{
  char inputLine[81];
  
  fgets(inputLine, 80, stdin);

  printf("stdinHnd [%s]: Received %s", (char *)clientData, inputLine);
  fflush(stdout);

  if (strlen(inputLine) == 2) { /* Only one character and the newline */
    if (inputLine[0] == 'q') {
      IPC_disconnect();
      exit(0);
    } else if (inputLine[0] == 'm') {
      char *string = "Forwarding";
      /* Need to make sure the trailing NULL character is sent */
      printf("\n  IPC_publish(%s, %d, %s)\n", 
	     MSG2, (int)strlen(string)+1, string);
      IPC_publish(MSG2, strlen(string)+1, string);
    } else if (inputLine[0] == 'u') {
      IPC_unsubscribeFD(fd, stdinHnd);
    }
  }
}
#endif

void ipcTest1(void);
void ipcTest1(void)
{
  /* Actually send one more byte (end-of-string) */
#define FIVE_BYTE_MSG_LEN  6
#define SIX_BYTE_MSG_LEN   7
#define EIGHT_BYTE_MSG_LEN 9

  char fiveBytes[FIVE_BYTE_MSG_LEN] = "abcde";
  char sixBytes[SIX_BYTE_MSG_LEN] = "abcdef";
  char eightBytes[EIGHT_BYTE_MSG_LEN] = "abcdefgh";

  fiveBytes[FIVE_BYTE_MSG_LEN-1] = '\0';
  sixBytes[SIX_BYTE_MSG_LEN-1] = '\0';
  eightBytes[EIGHT_BYTE_MSG_LEN-1] = '\0';

  /****************************************************************
   *                TESTS OF THE BASIC IPC INTERFACE
   ****************************************************************/

  /* Connect to the central server */
  printf("\nIPC_connect(%s)\n", TASK_NAME);
  IPC_connect(TASK_NAME);

  /* Default is to exit on error; Override default, because some of the
     tests in this file explicitly induce errors. */
  printf("\nIPC_setVerbosity(IPC_Print_Errors)\n");
  IPC_setVerbosity(IPC_Print_Errors);

  /* Test out the timers */
  realStart = x_ipc_timeInMsecs();
  printf("\nIPC_addTimer(1000, TRIGGER_FOREVER, timerHandler, \"timer1\")\n");
  IPC_addTimer(1000, TRIGGER_FOREVER, (TIMER_HANDLER_TYPE)timerHandler,
	       (void *)"timer1");

  /* Define a fixed length message (no format string) */
  printf("\nIPC_defineMsg(%s, %d, NULL)\n", MSG1, FIVE_BYTE_MSG_LEN);
  IPC_defineMsg(MSG1, FIVE_BYTE_MSG_LEN, NULL);
  /* Define a variable length message (no format string) */
  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, NULL)\n", MSG2);
  IPC_defineMsg(MSG2, IPC_VARIABLE_LENGTH, NULL);

  printf("\nIPC_isMsgDefined(%s) => %s\n", 
	 MSG1, IPC_isMsgDefined(MSG1) ? "TRUE" : "FALSE");
  printf("\nIPC_isMsgDefined(%s) => %s\n", 
	 MSG3, IPC_isMsgDefined(MSG3) ? "TRUE" : "FALSE");

  /* Subscribe to the first message, with client data */
  printf("\nIPC_subscribe(%s, msg1Handler, %s)\n", MSG1, "client1a");
  IPC_subscribe(MSG1, msg1Handler, (void *)"client1a");
  /* Subscribe to the second message, with client data */
  printf("\nIPC_subscribe(%s, msg1Handler, %s)\n", MSG2, "client2a");
  IPC_subscribe(MSG2, msg1Handler, (void *)"client2a");

  /* Publish the fixed length message and listen for it (in this simple 
     example program, both publisher and subscriber are in the same process */
  printf("\nIPC_publish(%s, IPC_FIXED_LENGTH, %s)\n", MSG1, fiveBytes);
  if (IPC_publish(MSG1, IPC_FIXED_LENGTH, fiveBytes) == IPC_OK) 
    /* MSECS is a simple macro that converts from seconds to msecs */
    IPC_listenClear(MSECS(1));

  /* Publish the fixed length message, giving the length explicitly */
  /* Send one extra byte -- for end-of-string */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG1, FIVE_BYTE_MSG_LEN, fiveBytes);
  if (IPC_publish(MSG1, FIVE_BYTE_MSG_LEN, fiveBytes) == IPC_OK)
    IPC_listenClear(MSECS(1));

  /* Produces an error, since the length passed is not the defined length */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG1, 10, fiveBytes);
  if (IPC_publish(MSG1, 10, fiveBytes) == IPC_Error) printf("ERROR\n");

  /* Publish the variable length message, sending (and receiving) 5 bytes */
  /* Send one extra byte -- for end-of-string */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG2, FIVE_BYTE_MSG_LEN, fiveBytes);
  if (IPC_publish(MSG2, FIVE_BYTE_MSG_LEN, fiveBytes) == IPC_OK)
    IPC_listenClear(MSECS(1));

  /* Publish the same variable length message, this time sending 8 bytes */
  /* Send one extra byte -- for end-of-string */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG2, EIGHT_BYTE_MSG_LEN, eightBytes);
  if (IPC_publish(MSG2, EIGHT_BYTE_MSG_LEN, eightBytes) == IPC_OK)
    IPC_listenClear(MSECS(1));

  /* Produces an error: Cannot pass IPC_VARIABLE_LENGTH as an argument */
  printf("\nIPC_publish(%s, IPC_VARIABLE_LENGTH, %s)\n", MSG2, eightBytes);
  if (IPC_publish(MSG2, IPC_VARIABLE_LENGTH, eightBytes) == IPC_Error)
    printf("ERROR\n");

  /* Produces an error: "msg2" is a variable length message */
  printf("\nIPC_publish(%s, IPC_FIXED_LENGTH, %s)\n", MSG2, eightBytes);
  if (IPC_publish(MSG2, IPC_FIXED_LENGTH, eightBytes) == IPC_Error)
    printf("ERROR\n");

  /* Get notified when handlers subscribe/unsubscribe to "msg2" */
  printf("\n%d handlers currently subscribed to " MSG2, IPC_numHandlers(MSG2));
  printf("\nIPC_subscribeHandlerChange("MSG2", handlerChangeHandler, NULL)\n");
  IPC_subscribeHandlerChange(MSG2, handlerChangeHandler, NULL);

  /* Subscribe a second message handler for "msg2" */
  printf("\nIPC_subscribe(%s, msg2Handler, %s)\n", MSG2, "client2b");
  IPC_subscribe(MSG2, msg2Handler, (void *)"client2b");
  /* If doing direct broadcasts, need to listen to get the direct info update */
  IPC_listen(250);

  /* Publish the message -- receive two messages (one for msg1Handler, 
     one for msg2Handler). */
  /* Send one extra byte -- for end-of-string */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG2, EIGHT_BYTE_MSG_LEN, eightBytes);
  if (IPC_publish(MSG2, EIGHT_BYTE_MSG_LEN, eightBytes) == IPC_OK) {
    /* Make sure all the subscribers get invoked before continuing on
       (keep listening until a second has passed without any msgs) */
    while (IPC_listen(MSECS(1)) != IPC_Timeout){};
  }
  /* Remove this subscription */
  printf("\nIPC_unsubscribe(%s, msg2Handler)\n", MSG2);
  IPC_unsubscribe(MSG2, msg2Handler);
  /* If doing direct broadcasts, need to listen to get the direct info update */
  IPC_listen(250);

  /* No longer get notified when handlers are added/removed */
  printf("\nIPC_unsubscribeHandlerChange("MSG2", handlerChangeHandler)\n");
  IPC_unsubscribeHandlerChange(MSG2, handlerChangeHandler);
  printf("  IPC_subscribe(%s, msg2Handler, %s)\n", MSG2, "client2c");
  IPC_subscribe(MSG2, msg2Handler, (void *)"client2c");
  printf("  IPC_unsubscribe(%s, msg2Handler)\n", MSG2);
  IPC_unsubscribe(MSG2, msg2Handler);

  /* Publish the message -- receive one message (for msg1Handler) */
  printf("\nIPC_publish(%s, %d, %s)\n", MSG2, EIGHT_BYTE_MSG_LEN, eightBytes);
  if (IPC_publish(MSG2, EIGHT_BYTE_MSG_LEN, eightBytes) == IPC_OK)
    IPC_listenClear(MSECS(1));

  /* Subscription of the same message handler *replaces* the old client data */
  printf("\nIPC_subscribe(%s, msg1Handler, %s)\n", MSG1, "client1b");
  IPC_subscribe(MSG1, msg1Handler, (void *)"client1b");
  /* Receive one message (for msg1Handler), but now with new client data.
     Note use of IPC_publishFixed. */
  printf("\nIPC_publishFixed(%s, %s)\n", MSG1, fiveBytes);
  if (IPC_publishFixed(MSG1, fiveBytes) == IPC_OK) IPC_listenClear(MSECS(1));

  /* Remove subscription to "msg1" */
  printf("\nIPC_unsubscribe(%s, msg1Handler)\n", MSG1);
  IPC_unsubscribe(MSG1, msg1Handler);
  /* If doing direct broadcasts, need to listen to get the direct info update */
  IPC_listen(250);

  /* Receive no messages -- IPC_listenClear times out */
  printf("\nIPC_publishFixed(%s, %s)\n", MSG1, fiveBytes);
  if (IPC_publishFixed(MSG1, fiveBytes) == IPC_OK) 
    if (IPC_listen(MSECS(1)) == IPC_Timeout) printf("Timed out\n");

  /****************************************************************
   *                TESTS OF THE QUERY/RESPONSE FUNCTIONS
   ****************************************************************/

  /* The handler of QUERY_MSG does two things: It *publishes* a message of
     type MSG2, and it *responds* to the query with a message of type MSG2.
     The published message gets handled only by the subscriber (msg1Handler),
     and the response gets handled only be replyHandler, since a response
     is a directed message. */
  /* NOTE: It is perfectly OK to subscribe to a message before it is defined! */
  printf("\nIPC_subscribe(%s, queryHandler, %s)\n", QUERY_MSG, "qtest");
  IPC_subscribe(QUERY_MSG, queryHandler, (void *)"qtest");
  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, NULL)\n", QUERY_MSG);
  IPC_defineMsg(QUERY_MSG, IPC_VARIABLE_LENGTH, NULL);

  /* This call allows IPC to send the process 2 messages at a time, rather than
     queueing them in the central server.
     This is needed in this example program because the sender and receiver of
     the query are the same process.  If this is taken out, the only difference
     is that the message that is published in queryHandler arrives *after* the
     message responded to (even though it is sent first).  This function should
     not be needed when we switch to using point-to-point communications
     (rather than sending via the central server). */
  IPC_setCapacity(2);

  /* Send one extra byte -- for end-of-string */
  printf("\nIPC_queryNotify(%s, %d, %s, replyHandler, %s)\n", 
	 QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes, "Notification");
  IPC_queryNotify(QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes, replyHandler,
		  (void *)"Notification");
  /* Make sure all the messages spawned by this query get handled before
     continuing (keep listening until a second has passed without any msgs) */
  while (IPC_listen(MSECS(1)) != IPC_Timeout){};

  { char *replyHandle = NULL;
    /* This essentially does the same thing as IPC_queryNotify above, except
       it is blocking, and sets the replyHandle to be the data responded to.
       Don't need to listen, since that is done within queryResponse, but
       could be dangerous to wait forever (if the response never comes ...) */
    /* Send one extra byte -- for end-of-string */
    printf("\nIPC_queryResponse(%s, %d, %s, replyHandle, IPC_WAIT_FOREVER)\n", 
	   QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes);
    if (IPC_queryResponse(QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes,
			  (void **)(void *)&replyHandle,
			  IPC_WAIT_FOREVER) == IPC_OK) {
      printf("Blocking Response: %s\n", replyHandle);
      IPC_freeByteArray((void *)replyHandle);
    }

    /* This one should time out before the response arrives */
    /* Send one extra byte -- for end-of-string */
    printf("\nIPC_queryResponse(%s, %d, %s, replyHandle, %d)\n", 
	   QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes, 0);
    if (IPC_queryResponse(QUERY_MSG, SIX_BYTE_MSG_LEN, sixBytes,
			  (void **)(void *)&replyHandle, 0) == IPC_OK) {
      printf("Blocking Response: %s\n", replyHandle);
      IPC_freeByteArray((void *)replyHandle);
    } else {
      /* NOTE: Since the function call times out before handling messages,
       * (a) The *response* to the query is lost (for good)
       * (b) The message *published* in queryHandler is waiting for the next
       *     time the module listens for messages (which actually occurs
       *     in IPC_msgFormatter, below).
       */
      printf("queryResponse timed out (replyHandle: %ld)\n", (long)replyHandle);
    }
  }

  /****************************************************************
   *                TESTS OF THE MARSHALLING FUNCTIONS
   ****************************************************************/

  /* Test the marshalling/unmarshalling functions, independently of
   *  sending/receiving messages
   */
  { IPC_VARCONTENT_TYPE varcontent;
    SAMPLE_TYPE sample;
    SAMPLE_PTR  sample2Ptr;

    sample.i1 = 666; sample.str1 = "hello, world"; sample.d1 = 3.14159;

    printf("\nIPC_marshall(...)\n");
    IPC_marshall(IPC_parseFormat(SAMPLE_FORMAT), &sample, &varcontent);
    printf("Marshall of 'sample' [length: %d]\n", varcontent.length);
    
    printf("\nIPC_unmarshall(...)\n");
    IPC_unmarshall(IPC_parseFormat(SAMPLE_FORMAT), 
		   varcontent.content, (void **)(void *)&sample2Ptr);
      
    printf("Orig: <%d, %s, %f>\nCopy: <%d, %s, %f>\n",
	    sample.i1, sample.str1, sample.d1,
	    sample2Ptr->i1, sample2Ptr->str1, sample2Ptr->d1);
    IPC_freeByteArray(varcontent.content);
    IPC_freeData(IPC_parseFormat(SAMPLE_FORMAT), sample2Ptr);
  }

  { IPC_VARCONTENT_TYPE varcontent;
    MATRIX_LIST_TYPE m1, m2, m3;
    int i, j, k;
    char *string;
    
    /* Define a variable-length message whose format is simply an integer */
    printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", MSG3, INT_FORMAT);
    IPC_defineMsg(MSG3, IPC_VARIABLE_LENGTH, INT_FORMAT);
    /* Define a variable-length message whose format is a string */
    printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", 
	   MSG4, STRING_FORMAT);
    IPC_defineMsg(MSG4, IPC_VARIABLE_LENGTH, STRING_FORMAT);
    /* Define a variable-length message whose format is a complex structure */
    printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", 
	   MSG5, MATRIX_LIST_FORMAT);
    IPC_defineMsg(MSG5, IPC_VARIABLE_LENGTH, MATRIX_LIST_FORMAT);

    /* Subscribe to each of the above messages, all using the same handler */
    printf("\nIPC_subscribe(%s, msg3Handler, NULL)\n", MSG3);
    IPC_subscribe(MSG3, msg3Handler, NULL);
    printf("\nIPC_subscribe(%s, msg3Handler, NULL)\n", MSG4);
    IPC_subscribe(MSG4, msg3Handler, NULL);
    printf("\nIPC_subscribe(%s, msg3Handler, NULL)\n", MSG5);
    IPC_subscribe(MSG5, msg3Handler, NULL);

    /* Marshall the integer into a byte array (takes byte-order into account) */
    i = 42;
    printf("\nIPC_marshall(...)\n");
    IPC_marshall(IPC_msgFormatter(MSG3), &i, &varcontent);
    /* Publish the marshalled byte array (message handler prints the data) */
    printf("\nIPC_publishVC(%s, varcontent[%d])\n", MSG3, varcontent.length);
    IPC_publishVC(MSG3, &varcontent);
    IPC_freeByteArray(varcontent.content);
    IPC_listenClear(MSECS(1));

    /* Marshall the string into a byte array. NOTE: Need to pass a *pointer*
       to the string! */
    string = "Hello, world";
    /* It's much better (safer) to use IPC_msgFormatter, but this is included
       just to illustrate the use of IPC_parseFormat */
    printf("\nIPC_marshall(...)\n");
    IPC_marshall(IPC_parseFormat(STRING_FORMAT), &string, &varcontent);
    /* Publish the marshalled byte array (message handler prints the data) */
    printf("\nIPC_publishVC(%s, varcontent[%d])\n", MSG4, varcontent.length);
    IPC_publishVC(MSG4, &varcontent);
    IPC_freeByteArray(varcontent.content);
    IPC_listenClear(MSECS(1));

    /* Set up a sample MATRIX_LIST structure */
    for (k=0, i=0; i<2; i++) for (j=0; j<2; j++) m1.matrix[i][j] = (i+j+k);
    m1.matrixName = "TheFirst"; m1.count = k; m1.next = &m2;

    for (k++, i=0; i<2; i++) for (j=0; j<2; j++) m2.matrix[i][j] = (i+j+k);
    m2.matrixName = "TheSecond"; m2.count = k; m2.next = &m3;

    for (k++, i=0; i<2; i++) for (j=0; j<2; j++) m3.matrix[i][j] = (i+j+k);
    m3.matrixName = "TheThird"; m3.count = k; m3.next = NULL;

    /* IPC_publishData both marsalls and publishes the data structure */
    printf("\nIPC_publishData(%s, m1)\n", MSG5);
    IPC_publishData(MSG5, &m1);
    IPC_listenClear(MSECS(1));
  }

  /* Use of IPC_queryResponseData and IPC_respondData -- 
     Send out a message with a matrix_list format;  The response is a
     message with the first matrix, but each element incremented by one. */

  { MATRIX_LIST_TYPE m1;
    MATRIX_TYPE *matrixPtr;
    int i, j, k;
    
    /* Define the "query" message */
    printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", 
	   QUERY2_MSG, MATRIX_LIST_FORMAT);
    IPC_defineMsg(QUERY2_MSG, IPC_VARIABLE_LENGTH, MATRIX_LIST_FORMAT);
    /* Define the "response" message */
    printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", 
	   RESPONSE2_MSG, MATRIX_FORMAT);
    IPC_defineMsg(RESPONSE2_MSG, IPC_VARIABLE_LENGTH, MATRIX_FORMAT);

    /* Subscribe to query message with automatic unmarshalling */
    printf("\nIPC_subscribeData(%s, query2Handler, NULL)\n", QUERY2_MSG);
    IPC_subscribeData(QUERY2_MSG, query2Handler, NULL);

    /* Set up a sample MATRIX_LIST structure */
    for (k=0, i=0; i<2; i++) for (j=0; j<2; j++) m1.matrix[i][j] = (i+j+k);
    m1.matrixName = "TheFirst"; m1.count = k; m1.next = NULL;

    /* IPC_queryResponseData both marsalls and sends the data structure */
    printf("\nIPC_queryResponseData(%s, m1, matrixPtr, IPC_WAIT_FOREVER)\n", 
	   QUERY2_MSG);
    if (IPC_queryResponseData(QUERY2_MSG, (void *)&m1, 
			      (void **)(void *)&matrixPtr, 
			      IPC_WAIT_FOREVER) == IPC_OK) {
      IPC_printData(IPC_msgFormatter(RESPONSE2_MSG), stdout, matrixPtr);
    } else {
      printf("IPC_queryResponseData failed\n");
    }
  }

#if !defined(VXWORKS) && !defined(_WINSOCK_)
 /* Don't do this for vxworks, since it does not handle stdin from the 
    terminal, nor for winsock, since it does not seem to be able to use
    select on a non-socket fd */

  /* Subscribe a handler for tty input.  Now, typing at the terminal will
      echo the input.  Typing "q" will quit the program; typing "m" will
      send a message; typing "u" will unsubscribe the handler (the program
      will no longer listen to input). */
  printf("\nIPC_subscribeFD(%d, stdinHnd, %s)\n", fileno(stdin), "FD1");
  IPC_subscribeFD(fileno(stdin), stdinHnd, (void *)"FD1");
  printf("\nEntering dispatch loop (terminal input is echoed, type 'q' to quit,\n");
  printf("  'm' to send a message, 'u' to stop listening to stdin).\n");
  IPC_dispatch();
#endif

  /* If ever reaches here, shut down gracefully */
  IPC_disconnect();
}

#ifndef VXWORKS
int main (void)
{
  ipcTest1();
  return 0;
}
#endif
