cmake_minimum_required(VERSION 3.7)
project(ipc)


set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

include_directories(${PLEXIL_ROOT}/include)

set(SOURCE_FILES
        src/basics.h
        src/behaviors.c
        src/behaviors.h
        src/centralIO.c
        src/centralIO.h
        src/centralMsg.h
        src/com.c
        src/com.h
        src/comModule.c
        src/comModule.h
        src/comServer.c
        src/comServer.h
        src/datamsg.c
        src/datamsg.h
        src/dispatch.c
        src/dispatch.h
        src/formatters.c
        src/formatters.h
        src/globalM.c
        src/globalM.h
        src/globalMUtil.c
        src/globalS.c
        src/globalS.h
        src/globalVar.c
        src/globalVar.h
        src/hash.c
        src/hash.h
        src/idtable.c
        src/idtable.h
        src/ipc.c
        src/ipc.h
        src/ipcFFI.c
        src/ipcFFI.h
        src/ipcLisp.c
        src/ipcPriv.h
        src/key.c
        src/key.h
        src/lex.c
        src/lex.h
        src/libc.h
        src/list.c
        src/list.h
        src/logging.c
        src/logging.h
        src/marshall.c
        src/modLogging.c
        src/modLogging.h
        src/modVar.c
        src/modVar.h
        src/msgTap.c
        src/msgTap.h
        src/multiThread.c
        src/multiThread.h
        src/parseFmttrs.c
        src/parseFmttrs.h
        src/primFmttrs.c
        src/primFmttrs.h
        src/printData.c
        src/printData.h
        src/queryResponse.c
        src/recvMsg.c
        src/recvMsg.h
        src/reg.c
        src/reg.h
        src/res.c
        src/res.h
        src/resMod.c
        src/resMod.h
        src/sendMsg.c
        src/sendMsg.h
        src/strList.c
        src/strList.h
        src/tca.h
        src/tcaInternal.h
        src/tcaMem.c
        src/tcaMem.h
        src/tcaRef.c
        src/tcaRef.h
        src/tcerror.c
        src/tcerror.h
        src/tcModError.c
        src/timer.c
        src/wsocksrt.c)

set(CMAKE_CXX_FLAGS "-O2")

add_definitions(-DREDHAT_52 -DREDHAT_6 -DREDHAT_71)

add_library(ipc SHARED ${SOURCE_FILES})