#!/usr/bin/make -f

# these can be overridden using make variables. e.g.
#   make CFLAGS=-O2
#   make install DESTDIR=$(CURDIR)/debian/midifalse.lv2 PREFIX=/usr

OPTIMIZATIONS ?= -msse -msse2 -mfpmath=sse -ffast-math -fomit-frame-pointer -O3 -fno-finite-math-only
PREFIX ?= /usr/local
CFLAGS ?= $(OPTIMIZATIONS) -Wall
LIBDIR ?= lib

STRIP?=strip
STRIPFLAGS?=-s

LIB_EXT=.so

LV2DIR ?= $(PREFIX)/$(LIBDIR)/lv2
LOADLIBES=-lm
LV2NAME=midifalse
BUNDLE=midifalse.lv2
BUILDDIR=build/
targets=

LV2LDFLAGS=-Wl,-Bstatic -Wl,-Bdynamic
override CFLAGS += -fPIC -std=c99

default: all

all: $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(targets) $(BUILDDIR)$(LV2NAME)$(LIB_EXT)

$(BUILDDIR)manifest.ttl: manifest.ttl.in
	sed "s/@LIB_EXT@/$(LIB_EXT)/g;s/@LV2NAME@/$(LV2NAME)/g" \
		manifest.ttl.in > $(BUILDDIR)manifest.ttl

$(BUILDDIR)$(LV2NAME).ttl: $(LV2NAME).ttl
	cp $(LV2NAME).ttl $(BUILDDIR)$(LV2NAME).ttl

$(BUILDDIR)$(LV2NAME)$(LIB_EXT): $(LV2NAME).c script.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		-o $(BUILDDIR)$(LV2NAME)$(LIB_EXT) $(LV2NAME).c \
		-shared $(LV2LDFLAGS) $(LDFLAGS) $(LOADLIBES)
	$(STRIP) $(STRIPFLAGS) $(BUILDDIR)$(LV2NAME)$(LIB_EXT)

install: all
	install -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m755 $(BUILDDIR)$(LV2NAME)$(LIB_EXT) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m644 $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(DESTDIR)$(LV2DIR)/$(BUNDLE)

clean:
	rm -f $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(targets) $(BUILDDIR)$(LV2NAME)$(LIB_EXT)
