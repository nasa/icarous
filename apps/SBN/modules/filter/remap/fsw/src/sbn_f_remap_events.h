#ifndef _sbn_f_remap_events_h
#define _sbn_f_remap_events_h

extern CFE_EVS_EventID_t SBN_F_REMAP_FIRST_EID; /* defined at module init time */

#define SBN_F_REMAP_EID     SBN_F_REMAP_FIRST_EID + 1
#define SBN_F_REMAP_TBL_EID SBN_F_REMAP_FIRST_EID + 2

#endif /* _sbn_f_remap_events_h */
