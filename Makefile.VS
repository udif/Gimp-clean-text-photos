# Makefile
TGT	= fix-text-bg.exe
TGTD = fix-text-bg-dbg.exe
OBJS = Main.obj 
CC=cl

CFLAGS = # /LD
DEBUG = /Zi -DGIMP_DEBUG
RELEASE = /Ox
INC = -I..\..\gimp-devel\include -Ic:/gtk+2.24/include/gtk-2.0 -Ic:/gtk+2.24/lib/gtk-2.0/include -Ic:/gtk+2.24/include/atk-1.0 -Ic:/gtk+2.24/include/cairo -Ic:/gtk+2.24/include/gdk-pixbuf-2.0 -Ic:/gtk+2.24/include/pango-1.0 -Ic:/gtk+2.24/include/glib-2.0 -Ic:/gtk+2.24/lib/glib-2.0/include -Ic:/gtk+2.24/include -Ic:/gtk+2.24/include/freetype2 -Ic:/gtk+2.24/include/libpng14
LIBS	= /link /subsystem:windows /libpath:\gtk+2.24\lib /libpath:..\..\gimp-devel\lib\x86 \
libgimp-2.0-0.lib libgimpwidgets-2.0-0.lib libgimpui-2.0-0.lib glib-2.0.lib gobject-2.0.lib gtk-win32-2.0.lib
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32 -lmingwex

.PHONY: all
all: $(TGT) $(TGTD)

$(TGT): Main.cpp
	$(CC) -c /FoRelease\Main.obj $(RELEASE) $(CFLAGS) $(LDFLAGS) /I"$(INCLUDE)" $(INC) Main.cpp
	$(CC) -c /FoRelease\fix-text-bg.obj $(RELEASE) $(CFLAGS) $(LDFLAGS) /I"$(INCLUDE)" $(INC) fix-text-bg.cpp
	$(CC) $(CFLAGS) /Fe$@ Release\Main.obj Release\fix-text-bg.obj $(LIBS)
	@echo Done.

$(TGTD): Main.cpp
	$(CC) -c /FoDebug\Main.obj   $(DEBUG)   $(CFLAGS) $(LDFLAGS) /I"$(INCLUDE)" $(INC) Main.cpp
	$(CC) -c /FoDebug\fix-text-bg.obj   $(DEBUG)   $(CFLAGS) $(LDFLAGS) /I"$(INCLUDE)" $(INC) fix-text-bg.cpp
	$(CC) $(CFLAGS) /Fe$@ Debug\Main.obj Debug\fix-text-bg.obj $(LIBS) /debug:full
	@echo Done.

#$(TGT): $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
#
#.cpp.obj:
#	$(CC) -c $(DEBUG) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $<

