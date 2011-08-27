# ,----------------------------------------------------------------------.
# |                                                                      |
# |  mksc68 -- Makefile.am -- Use with automake to produce Makefile.in   |
# |                                                                      |
# |                 (C) 2006-2011 Benjamin Gerard                        |
# |          <benjihan -4t- users.sourceforge -d0t- net>                 |
# |                                                                      |
# `----------------------------------------------------------------------'
# $Id: Makefile.am 26 2009-01-28 08:11:43Z benjihan $
# @configure_input@

SUBDIRS              = $(subdirs)
ACLOCAL_AMFLAGS      = -I m4 --install

AM_CFLAGS            = $(ALL_CFLAGS) $(BIN_CFLAGS)
AM_CPPFLAGS          = $(ALL_EFLAGS) $(BIN_EFLAGS) $(EFLAGS)
AM_LDFLAGS           = $(ALL_LFLAGS) $(BIN_LFLAGS)
EFLAGS               = $(sc68_inc) $(file68_inc) $(desa68_inc)

# ,----------------------------------------------------------------------.
# | Libtool program                                                      |
# `----------------------------------------------------------------------'

# if SOURCE_DESA68
# DESA68_LA  = $(desa68_dir)/libdesa68.la
# @SET_MAKE@
# $(desa68_dir)/libdesa68.la:
# 	cd "$(desa68_dir)" && $(MAKE) $(AM_MAKEFLAGS) libdesa68.la
# else
# DESA68_LA  = 
# endif

if SOURCE_SC68
SC68_LA = $(sc68_dir)/libsc68.la
@SET_MAKE@
$(sc68_dir)/libsc68.la:
	cd "$(sc68_dir)" && $(MAKE) $(AM_MAKEFLAGS) libsc68.la
else
SC68_LA =
endif

sources = mksc68.c mksc68_opt.c mksc68_msg.c mksc68_cmd.c mksc68_cli.c	\
mksc68_dsk.c mksc68_str.c mksc68_tag.c mksc68_emu.c mksc68_cmd_new.c	\
mksc68_cmd_load.c mksc68_cmd_play.c mksc68_cmd_time.c mksc68_cmd_tag.c	\
mksc68_cmd_dbg.c
header  =

bin_PROGRAMS         = mksc68
mksc68_SOURCES       = $(sources) $(headers)
mksc68_LDADD         = $(SC68_LA) $(DESA68_LA)
noinst_HEADERS       = mksc68_doc.h

# ,----------------------------------------------------------------------.
# | Documentation                                                        |
# `----------------------------------------------------------------------'

dist_man_MANS        = mksc68.1
info_TEXINFOS        = mksc68.texi
MAINTAINERCLEANFILES = $(srcdir)/mksc68.1

TEXINFO2MAN          = $(SHELL) $(top_srcdir)/missing --run texinfo2man
fail                 = { test -e '$@' && rm '$@'; test a = b; }

$(srcdir)/mksc68.1: mksc68.1.tpl.in $(srcdir)/mksc68.texi
	$(TEXINFO2MAN) mksc68.1.tpl $(srcdir)/mksc68.texi >$@ || $(fail)