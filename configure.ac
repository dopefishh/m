AC_INIT([m], [0.1], [m@martlubbers.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_ARG_ENABLE([flac], AS_HELP_STRING([--disable-flac], [Disable flac support]))
AS_IF([test "x$with_flac" != "xno"], [
	PKG_CHECK_MODULES([FLAC], [flac])
	LDFLAGS="$LDFLAGS $FLAC_LIBS"
	CFLAGS="$CFLAGS $FLAC_CFLAGS"
])
AC_ARG_ENABLE([opus], AS_HELP_STRING([--disable-opus], [Disable opus support]))
AS_IF([test "x$with_opus" != "xno"], [PKG_CHECK_MODULES([OPUS], [opusfile])])
AC_ARG_ENABLE([ogg], AS_HELP_STRING([--disable-ogg], [Disable ogg support]))
AS_IF([test "x$with_ogg" != "xno"], [PKG_CHECK_MODULES([OGG], [vorbisfile])])
AC_ARG_ENABLE([mp3], AS_HELP_STRING([--disable-mp3], [Disable mp3 support]))
AS_IF([test "x$with_mp3" != "xno"], [PKG_CHECK_MODULES([MP3], [id3tag])])

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
