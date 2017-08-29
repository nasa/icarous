/*
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 */
#include <pthread.h>
#include "ipc.h"
#if defined(__APPLE__) || defined(REDHAT_71)
#include <unistd.h>
#endif

#define TASK_NAME "multiTest"

#define QUERY1_MSG    "query1"
#define RESPONSE1_MSG "response1"

#define QUERY2_MSG    "query2"
#define RESPONSE2_MSG "response2"

#define BROADCAST_MSG "msg1"

typedef struct { int i1;
		 char *str1;
		 double d1;
	       } SAMPLE_TYPE, *SAMPLE_PTR;

#define SAMPLE_FORMAT "{int, string, double}"

static void broadcastHandler (MSG_INSTANCE msgRef, 
			      void * callData, void *clientData)
{
  fprintf(stderr, "broadcastHandler: Receiving broadcast message: ");
  IPC_printData(IPC_msgInstanceFormatter(msgRef), stderr, callData);
  IPC_freeData(IPC_msgInstanceFormatter(msgRef), callData);
}

static void query1Handler (MSG_INSTANCE msgRef, 
			   void * callData, void *clientData)
{
  fprintf(stderr, "query1Handler: Receiving query1 message: ");
  IPC_printData(IPC_msgInstanceFormatter(msgRef), stderr, callData);
  IPC_respondData(msgRef, RESPONSE1_MSG, callData);
  IPC_freeData(IPC_msgInstanceFormatter(msgRef), callData);
}

static void query2Handler (MSG_INSTANCE msgRef, 
			   void * callData, void *clientData)
{
  fprintf(stderr, "query2Handler: Receiving query2 message: ");
  IPC_printData(IPC_msgInstanceFormatter(msgRef), stderr, callData);
  IPC_respondData(msgRef, RESPONSE2_MSG, callData);
  IPC_freeData(IPC_msgInstanceFormatter(msgRef), callData);
}

static void *thread1Fn (void *dummy)
{
  SAMPLE_TYPE query1 = {0, "query1", 0.0}, *response1;
  int count = 0;

  while (1) {
    fprintf(stderr, "THREAD1 (%d)\n", ++count);
    IPC_queryResponseData(QUERY1_MSG, &query1, (void **)(void *)&response1,
			  IPC_WAIT_FOREVER);
    fprintf(stderr, "  RESP1 (%d): ", count);
    IPC_printData(IPC_parseFormat(SAMPLE_FORMAT), stderr, response1);
    IPC_freeData(IPC_parseFormat(SAMPLE_FORMAT), response1);
    query1.i1++;    query1.d1--;
  }
  return dummy;
}

static void *thread2Fn (void *dummy)
{
  SAMPLE_TYPE query2 = {0, "query2", 0.0}, *response2;
  int count = 0;

  while (1) {
    fprintf(stderr, "THREAD2 (%d)\n", ++count);
    IPC_queryResponseData(QUERY2_MSG, &query2, (void **)(void *)&response2,
			  IPC_WAIT_FOREVER);
    fprintf(stderr, "  RESP2 (%d): ", count);
    IPC_printData(IPC_parseFormat(SAMPLE_FORMAT), stderr, response2);
    IPC_freeData(IPC_parseFormat(SAMPLE_FORMAT), response2);
    query2.i1 += 2;    query2.d1 -= 2;
  }
  return dummy;
}

static void *thread3Fn (void *dummy)
{
  char string[20], *stringPtr;
  int count = 0;

  stringPtr = string;
  while (1) {
    fprintf(stderr, "THREAD3 (%d)\n", ++count);
    sprintf(string, "Broadcasts #%d", count);
    IPC_publishData(BROADCAST_MSG, &stringPtr);
    usleep(20000);
  }
  return dummy;
}

int main (void)
{
  pthread_t thread1, thread2, thread3;

  printf("\nIPC_connect(%s)\n", TASK_NAME);
  IPC_connect(TASK_NAME);

  IPC_defineMsg(QUERY1_MSG, IPC_VARIABLE_LENGTH, SAMPLE_FORMAT);
  IPC_defineMsg(RESPONSE1_MSG, IPC_VARIABLE_LENGTH, SAMPLE_FORMAT);
  IPC_defineMsg(QUERY2_MSG, IPC_VARIABLE_LENGTH, SAMPLE_FORMAT);
  IPC_defineMsg(RESPONSE2_MSG, IPC_VARIABLE_LENGTH, SAMPLE_FORMAT);
  IPC_defineMsg(BROADCAST_MSG, IPC_VARIABLE_LENGTH, "string");
 
  IPC_subscribeData(QUERY1_MSG, query1Handler, NULL);
  IPC_subscribeData(QUERY2_MSG, query2Handler, NULL);
  IPC_subscribeData(BROADCAST_MSG, broadcastHandler, NULL);

  /* This may be needed, if you have a slow machine */
  /* IPC_setMsgQueueLength(BROADCAST_MSG, 1);*/

  /* Spawn three threads */
  pthread_create(&thread1, NULL, thread1Fn, NULL);
  pthread_create(&thread2, NULL, thread2Fn, NULL);
  pthread_create(&thread3, NULL, thread3Fn, NULL);

  IPC_dispatch();

  return 0;
}
