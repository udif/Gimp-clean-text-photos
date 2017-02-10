# Makefile
TGT	= fix-text-bg.exe
OBJS = Main.obj 
CC=i686-w64-mingw32-gcc

CFLAGS = # /LD
DEBUG =  -g
RELEASE = -O2
INCLUDE = -I /mingw32/include/pango-1.0 -I /mingw32/include/cairo -I /usr/include/glib-2.0 -I /mingw32/include/gtk-2.0
#LIBS	=
#LIBS	+= -ladvapi32 -luser32 -lgdi32 -lkernel32 -lmingwex

.PHONY: all
all: $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

Main.obj:
	$(CC) -c $(DEBUG) $(CFLAGS) $(LDFLAGS) $(INCLUDE) Main.cpp

.PHONY: clean
clean:
	@del -f *.obj*
	@echo Cleaned.
