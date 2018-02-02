/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.2 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 *
 * (c) Copyright 2001 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCRefCounter_h
#define INCRefCounter_h

class RefCounter {
public:
  void ref(void) { refCount++; }
  void unref(void) {
    refCount--;
    if (refCount <= 0) delete this;
  }
  void checkRef(void) {
    if (refCount <= 0) delete this;
  }

protected:
  RefCounter(void) { refCount = 0; }
  // must override default copy constructor!
  RefCounter(RefCounter &r) { refCount = 0; }
  virtual ~RefCounter(void) { }
  int refCount;
};

#endif // INCRefCounter_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: RefCounter.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:01  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/05 21:10:46  trey
 * initial check-in
 *
 *
 ***************************************************************************/
