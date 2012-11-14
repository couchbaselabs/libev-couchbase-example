AC_INIT(server, 2.0)

AC_CONFIG_SRCDIR([server.c])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE(subdir-objects foreign)
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LN_S
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AM_SILENT_RULES([yes])

AC_SEARCH_LIBS(dlopen, dl)
AC_CHECK_LIB([couchbase], [lcb_set_http_data_callback], [],
             [AC_MSG_ERROR(Failed to locate libcouchbase >= 2.0.0)])

AC_CHECK_HEADERS_ONCE([unistd.h fcntl.h stdio.h stdlib.h string.h errno.h netinet/in.h inttypes.h])

AH_TOP([
#ifndef CONFIG_H
#define CONFIG_H
])

AH_BOTTOM([
#include "config_static.h"
#endif
])

m4_include([libev.m4])

AC_OUTPUT

