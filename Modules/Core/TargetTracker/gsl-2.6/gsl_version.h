#ifndef __GSL_VERSION_H__
#define __GSL_VERSION_H__

#include <gsl/gsl_types.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif
__BEGIN_DECLS


#define GSL_VERSION "2.6"
#define GSL_MAJOR_VERSION 2
#define GSL_MINOR_VERSION 6

GSL_VAR const char * gsl_version;

__END_DECLS

#endif /* __GSL_VERSION_H__ */
