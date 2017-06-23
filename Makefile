PROGRAM:=m
OBJS:=file.o db.o m.o util.o xdg.o

all: $(PROGRAM)

$(PROGRAM): $(HEADERS) $(OBJS)
	$(LINK.c) $(LDLIBS) $^ $(OUTPUT_OPTION)

clean:
	$(RM) $(OBJS) $(PROGRAM)
