AC_INIT([m], [0.1], [m@martlubbers.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror -Wno-portability])

# Flac support
AC_ARG_ENABLE([flac], AS_HELP_STRING([--disable-flac], [Disable flac support]))
AM_CONDITIONAL([USE_FLAC], [test "x$with_flac" != "xno"])
AM_COND_IF([USE_FLAC], [
	PKG_CHECK_MODULES([FLAC], [flac])
	AC_DEFINE([USE_FLAC], [], [Flac support])
])

# Opus support
AC_ARG_ENABLE([opus], AS_HELP_STRING([--disable-opus], [Disable opus support]))
AM_CONDITIONAL([USE_OPUS], [test "x$with_opus" != "xno"])
AM_COND_IF([USE_OPUS], [
	PKG_CHECK_MODULES([OPUS], [opusfile])
	AC_DEFINE([USE_OPUS], [], [Opus support])
])

# Ogg support
AC_ARG_ENABLE([ogg], AS_HELP_STRING([--disable-ogg], [Disable ogg support]))
AM_CONDITIONAL([USE_OGG], [test "x$with_ogg" != "xno"])
AM_COND_IF([USE_OGG], [
	PKG_CHECK_MODULES([OGG], [vorbisfile])
	AC_DEFINE([USE_OGG], [], [Ogg support])
])

# MP3 support
AC_ARG_ENABLE([mp3], AS_HELP_STRING([--disable-mp3], [Disable mp3 support]))
AM_CONDITIONAL([USE_MP3], [test "x$with_mp3" != "xno"])
AM_COND_IF([USE_MP3], [
	PKG_CHECK_MODULES([MP3], [id3tag])
	AC_DEFINE([USE_MP3], [], [MP3 support])
])

# Checks for libraries.
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([stdbool.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_CHECK_TYPE([int8_t int16_t, int32_t int64_t ssize_t])
AC_CHECK_TYPE([uint8_t uint16_t, uint32_t uint64_t size_t])

# Checks for library functions.
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([realpath])
AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strndup])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([malloc])
AC_CHECK_FUNCS([realloc])

# Output files.
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
