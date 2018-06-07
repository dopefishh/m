SHELL=/bin/sh

VERSION:=0.1
PROGRAM:=m

all: $(PROGRAM)

$(PROGRAM): src/$(PROGRAM)
	mv $< $@

src/$(PROGRAM):
	$(MAKE) -C $(dir $@) m

%.1.gz: %
	help2man -n m -s 1 -m User\ Commands ./$< | gzip -9 > $@

install: m m.1.gz
	$(INSTALL_PROGRAM) m $(DESTDIR)$(BINDIR)/m
	$(INSTALL_DATA) README.md $(DESTDIR)$(DOCDIR)/README.md
	$(INSTALL_DATA) doc/config.example $(DESTDIR)$(DOCDIR)/examples/config
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
