/**
 * @ingroup   debug68_devel
 * @file      SC68debug_io.h
 * @author    Ben(jamin) Gerard <ben@sashipa.com>
 * @date      1999/08/14
 * @brief     debug68 fake IO emulation
 * @version   $Id: SC68debug_io.h,v 2.2 2003/11/20 21:10:39 benjihan Exp $
 */

/*
 *                    sc68 - debug68 fake IO emulation
 *         Copyright (C) 2001 Ben(jamin) Gerard <ben@sashipa.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef _SC68DEBUG_IO_H_
#define _SC68DEBUG_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "emu68/struct68.h"

/** Used to detect amiga hardware access. */
extern int paula_debug_access;

/** Used to detect YM-2149 hardware access. */
extern int ym_debug_access;

/** Used to detect Microwire and LMC hardware access. */
extern int mw_debug_access;

/** Used to detect Atari-ST shifter hardware access. */
extern int sh_debug_access;

/** @name Fake io plugins.
 *  @{
 */
extern io68_t paula_io;
extern io68_t ym_io;
extern io68_t mw_io;
extern io68_t sh_io;
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SC68DEBUG_IO_H_ */
