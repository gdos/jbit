# File generated by genbsd.pl - DO NOT EDIT

#
# VARIABLES
#

CXXFLAGS ?= -fno-exceptions -fno-rtti -Wall -Os -fomit-frame-pointer
LDFLAGS ?= -s

DEVSYMS = d_xv65.h d_io2.h d_mio.h
OBJS = main.o devimpl.o cpu.o asm.o utils.o symdefs.o
EXE = jbit

#
# BSD DEFAULTS AND LOCAL PLATFORM OVERRIDE (tested on NetBSD 1.6.2)
#

HAS_LOCAL_MK = 0
.ifdef PLATFORM
.if $(PLATFORM) == "local"
HAS_LOCAL_MK = 1
.include "Local.mk"
.endif
.endif
.if $(HAS_LOCAL_MK) == 0

OBJS += stdout.o xv65.o

.endif

#
# RULES
#

all: $(EXE) jbit.1 BSD.mk

$(EXE): $(DEVSYMS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

jbit.1: jbit.ron
	ronn <jbit.ron >jbit.1

BSD.mk: Makefile
	perl ../tools/genbsd.pl <Makefile >BSD.mk

stdout.o: _xv65.h
xv65.o: _xv65.h

d_xv65.h: xv65.inc
	perl ../tools/inc2sym.pl <xv65.inc >d_xv65.h
	perl ../tools/inc2h.pl <xv65.inc >_xv65.h

d_io2.h: io2.inc
	perl ../tools/inc2sym.pl <io2.inc >d_io2.h
	perl ../tools/inc2h.pl <io2.inc >_io2.h

d_mio.h: mio.inc
	perl ../tools/inc2sym.pl <mio.inc >d_mio.h

touch:
	touch jbit.1 BSD.mk *.h

clean:
	rm -f jbit jbit.exe *.o *.bin

distclean: clean
	rm -f jbit.1
