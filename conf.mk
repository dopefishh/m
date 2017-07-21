# Directories:
DESTDIR?=
INSTALL_PROGRAM?=install -D
INSTALL_DATA?=install -D

PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BINDIR?=$(PREFIX)/bin
SBINDIR?=$(PREFIX)/sbin
LIBEXECDIR?=$(PREFIX)/libexec
DATAROOTDIR?=$(PREFIX)/share
DATADIR?=$(DATAROOTDIR)
SYSCONFDIR?=$(PREFIX)/etc
SHAREDSTATEDIR?=$(PREFIX)/com
LOCALSTATEDIR?=$(PREFIX)/var
RUNSTATEDIR?=$(LOCALSTATEDIR)/run
INCLUDEDIR=$(PREFIX)/include
OLDINCLUDEDIR=/usr/include
DOCDIR=$(DATAROOTDIR)/doc/m
INFODIR=$(DATAROOTDIR)/info
HTMLDIR=$(DOCDIR)
DVIDIR=$(DOCDIR)
PDFDIR=$(DOCDIR)
PSDIR=$(DOCDIR)
LIBDIR=$(EXEC_PREFIX)/lib
LOCALEDIR=$(DATAROOTDIR)/locale
MANDIR=$(DATAROOTDIR)/man
MANEXT=.1
SRCDIR=.

# Set your library options here.
# A non-empty value means that it will be included
# Enable mp3 tagging support
USE_MP3:=yes
# Enable flac tagging support
USE_FLAC:=yes
# Enable opus tagging support
USE_OPUS:=yes
# Enable ogg tagging support
USE_OGG:=yes
