
# Variables
PKGS = fuse libxml-2.0 libcurl libapp libjson
FLAGS = $(shell pkg-config ${PKGS} --cflags)  ${CFLAGS}
LIBS = $(shell pkg-config ${PKGS} --libs) -lpthread
OBJS = boxfs.o boxapi.o boxpath.o boxhttp.o boxopts.o boxjson.o boxcache.o boxutils.o
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
DEPS = vincenthz/libjson drotiro/libapp

.PHONY: clean install check_pkg deps

# Targets
boxfs: $(OBJS) 
	@echo "Building  $@"
	$(CC) $(FLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	@echo Compiling $<
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) *~ boxfs

install: boxfs
	install -s boxfs $(BINDIR)
	install boxfs-init $(BINDIR)

deps:
	@$(foreach i,$(DEPS), git clone https://github.com/$i && make -C `basename $i` install; )

# Check required programs
PKG_CONFIG_VER := $(shell pkg-config --version 2>/dev/null)
ifndef PKG_CONFIG_VER
$(error Please install pkg-config)
endif

# Dependencies
# (gcc -MM *.c  -D_FILE_OFFSET_BITS=64)
boxapi.o: boxapi.c boxapi.h boxpath.h boxjson.h boxhttp.h boxopts.h boxutils.h boxcache.h
boxcache.o: boxcache.c boxcache.h boxutils.h
boxfs.o: boxfs.c boxapi.h
boxhttp.o: boxhttp.c boxhttp.h boxapi.h boxopts.h boxutils.h
boxjson.o: boxjson.c boxjson.h
boxopts.o: boxopts.c boxapi.h boxopts.h
boxpath.o: boxpath.c boxpath.h boxjson.h boxapi.h boxopts.h boxutils.h
boxutils.o: boxutils.c boxutils.h
