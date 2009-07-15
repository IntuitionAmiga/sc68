/*
 *          sc68 - YM-2149 emulator - Linear Volume Table
 *             Copyright (C) 2001-2009 Benjamin Gerard
 *           <benjihan -4t- users.sourceforge -d0t- net>
 *
 * This  program is  free  software: you  can  redistribute it  and/or
 * modify  it under the  terms of  the GNU  General Public  License as
 * published by the Free Software  Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 *
 * You should have  received a copy of the  GNU General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* $Id$ */

/* Table of 5 bit D/A output level for 1 channel.*/

static const u16 ymout1c5bit[32] =
{
  310,  369,  438,  521,  619,  735,  874, 1039,
  1234, 1467, 1744, 2072, 2463, 2927, 3479, 4135,
  4914, 5841, 6942, 8250, 9806,11654,13851,16462,
  19565,23253,27636,32845,39037,46395,55141,65535
};

/* Create a linear 3 channels 5 bit per channels DAC table.
 */
void ym_create_5bit_linear_table(s16 * out, unsigned int level)
{
  int i;
  const unsigned int min = ymout1c5bit[00];
  const unsigned int max = ymout1c5bit[31];
  const unsigned int div = max-min ? max-min : 1;
  const int center = ( level + 1 ) >> 1;
  for (i=0; i<32*32*32; ++i) {
    int tmp =
      (  (unsigned int) ymout1c5bit[0x1F & (i>>10)] +
         (unsigned int) ymout1c5bit[0x1F & (i>> 5)] +
         (unsigned int) ymout1c5bit[0x1F & (i>> 0)] -
         3u * min ) / 3u * level / div;
    out[i] = tmp - center;
  }
  msg68_info("ym-2149: volume model -- *LINEAR* -- [%d..%d]\n",
             out[0],out[0x7FFF]);
}
