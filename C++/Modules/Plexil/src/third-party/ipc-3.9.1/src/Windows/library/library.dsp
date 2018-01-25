# Microsoft Developer Studio Project File - Name="library" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=library - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "library.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "library.mak" CFG="library - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "library - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "library - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "library - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib/win/ipc.lib"

!ELSEIF  "$(CFG)" == "library - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../ipc.lib"

!ENDIF 

# Begin Target

# Name "library - Win32 Release"
# Name "library - Win32 Debug"
# Begin Source File

SOURCE=..\..\behaviors.c
# End Source File
# Begin Source File

SOURCE=..\..\com.c
# End Source File
# Begin Source File

SOURCE=..\..\comModule.c
# End Source File
# Begin Source File

SOURCE=..\..\datamsg.c
# End Source File
# Begin Source File

SOURCE=..\..\formatters.c
# End Source File
# Begin Source File

SOURCE=..\..\globalM.c
# End Source File
# Begin Source File

SOURCE=..\..\globalMUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\hash.c
# End Source File
# Begin Source File

SOURCE=..\..\idtable.c
# End Source File
# Begin Source File

SOURCE=..\..\ipc.c
# End Source File
# Begin Source File

SOURCE=..\..\key.c
# End Source File
# Begin Source File

SOURCE=..\..\lex.c
# End Source File
# Begin Source File

SOURCE=..\..\list.c
# End Source File
# Begin Source File

SOURCE=..\..\marshall.c
# End Source File
# Begin Source File

SOURCE=..\..\modLogging.c
# End Source File
# Begin Source File

SOURCE=..\..\modVar.c
# End Source File
# Begin Source File

SOURCE=..\..\multiThread.c
# End Source File
# Begin Source File

SOURCE=..\..\parseFmttrs.c
# End Source File
# Begin Source File

SOURCE=..\..\primFmttrs.c
# End Source File
# Begin Source File

SOURCE=..\..\printData.c
# End Source File
# Begin Source File

SOURCE=..\..\queryResponse.c
# End Source File
# Begin Source File

SOURCE=..\..\reg.c
# End Source File
# Begin Source File

SOURCE=..\..\resMod.c
# End Source File
# Begin Source File

SOURCE=..\..\sendMsg.c
# End Source File
# Begin Source File

SOURCE=..\..\strList.c
# End Source File
# Begin Source File

SOURCE=..\..\tcaMem.c
# End Source File
# Begin Source File

SOURCE=..\..\tcaRef.c
# End Source File
# Begin Source File

SOURCE=..\..\tcModError.c
# End Source File
# Begin Source File

SOURCE=..\..\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\wsocksrt.c
# End Source File
# End Target
# End Project
