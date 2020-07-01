#ifndef _sbn_tcp_events_h
#define _sbn_tcp_events_h

extern CFE_EVS_EventID_t SBN_TCP_FIRST_EID; /* defined at module init time */

#define SBN_TCP_SOCK_EID       SBN_TCP_FIRST_EID + 1 /* skip 0th */
#define SBN_TCP_CONFIG_EID     SBN_TCP_FIRST_EID + 2
#define SBN_TCP_DEBUG_EID      SBN_TCP_FIRST_EID + 3

#endif /* _sbn_tcp_events_h */
