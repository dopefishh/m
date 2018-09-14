bin_PROGRAMS = m
noinst_LTLIBRARIES = libmm.la

m_SOURCES = config.c config/print.c config/search.c config/update.c \
	db.c db/io.c db/shadow.c \
	exclude.c \
	file.c \
	format.c format/format.tab.c format/format.yy.c \
	list.c \
	log.c \
	m.c \
	search.c \
	util.c \
	xdg.c
if USE_FLAC
m_SOURCES += file/flac.c
endif
if USE_OPUS
m_SOURCES += file/opus.c
endif
if USE_OGG
m_SOURCES += file/ogg.c
endif
if USE_MP3
m_SOURCES += file/mp3.c id3map.c
endif

m_CFLAGS = -Wall -Wpedantic -Wextra -D_DEFAULT_SOURCE \
	$(FLAC_CFLAGS) $(OPUS_CFLAGS) $(OGG_CFLAGS) $(MP3_CFLAGS)
m_LDADD = -lm $(FLAC_LIBS) $(OPUS_LIBS) $(OGG_LIBS) $(MP3_LIBS)

libmm_la_SOURCES = $(m_SOURCES)
libmm_la_LIBADD = $(m_LDADD)
libmm_la_CFLAGS = $(m_CFLAGS)

%.tab.c: %.y
	$(YACC.y) -b $(basename $<) -d -Dapi.prefix={$(notdir $(basename $<))yy} $<

%.yy.c: %.l
	$(LEX) --header=$(basename $<).yy.h -P $(notdir $(basename $<))yy $(OUTPUT_OPTION) $<

$(m_OBJECTS): format/format.yy.c format/format.tab.c

clean-local:
	$(RM) format/format.tab.[ch] format/format.yy.[ch]
