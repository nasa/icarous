//
// Created by swee on 7/22/18.
//

#ifndef ICAROUS_CFS_MERGER_TEST_H
#define ICAROUS_CFS_MERGER_TEST_H

#include "merger_table.h"
#include "msgdef/merger_msg.h"
#include "msgdef/ardupilot_msg.h"
#include "IntersectionScheduler.h"
#include "stdbool.h"

#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_timer_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"


mergerAppData_t mergerAppData;
log_ud_acc_t logEntry;

void Test_Setup(void);

void Test_TearDown(void);

void InitializeData(void);

void Test_ComputeEntryPoint(void);

void Test_ComputeArrivalData(void);

void Test_ComputePathLength(void);

void Test_CheckIntersectionConflict(void);

void Test_ComputeNewTrajectory(void);

#endif //ICAROUS_CFS_MERGER_TEST_H
