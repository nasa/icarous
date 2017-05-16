/*******************************************************************************
** File: interface.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _interface_h_
#define _interface_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define INTERFACE_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/
typedef enum {SOCKET, SERIAL} PortType_t;


#define TASK_1_ID         1
#define TASK_1_STACK_SIZE 1024
#define TASK_1_PRIORITY   101

uint32 task_1_stack[TASK_1_STACK_SIZE];

#define TASK_2_ID         2
#define TASK_2_STACK_SIZE 1024
#define TASK_2_PRIORITY   102
#define BUFFER_LENGTH 300

uint32 task_2_stack[TASK_2_STACK_SIZE];

uint32 task_1_id, task_2_id;

uint8 recvbuffer[BUFFER_LENGTH];

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (Interface_AppMain), these
**       functions are not called from any other source module.
*/
void INTERFACE_AppMain(void);
void INTERFACE_AppInit(void);
void InitializeSocketPort(char targetip[], int inportno, int outportno);
//void InitializeSerialPort();

void readTask(void);
void writeTask(void);
int readSocket(void);
int GetMAVLinkMsg(void);

#endif /* _interface_app_h_ */
