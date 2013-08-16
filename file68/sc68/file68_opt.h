/**
 * @ingroup  file68_lib
 * @file     sc68/file68_opt.h
 * @brief    Command line option manipulation header.
 * @author   Benjamin Gerard
 * @date     2009-02-04
 */
/* Time-stamp: <2013-08-16 05:08:15 ben> */

/* Copyright (C) 1998-2013 Benjamin Gerard */

#ifndef FILE68_OPT_H
#define FILE68_OPT_H

#ifndef FILE68_API
# include "file68_api.h"
#endif

/**
 * @defgroup  file68_opt  Options manipulation
 * @ingroup   file68_lib
 *
 *   Provides command line options manipulation functions.
 *
 * @{
 */

/**
 * option argument types.
 */
enum option68_e {
  option68_BOL = 0,             /**< Boolean (set or unset). */
  option68_STR = 1,             /**< String value.           */
  option68_INT = 2,             /**< Integer value.          */
  option68_ERR = -1             /**< Errorcode.              */
};

/**
 * option type.
 */
typedef struct option68_s option68_t;

/**
 * option value.
 */
typedef union {
  const char * str;                /**< Value for string argument.  */
  int          num;                /**< Value for integer argument. */
} value68_t;

/**
 * Options help display function.
 *
 *  -# user data
 *  -# option
 *  -# envvar
 *  -# short description
 */
typedef void (*option68_help_t)(void *, const char*, const char*, const char*);

/**
 * Callback on change value.
 */
typedef int (*option68_cb_t)(const option68_t *, value68_t *);

/** Command line option description and parsing info. */
struct option68_s {
  option68_cb_t   onchange;    /**< Call on value change.           */
  int             has_arg;     /**< @see option68_e. ~val => setted */
  const char    * prefix;      /**< Key prefix.                     */
  const char    * name;        /**< Key name (bare).                */
  const char    * cat;         /**< Category name.                  */
  const char    * desc;        /**< Sh-ort description.             */
  value68_t       val;         /**< Option value.                   */

  /**
   * @name internals
   * @{
   */
  int          prefix_len;       /**< length of option68_t::prefix. */
  int          name_len;         /**< length of option68_t::name.   */
  option68_t * next;             /**< Chain to next option.         */
  /**
   * @}
   */
};

FILE68_API
/**
 * Print defined options.
 *
 * @param  cookie  User data used as 1st argument for fct
 * @param  fct     Fonction call for each possible option
 */
void option68_help(void * cookie, option68_help_t fct);


FILE68_API
/**
 * Append option definitions.
 *
 * @param  options  Array of options
 * @param  n        Array size
 *
 * @notice Options are not internally copied.
 */
int option68_append(option68_t * options, int n);

FILE68_API
/**
 * Parse command line options.
 *
 * @param  argc     argument count
 * @param  argv     arguments
 * @param  reset    reset all options before parsing
 * @retval remaining argc
 */
int option68_parse(int argc, char ** argv, int reset);

FILE68_API
/**
 * Get option type.
 *
 * @param   opt      option
 * @return  one of option68_e value
 * @retval  option68_ERR on error
 */
int option68_type(const option68_t * opt);

FILE68_API
/**
 * Enumerate options.
 *
 * @param   idx     options index (0-based).
 * @return  option
 * @retval  0       not found
 */
option68_t * option68_enum(int idx);

FILE68_API
/**
 * Get option by name.
 *
 * @param   key      argument count
 * @param   setonly  only if option has been set
 * @return  option
 * @retval  0        not found
 */
option68_t * option68_get(const char * key, int setonly);

FILE68_API
/**
 * Test if option has been set.
 *
 * @param   opt  option to test
 * @retval  1    option has been set
 * @retval  0    option has not been set
 */
int option68_isset(const option68_t * opt);

FILE68_API
/**
 * Set option.
 *
 * @param   option  option to set
 * @retval  0       on success
 * @retval -1       on failure
 */
int option68_set(option68_t * opt, const char * str);

FILE68_API
/**
 * Set option (integer and boolean only).
 *
 * @param   opt  option to set
 * @retval  0    on success
 * @retval -1    on failure
 */
int option68_iset(option68_t * opt, int val);

FILE68_API
/**
 * Unset option.
 *
 * @param   opt  option to unset
 * @retval  0    on success
 * @retval -1    on failure
 */
int option68_unset(option68_t * opt);

FILE68_API
/**
 * Get associate environment variable value.
 *
 * @param   opt  option
 * @param   set  enable option set at the same time
 * @retval  0    on error (or envvar does not exist)
 */
const char * option68_getenv(option68_t * opt, int set);

FILE68_API
/**
 * Add symbolic value.
 *
 * @param   key  symbolic value
 * @param   val  integer value
 * @retval  0    on success
 * @retval  -1   on error
 */
int option68_add_symb(const char * key, int val);

FILE68_API
/**
 * Get symbolic value.
 *
 * @param   key  symbolic value
 * @param   val  pointer to integer for storage
 * @retval  0    on success
 * @retval  -1   on error
 */
int option68_get_symb(const char * key, int * val);

/**
 * @}
 */

#endif
