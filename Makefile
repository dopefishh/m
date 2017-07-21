LIBRARIES:=flac vorbisfile opusfile
CFLAGS:=-Wall -Wextra -Werror -g $(shell pkg-config --cflags $(LIBRARIES))
LDFLAGS:=$(shell pkg-config --libs $(LIBRARIES))
PROGRAM:=m
OBJS:=file.o db.o m.o util.o xdg.o parse.o config.o log.o file_flac.o file_ogg.o file_opus.o
HEADERS:=cfg.h

all: $(PROGRAM)

$(PROGRAM): $(HEADERS) $(OBJS)
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

clean:
	$(RM) $(OBJS) $(PROGRAM)
