TESTS = check_util check_list
check_PROGRAMS = check_util check_list

check_util_SOURCES = check_util.c $(top_builddir)/src/util.h
check_util_CFLAGS = $(CHECK_CFLAGS)
check_util_LDADD = $(top_builddir)/src/libm.la $(CHECK_LIBS)

check_list_SOURCES = check_list.c $(top_builddir)/src/list.h
check_list_CFLAGS = $(CHECK_CFLAGS)
check_list_LDADD = $(top_builddir)/src/libm.la $(CHECK_LIBS)
