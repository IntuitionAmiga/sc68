/**
 * @ingroup  file68_lib
 * @file     sc68/file68_api.h
 * @author   Benjamin Gerard
 * @date     2007-02-25
 * @brief    Symbol exportation header.
 *
 */

/* $Id$ */

/* Copyright (C) 1998-2009 Benjamin Gerard */

#ifndef _FILE68_FILE68_API_H_
#define _FILE68_FILE68_API_H_

#ifndef FILE68_EXTERN
# ifdef __cplusplus
#   define FILE68_EXTERN extern "C"
# else
#   define FILE68_EXTERN extern
# endif
#endif

#ifndef FILE68_API
/* Building */
# ifdef FILE68_EXPORT
#  if defined(DLL_EXPORT) && defined(HAVE_DECLSPEC)
#   define FILE68_API __declspec(dllexport)
#  elif defined(HAVE_VISIBILITY)
#   define FILE68_API FILE68_EXTERN __attribute__ ((visibility("default")))
#  else
#   define FILE68_API FILE68_EXTERN
#  endif
/* Using */
# else
#  if defined(FILE68_DLL)
#   define FILE68_API __declspec(dllimport)
#  else
#   define FILE68_API FILE68_EXTERN
#  endif
# endif
#endif

/** @addtogroup file68_lib
 *
 *     How symbols are exported.
 *
 *  @{
 */

#ifndef FILE68_API
/** file68 symbols specification.
 *
 *  Define special atributs for importing/exporting file68 symbols.
 */
#define FILE68_API FILE68_EXTERN
#error "FILE68_API should be defined"
#endif

/**
 *  @}
 */

#endif /* ifndef _FILE68_FILE68_API_H_ */
