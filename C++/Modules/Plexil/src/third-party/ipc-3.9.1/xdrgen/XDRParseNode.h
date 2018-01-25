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

#ifndef INCXDRParseNode_h
#define INCXDRParseNode_h

struct XDRParseNode : public RefCounter {
  virtual ostream &outputToStream(ostream &out);
  friend ostream &operator <<(ostream &os, const XDRParseNode &node);
};

#endif // INCXDRParseNode_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRParseNode.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:04  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/05 21:10:50  trey
 * initial check-in
 *
 *
 ***************************************************************************/
