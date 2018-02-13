/*****************************************************************************
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 *
 * FUNCTION: startWinsock()
 *
 * DESCRIPTION: We must start up the Winsock before making any socket requests
 *              Begin winsock startup.
 *
 *****************************************************************************/

#include "globalM.h"

#ifdef _WINSOCK_
void startWinsock (void)
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  
  wVersionRequested = MAKEWORD(1, 1);
  err = WSAStartup(wVersionRequested, &wsaData);
  if(err!=0) {
    switch (err) {
    case WSASYSNOTREADY:
      printf("Network subsystem not ready for network communication\n");
      printf("Cannot continue\n");
      break;
    case WSAVERNOTSUPPORTED:
      printf("You do not have a version 1.1 Winsock or greater\n");
      printf("Cannot continue\n");
      break;
    case WSAEINVAL:
      printf("Your winsock.dll is not 100%% Winsock compatible\n");
      printf("Cannot continue\n");
      break;
    default:
      printf("Unknown Winsock start up error #%d",err);
      printf("Cannot continue\n");
    }
    return;
  }
  
  if (LOBYTE(wVersionRequested) != 1 || HIBYTE(wVersionRequested) != 1) {
    printf("Your winsock.dll does not support version 1.1");
    printf("Cannot continue\n");
    WSACleanup();
    printf("Socket cleaned up.");
    return;
  }
  
  /* End winsock startup */
  printf("Winsock successfully loaded\n");
  /* If we made it here, version 1.1 is supported and has been loaded */
}

#else /* !_WINSOCK_ */

void startWinsock (void)
{
  fprintf(stderr,
	  "\nWhat you want with WINSOCK when you haven't compiled for it?");
  fflush(stderr);
}

#endif /* !_WINSOCK_ */
