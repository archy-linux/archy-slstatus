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
SLSTATUS_OBJ = $(OBJ_DIR)/archy_slstatus.o

all: $(BUILD_DIR)/archy-slstatus

$(COM_OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(REQ_OBJ) config.mk $(REQ:%=$(SRC_DIR)/%.h)
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

$(REQ_OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

$(SLSTATUS_OBJ): $(SRC_DIR)/archy_slstatus.c $(SRC_DIR)/archy_slstatus.h $(SRC_DIR)/arg.h config.mk $(REQ_OBJ) $(REQ:%=$(SRC_DIR)/%.h)
	@mkdir -p $(@D)
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

$(BUILD_DIR)/archy-slstatus: $(SLSTATUS_OBJ) $(COM_OBJ) $(REQ_OBJ)
	$(CC) -o $@ $(LDFLAGS) $(SLSTATUS_OBJ) $(COM_OBJ) $(REQ_OBJ) $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)

dist: build/archy-slstatus
	mkdir -p "dist/archy-slstatus_$(VERSION)"
	cp -R LICENSE README.md $(BUILD_DIR)/archy-slstatus archy-slstatus.1 "dist/archy-slstatus_$(VERSION)"
	tar -cf - "dist/archy-slstatus_$(VERSION)" | gzip -c > "archy-slstatus_$(VERSION).tar.gz"
	rm -rf "dist/archy-slstatus_$(VERSION)"
	mv "archy-slstatus_$(VERSION).tar.gz" "dist/archy-slstatus_$(VERSION).tar.gz"
	echo "The tarball is ready."
	printf "Tarball size: %s\n" $$(du -h dist/archy-slstatus_${VERSION}.tar.gz | cut -f1)
	printf "Tarball hash: %s\n" $$(sha256sum dist/archy-slstatus_${VERSION}.tar.gz)

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f $(BUILD_DIR)/archy-slstatus "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/archy-slstatus"
	mkdir -p "$(DESTDIR)$(MANPREFIX)/man1"
	cp -f archy-slstatus.1 "$(DESTDIR)$(MANPREFIX)/man1"
	chmod 644 "$(DESTDIR)$(MANPREFIX)/man1/archy-slstatus.1"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/archy-slstatus"
	rm -f "$(DESTDIR)$(MANPREFIX)/man1/archy-slstatus.1"

.PHONY: all clean dist install uninstall
