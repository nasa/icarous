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
  int plexil_getLookup(struct plexilInterfaceAdapter* adp,PlexilMsg* msg);
  int plexil_getCommand(struct plexilInterfaceAdapter* adp,PlexilMsg* msg);
  void plexil_return(struct plexilInterfaceAdapter* adp, PlexilMsg* msg);

  void serialize(char* buffer);
  char* serializeBool(const bool o,char* b);
  char* serializeBoolArray(int size,const bool val[],char* buffer);
  char* serializeInt(const int32_t val,char* buffer);
  char* serializeIntArray(int size,const int32_t val[],char* buffer);
  char* serializeReal(const double val,char* buffer);
  char* serializeRealArray(int size,const double val[],char* buffer);
  char* serializeString(int size,const char val[],char* buffer);

  const char* deSerializeBool(bool* val,const char* buffer);
  const char* deSerializeBoolArray(bool val[],const char* buffer);
  const char* deSerializeInt(int32_t* val,const char* buffer);
  const char* deSerializeIntArray(int32_t val[],const char* buffer);
  const char* deSerializeReal(double* val,const char* buffer);
  const char* deSerializeRealArray(double val[],const char* buffer);
  const char* deSerializeString(char val[],const char* buffer);

  char *serializeBoolVector(int size,const bool o[], char *b);
  char const *deserializeBoolVector(int size,bool o[], const char *b);

#ifdef __cplusplus
}
#endif

#endif // PLEXIL_WRAPPER_H
