#ifndef PLEXIL_WRAPPER_H
#define PLEXIL_WRAPPER_H

#include "cfs-data-format.hh"
#include <string.h>
#include <stdbool.h>
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
  static char* serializeBool(bool arrayelement,const bool o,char* b);
  static char* serializeBoolArray(int size,const bool val[],char* buffer);
  static char* serializeInt(bool arrayelement,const int32_t val,char* buffer);
  static char* serializeIntArray(int size,const int32_t val[],char* buffer);
  static char* serializeReal(bool arrayelement,const double val,char* buffer);
  static char* serializeRealArray(int size,const double val[],char* buffer);
  static char* serializeString(int size,const char val[],char* buffer);

  static const char* deSerializeBool(bool arrayelement,bool* val,const char* buffer);
  static const char* deSerializeBoolArray(bool val[],const char* buffer);
  static const char* deSerializeInt(bool arrayelement,int32_t* val,const char* buffer);
  static const char* deSerializeIntArray(int32_t val[],const char* buffer);
  static const char* deSerializeReal(bool arrayelement,double* val,const char* buffer);
  static const char* deSerializeRealArray(double val[],const char* buffer);
  static const char* deSerializeString(char val[],const char* buffer);

  static char *serializeBoolVector(int size,const bool o[], char *b);
  static char const *deserializeBoolVector(int size,bool o[], const char *b);

#ifdef __cplusplus
}
#endif



#endif // PLEXIL_WRAPPER_H
