/**
 * @ingroup  file68_lib
 * @file     sc68/timedb68.h
 * @author   Benjamin Gerard
 * @date     2013-05-30
 * @brief    music duration database header.
 */
/* Time-stamp: <2013-07-08 07:36:55 ben> */

/* Copyright (C) 1998-2013 Benjamin Gerard */

#ifndef _FILE68_TIMEDB68_H_
#define _FILE68_TIMEDB68_H_

#include "file68_api.h"

/**
 *  @defgroup  file68_timedb  External music info database.
 *  @ingroup   file68_lib
 *
 *    Provides function for accessing the music length database.
 *
 *    Because many sndh files do not have a song length information
 *    and many people seems to be using sndh files rather than sc68
 *    ones, the use of this database should help exposing a proper
 *    song length in most cases.
 *
 *  @{
 */

/**
 * flags values.
 */
enum {
  TDB_STE = 1 << 4,              /**< set if STE hardware is used */
  TDB_TD  = 1 << 3,              /**< set if timer D *NOT* used   */
  TDB_TC  = 1 << 2,              /**< set if timer C *NOT* used   */
  TDB_TB  = 1 << 1,              /**< set if timer B *NOT* used   */
  TDB_TA  = 1 << 0,              /**< set if timer A *NOT* used   */
};


FILE68_API
/**
 *   Load the database,
 */
int timedb68_load(void);

FILE68_API
/**
 *   Save the database (only if it has been modified).
 */
int timedb68_save(void);

FILE68_API
/**
 *   Add an entry to the database (in memory only).
 */
int timedb68_add(int hash, int track, unsigned int frames, int flags);

FILE68_API
/**
 *   Retrieve an entry from the database.
 */
int timedb68_get(int hash, int track, unsigned int * frames, int * flags);

/**
 *  @}
 */

#endif /* #ifndef _FILE68_TIMEDB68_H_ */
