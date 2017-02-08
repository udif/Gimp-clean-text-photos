# Makefile
TGT	= fix-text-bg.exe
TGTD = fix-text-bg-dbg.exe
OBJS = Main.obj 
CC=cl

CFLAGS = # /LD
DEBUG = /Zi
RELEASE = /Ox
#INCLUDE =
#LIBS	=
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32 -lmingwex

.PHONY: all
all: $(TGT) $(TGTD)

$(TGT): Main.cpp
	$(CC) -c /FoRelease\Main.obj $(RELEASE) $(CFLAGS) $(LDFLAGS) $(INCLUDE) Main.cpp
	$(CC) $(CFLAGS) /Fe$@ Release\Main.obj $(LIBS)
	@echo Done.

$(TGTD): Main.cpp
	$(CC) -c /FoDebug\Main.obj $(DEBUG) $(CFLAGS) $(LDFLAGS) $(INCLUDE) Main.cpp
	$(CC) $(CFLAGS) /Fe$@ Debug\Main.obj $(LIBS)
	@echo Done.

#$(TGT): $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
#
#.cpp.obj:
#	$(CC) -c $(DEBUG) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $<

.PHONY: clean
clean:
	@del -f *.obj*
	@echo Cleaned.
