.POSIX:

include config.mk

REQ = util
COM = \
	components/battery \
	components/cpu \
	components/datetime \
	components/disk \
	components/entropy \
	components/hostname \
	components/ip \
	components/kernel_release \
	components/keyboard_indicators \
	components/keymap \
	components/load_avg \
	components/netspeeds \
	components/num_files \
	components/ram \
	components/run_command \
	components/separator \
	components/swap \
	components/temperature \
	components/uptime \
	components/user \
	components/volume \
	components/wifi

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
SRC_DIR = src

COM_OBJ = $(COM:%=$(OBJ_DIR)/%.o)
REQ_OBJ = $(REQ:%=$(OBJ_DIR)/%.o)
SLSTATUS_OBJ = $(OBJ_DIR)/slstatus.o

all: $(BUILD_DIR)/slstatus

$(COM_OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(REQ_OBJ) config.mk $(REQ:%=$(SRC_DIR)/%.h)
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

$(REQ_OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

$(SLSTATUS_OBJ): $(SRC_DIR)/slstatus.c $(SRC_DIR)/slstatus.h $(SRC_DIR)/arg.h config.mk $(REQ_OBJ) $(REQ:%=$(SRC_DIR)/%.h)
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

config.h:
	cp config.def.h $@

$(BUILD_DIR)/slstatus: $(SLSTATUS_OBJ) $(COM_OBJ) $(REQ_OBJ)
	$(CC) -o $@ $(LDFLAGS) $(SLSTATUS_OBJ) $(COM_OBJ) $(REQ_OBJ) $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)

dist:
	rm -rf "slstatus-$(VERSION)"
	mkdir -p "slstatus-$(VERSION)/components"
	cp -R LICENSE Makefile README config.mk config.def.h \
		arg.h $(SRC_DIR)/slstatus.c $(COM:%=$(SRC_DIR)/%.c) $(REQ:%=$(SRC_DIR)/%.c) $(REQ:%=$(SRC_DIR)/%.h) \
		slstatus.1 "slstatus-$(VERSION)"
	tar -cf - "slstatus-$(VERSION)" | gzip -c > "slstatus-$(VERSION).tar.gz"
	rm -rf "slstatus-$(VERSION)"

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f $(BUILD_DIR)/slstatus "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/slstatus"
	mkdir -p "$(DESTDIR)$(MANPREFIX)/man1"
	cp -f slstatus.1 "$(DESTDIR)$(MANPREFIX)/man1"
	chmod 644 "$(DESTDIR)$(MANPREFIX)/man1/slstatus.1"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/slstatus"
	rm -f "$(DESTDIR)$(MANPREFIX)/man1/slstatus.1"

.PHONY: all clean dist install uninstall
