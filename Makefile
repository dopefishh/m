include conf.mk

CFLAGS?=-Wall -Wextra -Werror -g
LDFLAGS?=
PROGRAM:=m
OBJS:=file.o db.o m.o util.o xdg.o parse.o config.o log.o

ifdef USE_FLAC
CFLAGS+=-DUSE_FLAC $(shell pkg-config --cflags flac)
LDFLAGS+=$(shell pkg-config --libs flac)
OBJS+=file_flac.o
endif

ifdef USE_OGG
CFLAGS+=-DUSE_OGG $(shell pkg-config --cflags vorbisfile)
LDFLAGS+=$(shell pkg-config --libs vorbisfile)
OBJS+=file_ogg.o
endif

ifdef USE_OPUS
CFLAGS+=-DUSE_OPUS $(shell pkg-config --cflags opusfile)
LDFLAGS+=$(shell pkg-config --libs opusfile)
OBJS+=file_opus.o
endif

ifdef USE_MP3
CFLAGS+=-DUSE_MP3 $(shell pkg-config --cflags id3tag)
LDFLAGS+=$(shell pkg-config --libs id3tag)
OBJS+=file_mp3.o
endif

all: $(PROGRAM)

$(PROGRAM): $(HEADERS) $(OBJS)
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

clean:
	$(RM) $(OBJS) $(PROGRAM)
