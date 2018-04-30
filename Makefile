# See LICENSE file for copyright and license details
# slstatus - suckless status monitor
.POSIX:

include os.mk
include config.mk

REQ = util
COM =\
	components/$(OS)/battery \
	components/$(OS)/cpu \
	components/datetime\
	components/disk\
	components/$(OS)/entropy \
	components/hostname\
	components/ip\
	components/kernel_release\
	components/keyboard_indicators\
	components/load_avg\
	components/num_files\
	components/$(OS)/ram \
	components/run_command\
	components/$(OS)/swap \
	components/$(OS)/temperature \
	components/$(OS)/uptime \
	components/user\
	components/volume\
	components/$(OS)/wifi

SLSCFLAGS  = -I$(X11INC) -D_DEFAULT_SOURCE $(CFLAGS)
SLSLDFLAGS = -L$(X11LIB) $(LDFLAGS)
SLSLIBS    = -lX11 $(OSSLIBS) $(LIBS)

all: slstatus

slstatus: slstatus.o $(COM:=.o) $(REQ:=.o)
slstatus.o: slstatus.c slstatus.h arg.h config.h $(REQ:=.h)
$(COM:=.o): config.mk $(REQ:=.h)

config.h:
	cp config.def.h $@

.o:
	$(CC) -o $@ $(SLSLDFLAGS) $< $(COM:=.o) $(REQ:=.o) $(SLSLIBS)

.c.o:
	$(CC) -o $@ -c $(SLSCFLAGS) $<

clean:
	rm -f slstatus slstatus.o $(COM:=.o) $(REQ:=.o)

dist:
	rm -rf "slstatus-$(VERSION)"
	mkdir -p "slstatus-$(VERSION)/components"
	cp -R LICENSE Makefile README config.mk config.def.h \
	      arg.h slstatus.c $(COM:=.c) $(REQ:=.c) $(REQ:=.h) \
	      slstatus.1 "slstatus-$(VERSION)"
	tar -cf - "slstatus-$(VERSION)" | gzip -c > "slstatus-$(VERSION).tar.gz"
	rm -rf "slstatus-$(VERSION)"

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f slstatus "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/slstatus"
	mkdir -p "$(DESTDIR)$(MANPREFIX)/man1"
	cp -f slstatus.1 "$(DESTDIR)$(MANPREFIX)/man1"
	chmod 644 "$(DESTDIR)$(MANPREFIX)/man1/slstatus.1"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/slstatus"
	rm -f "$(DESTDIR)$(MANPREFIX)/man1/slstatus.1"
