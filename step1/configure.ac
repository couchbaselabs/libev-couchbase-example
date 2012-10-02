AC_INIT(server, 1.0)

AC_CONFIG_SRCDIR([server.c])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE(subdir-objects foreign)
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LN_S
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AM_SILENT_RULES([yes])
AM_CPPFLAGS="$AM_CPPFLAGS -I\${top_srcdir}/libev"

m4_include([libev.m4])

AC_OUTPUT

