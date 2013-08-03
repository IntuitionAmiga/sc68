/*
 * @file    istream68_ao.c
 * @brief   implements istream68 for libao
 * @author  http://sourceforge.net/users/benjihan
 *
 * Copyright (C) 1998-2011 Benjamin Gerard
 *
 * Time-stamp: <2013-08-03 13:50:49 ben>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "file68_api.h"
#include "file68_vfs_ao.h"
#include "file68_aud.h"
#include "file68_msg.h"

#ifndef DEBUG_AO68_O
# define DEBUG_AO68_O 0
#endif
int ao68_cat = msg68_DEFAULT;

/* Define this if you want xiph libao support. */
#ifdef USE_AO

#include "file68_vfs_def.h"
#include "file68_str.h"
#include "file68_uri.h"

#include <ao/ao.h>
#include <string.h>
#include <stdlib.h>

struct ao68_info_s {
  int                default_id;
  ao_device        * default_device;
  int                driver_id;
  ao_device        * device;
  ao_sample_format   format;
  ao_option        * options;
};

typedef struct ao68_info_s ao68_info_t;

/** vfs ao structure. */
typedef struct {
  vfs68_t vfs;        /**< vfs function.                */
  unsigned int count;         /**< current position.                */
  ao68_info_t ao;             /**< ao specific struct.              */
  int  no_overwrite;          /**< overwriting file output.         */
  char *outname;              /**< part of name[] for file driver.  */
  char defoutname[16];        /**< default outname for file driver. */
  char name[1];               /**< filename.                        */
} vfs68_ao_t;

enum {
  AO_SPR_MIN = 8000,
  AO_SPR_MAX = 96000,
  AO_SPR_DEF = 44100
};

static volatile int init;
static unsigned int vfs68_ao_defaut_rate = AO_SPR_DEF;
static vfs68_ao_t * ao_unic_vfs;
static int ao_ismine(const char *);
static vfs68_t * ao_create(const char *, int, int, va_list);
static scheme68_t ao_scheme = {
  0, "vfs-ao", ao_ismine, ao_create
};

static unsigned int spr_in_range(unsigned int rate)
{
  if (rate > (unsigned)AO_SPR_MAX) {
    rate = AO_SPR_MAX;
  } else if (rate < (unsigned)AO_SPR_MIN) {
    rate = AO_SPR_MIN;
  }
  return rate;
}

int vfs68_ao_sampling(vfs68_t * vfs)
{
  vfs68_ao_t * is = (vfs68_ao_t *)vfs;
  if (!is || !is->ao.format.rate)
    return vfs68_ao_defaut_rate;
  return is->ao.format.rate;
}

/* $$$ AO is the only audio backend we have. No need for much
   complicated mechanism right now. */
unsigned int audio68_sampling_rate(const unsigned int rate)
{
  if (!rate) {
    return vfs68_ao_sampling(&ao_unic_vfs->vfs);
  }
  vfs68_ao_defaut_rate = spr_in_range(rate);
  TRACE68(ao68_cat,
          "audio68: default sampling rate -- *%uhz*\n",
          vfs68_ao_defaut_rate);
  if (ao_unic_vfs) {
    TRACE68(ao68_cat,
            "audio68: VFS sampling rate -- *%uhz*\n",
            vfs68_ao_defaut_rate);
    ao_unic_vfs->ao.format.rate = vfs68_ao_defaut_rate;
  }
  return vfs68_ao_defaut_rate;
}

/* Init does not actually initialize aolib. That task will be
   performed on the fly only if needed. */
int vfs68_ao_init(void)
{
  if (ao68_cat == msg68_DEFAULT) {
    ao68_cat =
      msg68_cat("audio","Xiph AO audio stream",DEBUG_AO68_O);
    if (ao68_cat == -1)
      ao68_cat = msg68_DEFAULT;
  }
  uri68_register(&ao_scheme);
  return 0;
}


static int init_aolib(void)
{
  int err = -1;

  if (init) {
    msg68_critical("libao68: *%s*\n","already done");
  } else {
    ao_initialize();
    err = 0;
    init = 1;
    TRACE68(ao68_cat,"libao68: *%s*\n","initialized");
  }
  return err;
}

static void shutdown_aolib(void)
{
  if (init) {
    init = 0;
    ao_shutdown();
    TRACE68(ao68_cat,"libao68: *%s*\n","shutdowned");
  }
}

void vfs68_ao_shutdown(void)
{
  shutdown_aolib();
  if (ao68_cat != msg68_DEFAULT) {
    msg68_cat_free(ao68_cat);
    ao68_cat = msg68_DEFAULT;
  }
}

static int ao_ismine(const char * uri)
{
  if (!strncmp68(uri, "audio:", 6))
    return SCHEME68_ISMINE | SCHEME68_WRITE;
  return 0;
}

static const char * isao_name(vfs68_t * vfs)
{
  vfs68_ao_t * ism = (vfs68_ao_t *)vfs;

  return (!ism || !ism->name[0])
    ? 0
    : ism->name
    ;
}

static void dump_ao_info(const int id, const ao_info * info, const int full)
{
  if (info) {
    TRACE68(ao68_cat,
      "\n"
      "libao68: ao_driver #%02d\n"
      "         ,------------\n"
      "         | type    : %s\n"
      "         | name    : %s\n"
      "         | short   : %s\n"
      "         | author  : %s\n"
      "         | comment : %s\n"
      "         | prio    : %d\n",
      id, info->type==AO_TYPE_LIVE?"live":"file",
      strnevernull68(info->name),
      strnevernull68(info->short_name),
      strnevernull68(info->author),
      strnevernull68(info->comment),info->priority);
    if (full) {
      int i;
      TRACE68(ao68_cat, "         | options : %d --", info->option_count);
      for (i=0; i<info->option_count; ++i)
        TRACE68(ao68_cat, " %s",info->options[i]);
      TRACE68(ao68_cat, "%c",'\n');
    }
  }
}

static void dump_ao_format(const ao_sample_format * fmt)
{
  TRACE68(ao68_cat,
          "libao68: sample format -- %d-bits %d-hz %d-chan %c-endian %s\n",
          fmt->bits, fmt->rate, fmt->channels,
          fmt->byte_format == AO_FMT_LITTLE ? 'L' :
          (fmt->byte_format == AO_FMT_BIG ? 'B' : 'N'),
          fmt->matrix?fmt->matrix:"no-matrix");
}

static int isao_open(vfs68_t * vfs)
{
  vfs68_ao_t * is = (vfs68_ao_t *)vfs;
  int err = -1;
  ao_info * info = 0;
  char * url;

  TRACE68(ao68_cat,"libao68: open -- '%s'\n", vfs68_filename(vfs));
  if (!is || is->ao.device) {
    goto error;
  }

  url = is->name;
  do {
    char * s = strchr(url,'/'), *s2;
    if (s) *s = '\0';
    s2 = strchr(url,'=');
    if (s2) {
      char * key = url;
      char * val = s2+1;
      *s2 = 0;
      TRACE68(ao68_cat,"libao68: open -- have a key -- [%s]='%s'\n", key, val);
      if (!strcmp68(key,"output")) {
        if (s) *s = '/';
        s = 0;
        TRACE68(ao68_cat,"libao68: open -- *FILENAME* '%s'\n", val);
        is->outname = val;
      } else if (!strcmp68(key,"driver")) {
        int id = !strcmp68(val,"default")
          ? ao_default_driver_id()
          : ao_driver_id(val);
        info = ao_driver_info(id);
        if (info) {
          is->ao.driver_id = id;
          TRACE68(ao68_cat,"libao68: open -- *DRIVER* #%d (%s) %s\n",
                  id, info->short_name, info->name);
        }
      } else if (!strcmp68(key,"rate")) {
        int frq = 0;
        while (*val>='0' && *val<='9') frq = frq*10 + *val++ - '0';
        if (frq) {
          frq = spr_in_range(frq);
          TRACE68(ao68_cat,"libao68: open -- *SAMPLING-RATE* %d\n",frq);
          is->ao.format.rate = frq;
        }
      } else if (!strcmp68(key,"format")) {
        while(*val) {
          switch (*val++) {

            /* ENDIANESS */
          case 'n': /* native (same as cpu) */
            is->ao.format.byte_format = AO_FMT_NATIVE;
            break;
          case 'l': /* little */
            is->ao.format.byte_format = AO_FMT_LITTLE;
            break;
          case 'b': /* big    */
            is->ao.format.byte_format = AO_FMT_BIG;
            break;

            /* SIGN */
          case '+': /* unsigned */
            msg68_warning("libao68: ignoring -- *%s* request\n", "UNSIGNED");
          case '-': /*   signed */
            break;

            /* CHANNELS */
          case '1': /* mono, 1 channel */
            is->ao.format.channels = 1;
            break;
          case '2': /* stereo, 2 channels */
            is->ao.format.channels = 2;
            break;

            /* FORMAT */
          case 'W': /* 16 bit */
            is->ao.format.bits = 16;
            break;
          case 'B': /*  8 bit */
            is->ao.format.bits = 8;
            break;
          case 'F': /* float  */
            msg68_warning("libao68: open -- ignoring *%s* request\n","FLOAT");
            break;
          } /* switch */
        } /* while */
      } else {
        int i = 0;
        if (!info)
          msg68_warning("libao68: choose driver before options -- [%s]='%s'\n",
                        key, val);
        else
          for (; i<info->option_count && strcmp68(info->options[i],key); ++i)
            ;
        if (!info || i<info->option_count) {
          int res = ao_append_option(&is->ao.options, key, val);
          res = res;
          TRACE68(ao68_cat, "libao68: add options [%s]='%s' -- *%s*\n",
                  key, val, strok68(!res));
        } else
          msg68_warning("libao68: ignore option for driver '%s' -- [%s]='%s'\n",
                        info->short_name, key, val);
      }
      *s2 = '=';
    } else {
      /* Options w/out '=' */
      if (!strcmp68(url,"drivers")) {
        /* List all drivers in debug */
        int id,n;
        ao_info ** infolist = ao_driver_info_list(&n);
        if (infolist) {
          for (id=0; id<n; ++id) {
            dump_ao_info(id,infolist[id],1);
          }
        }
      }
    }

    if (s) {
      *s = '/';
      url = s+1;
    } else {
      url = 0;
    }
  } while (url);

  info = ao_driver_info(is->ao.driver_id);
  if (!info) {
    goto error;
  }

  dump_ao_info(is->ao.driver_id,info,1);

  if (!is->outname) {
#ifdef HAVE_AO_FILE_EXTENSION
    char * ext = ao_file_extension(is->ao.driver_id);
#else
    char * ext = ".out";
    if (!strcmp68(info->short_name,"wav")) {
      ext = ".wav";
    } else if (!strcmp68(info->short_name,"au")) {
      ext = ".au";
    } else if (!strcmp68(info->short_name,"iff")) {
      ext = ".iff";
    } else if (!strcmp68(info->short_name,"mp3")) {
      ext = ".mp3";
    } else if (!strcmp68(info->short_name,"ogg")) {
      ext = ".ogg";
    } else if (!strcmp68(info->short_name,"raw")) {
      ext = ".raw";
    }
#endif

    strcpy(is->defoutname,"sc68");
    strcat68(is->defoutname,ext,sizeof(is->defoutname)-1);
    is->outname = is->defoutname;
    TRACE68(ao68_cat,
            "libao68: open -- default *FILENAME* '%s'\n", is->outname);
  }

  if (is->ao.options) {
    int i;
    ao_option * o;
    TRACE68(ao68_cat,"%s\n", "libao68: options summary:");
    for (i=1, o=is->ao.options; o; o=o->next, ++i) {
      TRACE68(ao68_cat,
              "libao68:  - #%d %-12s = '%s'\n", i, o->key, o->value);
    }
  }

  if (!is->ao.format.rate) {
    is->ao.format.rate = vfs68_ao_defaut_rate;
    TRACE68(ao68_cat,"libao68: using default sampling rate\n");
  }
  dump_ao_format(&is->ao.format);
  is->ao.device =
    (info->type==AO_TYPE_LIVE)
    ? ao_open_live(is->ao.driver_id, &is->ao.format, is->ao.options)
    : ao_open_file(is->ao.driver_id, is->outname, !is->no_overwrite,
                   &is->ao.format, is->ao.options);
  if (!is->ao.device) {
    goto error;
  }

  if (info->type == AO_TYPE_LIVE) {
    msg68_notice("libao68: live driver -- *%s*\n", info->short_name);
  } else {
    msg68_notice("libao68: file driver -- *%s* -- '%s''\n",
                 info->short_name, is->outname);
  }
  dump_ao_format(&is->ao.format);

  is->count = 0;
  err = 0;
error:
  TRACE68(ao68_cat, "libao68: open -- *%s*\n", strok68(err));
  return err;
}

static int isao_close(vfs68_t * vfs)
{
  vfs68_ao_t * is = (vfs68_ao_t *)vfs;
  int err = -1;

  if (is->ao.options) {
    ao_free_options(is->ao.options);
    is->ao.options = 0;
  }
  if (is->ao.device) {
    ao_close(is->ao.device);
    is->ao.device = 0;
    err = 0;
  }
  TRACE68(ao68_cat, "libao68: close -- *%s*\n", strok68(err));
  return err;
}

static int isao_read(vfs68_t * vfs, void * data, int n)
{
  return -1;
}

static int isao_write(vfs68_t * vfs, const void * data, int n)
{
  vfs68_ao_t * isao = (vfs68_ao_t *)vfs;

  if (!isao || !isao->ao.device) {
    return -1;
  }
  if (!n) {
    return 0;
  }
  if (n<0) {
    return 0;
  }
  if (ao_play(isao->ao.device, (void *)data, n)) {
    isao->count += n;
    return n;
  } else {
    return -1;
  }
}

static int isao_flush(vfs68_t * vfs)
{
  vfs68_ao_t * isao = (vfs68_ao_t *)vfs;
  return !isao->ao.device
    ? -1
    : 0
    ;
}

static int isao_length(vfs68_t * vfs)
{
  vfs68_ao_t * isao = (vfs68_ao_t *)vfs;
  return !isao->ao.device
    ? -1
    : isao->count
    ;
}

static int isao_tell(vfs68_t * vfs)
{
  return isao_length(vfs);
}

static int isao_seek(vfs68_t * vfs, int offset)
{
  return -1;
}

static void isao_destroy(vfs68_t * vfs)
{
  TRACE68(ao68_cat, "libao68: destroy -- '%s'\n", vfs68_filename(vfs));
  if (&ao_unic_vfs->vfs == vfs)
    ao_unic_vfs = 0;
  vfs68_ao_shutdown();
  free(vfs);
}

static const vfs68_t vfs68_ao = {
  isao_name,
  isao_open, isao_close,
  isao_read, isao_write, isao_flush,
  isao_length, isao_tell, isao_seek, isao_seek,
  isao_destroy
};

/* ao stream URL is composed by /key=value pairs.
 *
 * Specific keys are:
 *
 *  - driver=[null|alsa|pulse|oss|esd|au|raw|wav ...]
 *  - rate=hz
 *  - format=[endianess][sign][channels][format]
 *    with (first value is the default)
 *        - endian   := [n|l|b]
 *        - sign     := [-|+]
 *        - channels := [1|2]
 *        - format   := [W|B|F]
 *
 * Other keys will be used as ao driver options.
 *
 */
vfs68_t * vfs68_ao_create(const char * uri, int mode, int spr)
{
  vfs68_ao_t *isf=0;
  int len;
  ao68_info_t ao;

  TRACE68(ao68_cat,"libao68: create -- '%s' (%d) %dhz\n",
          strnevernull68(uri), mode, spr);

  if (!init) {
    if (init_aolib() || !init) {
      msg68_critical("libao68: create error -- *libao*\n");
      goto error;
    }
  }

  if (!uri || !ao_ismine(uri)) {
    msg68_error("libao68: create error -- *parameter*\n");
    goto error;
  }

  if (mode != VFS68_OPEN_WRITE) {
    msg68_error("libao68: create error -- *mode*\n");
    goto error;
  }

  len = strlen(uri);
  isf = calloc(sizeof(vfs68_ao_t) + len, 1);
  if (!isf) {
    goto error;
  }


  /* -- Setup for default driver -- */
  memset(&ao,0,sizeof(ao));
  ao.default_id         = ao_default_driver_id();
  ao.driver_id          = ao.default_id;
  ao.format.bits        = 16;
  ao.format.channels    = 2;
  ao.format.rate        = spr ? spr_in_range(spr) : 0;
  ao.format.byte_format = AO_FMT_NATIVE;

  /* Copy vfs functions. */
  memcpy(&isf->vfs, &vfs68_ao, sizeof(vfs68_ao));
  /*   isf->mode = mode & (VFS68_OPEN_READ|VFS68_OPEN_WRITE); */
  isf->ao   = ao;
  strcpy(isf->name, uri);

error:
  if (!ao_unic_vfs)
    ao_unic_vfs = isf;
  uri = vfs68_filename(&isf->vfs);
  TRACE68(ao68_cat,"libao68: create -- *%s* -- '%s'\n",
          strok68(!isf),strnevernull68(uri));
  return isf ? &isf->vfs : 0;
}

static vfs68_t * ao_create(const char * uri, int mode, int argc, va_list list)
{
  unsigned int rate = 0;
  if (argc == 1)
    rate = va_arg(list,unsigned int);
  return vfs68_ao_create(uri, mode, rate);
}

#else /* #ifdef USE_AO */

/* vfs ao must not be include in this package. Anyway the creation
 * still exist but it always returns error.
 */

#include "file68_vfs_ao.h"
#include "file68_vfs_def.h"

int vfs68_ao_init(void)
{
  return 0;
}

void vfs68_ao_shutdown(void)
{
}

unsigned int audio68_sampling_rate(const unsigned int rate)
{
  return rate;
}

#endif
