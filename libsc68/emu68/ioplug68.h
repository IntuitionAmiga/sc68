/**
 * @ingroup   emu68_ioplug_devel
 * @file      emu68/ioplug68.h
 * @date      1999/03/13
 * @brief     68k IO plugger header.
 * @author    Benjamin Gerard
 *
 * $Id$
 *
 */
 
/* Copyright (C) 1998-2007 Benjamin Gerard */

#ifndef _EMU68_IOPLUG68_H_
#define _EMU68_IOPLUG68_H_

#include "struct68.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup  emu68_ioplug_devel  68k IO plugger
 * @ingroup   emu68_devel
 * @brief     IO plugging and mapping facilities.
 *
 *   Provide functions for warm plugging/unplugging of IO
 *   chipset. Limitations are explained in 
 *   @ref emu68_mem68_devel "68k memory and IO manager"
 *   detailed description.
 *
 * @{
 */

/** Unplug all IO.
 *
 *    Process emu68_ioplug_unplug() function for all pluged IO.
 *
 */
void emu68_ioplug_unplug_all(emu68_t * const emu68);

/** Unplug and destroy all IO.
 *
 *    Process emu68_ioplug_unplug() function for all pluged IO and
 *    destroy each io by calling its io68_t::destroy function.
 *
 */
void emu68_ioplug_destroy_all(emu68_t * const emu68);

/** Unplug an IO.
 *
 *    The emu68_ioplug_unplug() function removes an IO from pluged IO
 *    list and reset memory access handler for its area.
 *
 *  @param   io  Address of IO structure to unplug.
 *
 *  @return   error-code
 *  @retval   0   Success
 *  @retval   <0  Error (probably no matching IO)
 */
int emu68_ioplug_unplug(emu68_t * const emu68, io68_t * const io);

/** Plug an IO.
 *
 *    The emu68_ioplug() function add an IO to pluged IO list and add
 *    suitable memory access handlers.
 *
 *  @param  io  Address of IO structure to plug.
 */
void emu68_ioplug(emu68_t * const emu68, io68_t * const io);

/**
 *@}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _EMU68_IOPLUG68_H_ */
