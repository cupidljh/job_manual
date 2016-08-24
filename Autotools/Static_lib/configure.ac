#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(Auto_SHDLC, 1.0, bug@naver.com)
AM_INIT_AUTOMAKE(Auto_SHDLC, 1.0)
AC_CONFIG_SRCDIR([run.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memmove])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_SUBDIRS([sava])
AC_OUTPUT
