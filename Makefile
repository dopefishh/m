SHELL = /bin/sh
include conf.mk

CFLAGS?=-Wall -Wpedantic -Wextra -Werror -g
LDFLAGS?=

VERSION:=0.1
PROGRAM:=m
OBJS:=$(patsubst %.h,%.o,\
		$(wildcard *.h) \
		$(wildcard config/*.h)\
	)

ifdef USE_FLAC
CFLAGS+=-DUSE_FLAC $(shell pkg-config --cflags flac)
LDFLAGS+=$(shell pkg-config --libs flac)
OBJS+=file/flac.o
endif

ifdef USE_OGG
CFLAGS+=-DUSE_OGG $(shell pkg-config --cflags vorbisfile)
LDFLAGS+=$(shell pkg-config --libs vorbisfile)
OBJS+=file/ogg.o
endif

ifdef USE_OPUS
CFLAGS+=-DUSE_OPUS $(shell pkg-config --cflags opusfile)
LDFLAGS+=$(shell pkg-config --libs opusfile)
OBJS+=file/opus.o
endif

ifdef USE_MP3
CFLAGS+=-DUSE_MP3 $(shell pkg-config --cflags id3tag)
LDFLAGS+=$(shell pkg-config --libs id3tag)
OBJS+=file/mp3.o id3map.o
endif

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

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
	$(RM) $(OBJS) $(PROGRAM) $(PROGRAM).1.gz
