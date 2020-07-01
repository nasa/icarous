/*************************************************************************
** \File sbn_version.h
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   Contains CFS SBN macros that specify SBN's version. Note that back-end
**   protocol modules should identify which version they work with and should
**   verify the version is the same or otherwise alert the user.
*************************************************************************/
#ifndef _sbn_version_
#define _sbn_version_

#define SBN_MAJOR_VERSION   1
#define SBN_MINOR_VERSION   16
#define SBN_REVISION        0

#define SBN_PROTOCOL_VERSION 1
#define SBN_FILTER_VERSION 1

#endif /*_sbn_version_*/
