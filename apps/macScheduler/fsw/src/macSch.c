/**
 * @file macSch.c
 * @brief function definitions for mac scheduler app
 */

#include "macSch.h"
#include "sch_msgids.h"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  MacSch_EventFilters[] =
{  /* Event ID    mask */
	{MACSCH_STARTUP_INF_EID,       0x0000},
	{MACSCH_COMMAND_ERR_EID,       0x0000},
};

/* MACSCH_AppMain() -- Application entry points */
void MACSCH_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

        MACSCH_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
	
	}

        MACSCH_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void MACSCH_AppInit(void){

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(MacSch_EventFilters,
			sizeof(MacSch_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Initialize all messages that this App generates.
	// To perfrom sense and avoid, as a minimum, the following messages must be generated
	CFE_SB_InitMsg(&FREQ_50,FREQ_50_WAKEUP_MID,sizeof(macScheduler_t),TRUE);
	CFE_SB_InitMsg(&FREQ_30,FREQ_30_WAKEUP_MID,sizeof(macScheduler_t),TRUE);
	CFE_SB_InitMsg(&FREQ_10,FREQ_10_WAKEUP_MID,sizeof(macScheduler_t),TRUE);
        CFE_SB_InitMsg(&FREQ_01,FREQ_01_WAKEUP_MID,sizeof(macScheduler_t),TRUE);


	// Send event indicating app initialization
	CFE_EVS_SendEvent (MACSCH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Mac Scheduler Interface initialized. Version %d.%d",
					   MACSCH_MAJOR_VERSION,
					   MACSCH_MINOR_VERSION);
        
        uint32_t clockAccuracy;
        status = OS_TimerCreate(&timerId_50,"FREQ_50",&clockAccuracy,timer50_callback);
        if(status != CFE_SUCCESS){
          OS_printf("Could not create FREQ_50 timer\n");
        }

        status = OS_TimerCreate(&timerId_30,"FREQ_30",&clockAccuracy,timer30_callback);
        if(status != CFE_SUCCESS){
          OS_printf("Could not create FREQ_30 timer\n");
        }

        status = OS_TimerCreate(&timerId_10,"FREQ_10",&clockAccuracy,timer10_callback);
        if(status != CFE_SUCCESS){
          OS_printf("Could not create FREQ_10 timer\n");
        }

        status = OS_TimerCreate(&timerId_01,"FREQ_01",&clockAccuracy,timer01_callback);
        if(status != CFE_SUCCESS){
          OS_printf("Could not create FREQ_01 timer\n");
        } 

        OS_TimerSet(timerId_50,100,1000000/50);
        OS_TimerSet(timerId_30,100,1000000/30);
        OS_TimerSet(timerId_10,100,1000000/10);
        OS_TimerSet(timerId_01,100,1000000);
 

}

void APINTERFACE_AppCleanUp(){
    //TODO: clean up memory allocation here if necessary
}

void timer50_callback(uint32_t timerid){
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FREQ_50);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FREQ_50);
}

void timer30_callback(uint32_t timerid){

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FREQ_30);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FREQ_30);
}

void timer10_callback(uint32_t timerid){

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FREQ_10);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FREQ_10);
}

void timer01_callback(uint32_t timerid){

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FREQ_01);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FREQ_01);
}
