SHELL=/bin/sh
include conf.mk

CFLAGS?=-Wall -Wpedantic -Wextra -D_DEFAULT_SOURCE
LDFLAGS?=-lm

ifdef DEBUG
CFLAGS+=-g
#YFLAGS+=-t
else
CFLAGS+=-O3
endif

VERSION:=0.1
PROGRAM:=m

PARSERS:=format/format.a
OBJS:= \
	config.o \
	config/print.o \
	config/search.o \
	config/update.o \
	db.o \
	db/io.o \
	db/shadow.o \
	exclude.o \
	file.o \
	format.o \
	list.o \
	log.o \
	m.o \
	search.o \
	util.o \
	xdg.o

ifeq ($(USE_FLAC),yes)
CFLAGS+=-DUSE_FLAC $(shell pkg-config --cflags flac)
LDFLAGS+=$(shell pkg-config --libs flac)
OBJS+=file/flac.o
endif

ifeq ($(USE_OGG),yes)
CFLAGS+=-DUSE_OGG $(shell pkg-config --cflags vorbisfile)
LDFLAGS+=$(shell pkg-config --libs vorbisfile)
OBJS+=file/ogg.o
endif

ifeq ($(USE_OPUS),yes)
CFLAGS+=-DUSE_OPUS $(shell pkg-config --cflags opusfile)
LDFLAGS+=$(shell pkg-config --libs opusfile)
OBJS+=file/opus.o
endif

ifeq ($(USE_MP3),yes)
CFLAGS+=-DUSE_MP3 $(shell pkg-config --cflags id3tag)
LDFLAGS+=$(shell pkg-config --libs id3tag)
OBJS+=file/mp3.o id3map.o
endif

all: $(PROGRAM)

format.o: format/format.a

$(PROGRAM): $(OBJS) $(PARSERS) 
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

../tests/%: $(filter-out m.o,$(OBJS)) $(PARSERS) 
	$(LINK.c) $(LDLIBS) $^ $@.c $(OUTPUT_OPTION)

%.a: %.tab.o %.yy.o
	$(AR) cr $@ $^

%.tab.c %.tab.h: %.y
	$(YACC.y) -b $* -d -Dapi.prefix={$(notdir $*)yy} $<

%.yy.c: %.l
	$(LEX) --header=$*.yy.h -P $(notdir $*)yy $(OUTPUT_OPTION) $<

%.1.gz: %
	help2man -n m -s 1 -m User\ Commands ./$< | gzip -9 > $@

install: m m.1.gz
	$(INSTALL_PROGRAM) m $(DESTDIR)$(BINDIR)/m
	$(INSTALL_DATA) README $(DESTDIR)$(DOCDIR)/README
	$(INSTALL_DATA) config.example $(DESTDIR)$(DOCDIR)/examples/config
	$(INSTALL_DATA) m.1.gz $(DESTDIR)$(MANDIR)/man1/m.1.gz

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/m\
		$(DESTDIR)$(DOCDIR)/README\
		$(DESTDIR)$(DOCDIR)/examples/config\
		$(DESTDIR)$(MANDIR)/man1/m.1.gz

maintainer-clean: clean

mostlyclean: clean

dist:
	tar --create --gzip --file $(PROGRAM)-$(VERSION).tar.gz \
		--transform 's/^/$(PROGRAM)-$(VERSION)\//' \
		*.c *.h README.md INSTALL COPYING

distclean: clean
	$(RM) $(PROGRAM)-$(VERSION).tar.gz

clean:
	$(RM) $(PARSERS) $(PARSERS:%.a=%.tab.[och]) $(PARSERS:%.a=%.yy.[och]) $(OBJS) $(PROGRAM) $(PROGRAM).1.gz
