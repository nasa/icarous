#ifndef PLEXIL_WRAPPER_H
#define PLEXIL_WRAPPER_H

#include "cfs-data-format.hh"

#ifdef __cplusplus
extern "C" {
#endif

  struct plexilExec;
  struct plexilInterfaceAdapter;
  int plexil_init(int argc, char** argv,struct plexilExec** exec,struct plexilInterfaceAdapter** adp);
  void plexil_run(struct plexilExec* exec);
  int plexil_destroy(struct plexilExec *exec);
  int plexil_getLookup(struct plexilInterfaceAdapter* adp,PlexilCommandMsg* msg);
  int plexil_getCommand(struct plexilInterfaceAdapter* adp,PlexilCommandMsg* msg);
  void plexil_return(struct plexilInterfaceAdapter* adp, PlexilCommandMsg* msg);

#ifdef __cplusplus
}
#endif

#endif // PLEXIL_WRAPPER_H
