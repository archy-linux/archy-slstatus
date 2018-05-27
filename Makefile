# See LICENSE file for copyright and license details
# slstatus - suckless status monitor
.POSIX:

include config.mk

REQ = util
COM =\
	components/battery\
	components/cpu\
	components/datetime\
	components/disk\
	components/entropy\
	components/hostname\
	components/ip\
	components/kernel_release\
	components/keyboard_indicators\
	components/keymap\
	components/load_avg\
	components/netspeeds\
	components/num_files\
	components/ram\
	components/run_command\
	components/swap\
	components/temperature\
	components/uptime\
	components/user\
	components/volume\
	components/wifi

all: slstatus

components/battery.o: components/battery.c config.mk $(REQ:=.h)
components/cpu.o: components/cpu.c config.mk $(REQ:=.h)
components/datetime.o: components/datetime.c config.mk $(REQ:=.h)
components/disk.o: components/disk.c config.mk $(REQ:=.h)
components/entropy.o: components/entropy.c config.mk $(REQ:=.h)
components/hostname.o: components/hostname.c config.mk $(REQ:=.h)
components/ip.o: components/ip.c config.mk $(REQ:=.h)
components/kernel_release.o: components/kernel_release.c config.mk $(REQ:=.h)
components/keyboard_indicators.o: components/keyboard_indicators.c config.mk $(REQ:=.h)
components/keymap.o: components/keymap.c config.mk $(REQ:=.h)
components/load_avg.o: components/load_avg.c config.mk $(REQ:=.h)
components/netspeeds.o: components/netspeeds.c config.mk $(REQ:=.h)
components/num_files.o: components/num_files.c config.mk $(REQ:=.h)
components/ram.o: components/ram.c config.mk $(REQ:=.h)
components/run_command.o: components/run_command.c config.mk $(REQ:=.h)
components/swap.o: components/swap.c config.mk $(REQ:=.h)
components/temperature.o: components/temperature.c config.mk $(REQ:=.h)
components/uptime.o: components/uptime.c config.mk $(REQ:=.h)
components/user.o: components/user.c config.mk $(REQ:=.h)
components/volume.o: components/volume.c config.mk $(REQ:=.h)
components/wifi.o: components/wifi.c config.mk $(REQ:=.h)
slstatus.o: slstatus.c slstatus.h arg.h config.h config.mk $(REQ:=.h)

.c.o:
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

config.h:
	cp config.def.h $@

slstatus: slstatus.o $(COM:=.o) $(REQ:=.o)
	$(CC) -o $@ $(LDFLAGS) $< $(COM:=.o) $(REQ:=.o) $(LDLIBS)

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
