CFLAGS:=-Wall -Wextra -Werror -g
LDFLAGS:=-lFLAC -lvorbisfile
PROGRAM:=m
OBJS:=file.o db.o m.o util.o xdg.o parse.o config.o log.o file_flac.o file_ogg.o
HEADERS:=cfg.h

all: $(PROGRAM)

$(PROGRAM): $(HEADERS) $(OBJS)
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

clean:
	$(RM) $(OBJS) $(PROGRAM)
