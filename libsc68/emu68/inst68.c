/**
 * @ingroup   emu68_devel
 * @file      inst68.c
 * @author    Benjamin Gerard
 * @date      1999/03/13
 * @brief     68000 arythmetic and logical instructions emulation
 * @version   $Id$
 */

/* Copyright (C) 1998-2007 Ben(jamin) Gerard */

#include "inst68.h"
#include "error68.h"
#include "mem68.h"
#include "macro68.h"

s32 add68(emu68_t * const emu68, s32 a, s32 b, s32 c)
{
  s32 s2,s,z;
  s = s2 = a+b+c;
  z = (!s2)<<SR_Z_BIT;
  a >>= 31;
  b >>= 31;
  s >>= 31;
  a &= (SR_V|SR_C|SR_X);
  b &= (SR_V|SR_C|SR_X);
  s &= (SR_V|SR_C|SR_X|SR_N);
  a ^= s;
  b ^= s;
  a ^= SR_V;
  b ^= SR_V;
  s |= SR_V;
  s &= ~SR_N;
  s ^= (a|b|z);
  SET_CCR(emu68->reg.sr,s);
  return s2;
}

s32 sub68(emu68_t * const emu68, s32 a, s32 b, s32 c)
{
  s32 s;
  int ccr;
  s = a-b-c;
  ccr  = (!s)<<SR_Z_BIT;
  ccr |= (s>>31)&SR_N;
  ccr |= ((((a^~s) & (b^s))^s)>>31) & (SR_C+SR_X);
  ccr |= (((a^s) & (~b^s))>>31) & SR_V;
  SET_CCR(REG68.sr,ccr);
  return s;
}

s32 and68(emu68_t * const emu68, u32 a, u32 b)
{
  s32 s;
  int ccr;
  s = a & b;
  ccr = REG68.sr&SR_X;
  ccr |= (!s)<<SR_Z_BIT;
  ccr |= (s>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return s;
}

s32 orr68(emu68_t * const emu68, u32 a, u32 b)
{
  s32 s;
  int ccr;
  s = a | b;
  ccr = REG68.sr&SR_X;
  ccr |= (!s)<<SR_Z_BIT;
  ccr |= (s>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return s;
}

s32 eor68(emu68_t * const emu68, u32 a, u32 b)
{
  s32 s;
  int ccr;
  s = a ^ b;
  ccr = REG68.sr&SR_X;
  ccr |= (!s)<<SR_Z_BIT;
  ccr |= (s>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return s;
}

s32 not68(emu68_t * const emu68, s32 s)
{
  int ccr;
  s = ~s;
  ccr = REG68.sr&SR_X;
  ccr |= (!s)<<SR_Z_BIT;
  ccr |= (s>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return s;
}

s32 muls68(emu68_t * const emu68, s32 a, s32 b)
{
  int ccr;
  a = (a>>16) * (b>>16);
  ccr  = REG68.sr&SR_X;
  ccr |= (!a)<<SR_Z_BIT;
  ccr |= ((s32)a>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return a;
}

s32 mulu68(emu68_t * const emu68, u32 a, u32 b)
{
  int ccr;
  a = (a>>16) * (b>>16);
  ccr  = REG68.sr&SR_X;
  ccr |= (!a)<<SR_Z_BIT;
  ccr |= ((s32)a>>31)&SR_N;
  SET_CCR(REG68.sr,ccr);
  return a;
}

s32 divs68(emu68_t * const emu68, s32 a, s32 b)
{
  s32 res,rem;
  int v,ccr;
  a>>=16;
  if (a) {
    res = b/a;
    rem = b%a;
    v = ((s16)res!=res)<<SR_V_BIT;
    ccr  = REG68.sr&SR_X;
    ccr |= v;
    ccr |= ((s16)res!=res)<<SR_V;
    ccr |= (!res)<<SR_Z_BIT;
    ccr |= ((s32)res>>31)&SR_N;
    SET_CCR(REG68.sr,ccr);
  } else {
    EXCEPTION(DIVIDE_VECTOR,DIVIDE_LVL);
    return b;
  }
  return v ? b : (((u16)res) | (rem<<16));
}

s32 divu68(emu68_t * const emu68, u32 a, u32 b)
{
  u32 res,rem;
  int v,ccr;
  a>>=16;
  if (a) {
    res = b/a;
    rem = b%a;
    v = ((u16)res!=res)<<SR_V_BIT;
    ccr  = REG68.sr&SR_X;
    ccr |= v;
    ccr |= (!res)<<SR_Z_BIT;
    ccr |= ((s32)res>>31)&SR_N;
    SET_CCR(REG68.sr,ccr);
  } else {
    EXCEPTION(DIVIDE_VECTOR,DIVIDE_LVL);
    return b;
  }
  return v ? b : (((u16)res) | (rem<<16));
}
