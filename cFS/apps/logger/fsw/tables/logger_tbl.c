#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "logger_table.h"
#include "logger.h"

#ifdef TESTCASE1
LoggerTable_t TblStruct={
    false,                         // log data
    "../ram/IClog/TestCase1",      // Source directory of log files
    "2018-206-13:34:43"            // time stamp for playback.
};
#elif TESTCASE2
LoggerTable_t TblStruct={
    false,                         // log data
    "../ram/IClog/TestCase2",      // Source directory of log files
    "2018-206-13:39:21"            // time stamp for playback.
};
#else
LoggerTable_t TblStruct={
     true,                         // log data
     "../ram/IClog/",               // Source directory of log files
     "1980-015-14:02:48"            // time stamp for playback.
};
#endif




/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(TblStruct, LOG.LoggerTable, Logger parameters, logger_tbl.tbl )
