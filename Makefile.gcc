
CURRENT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TARGET := riscv64-unknown-linux-gnu
CC := $(TARGET)-gcc
LD := $(TARGET)-gcc
OBJCOPY := $(TARGET)-objcopy

# See comments in lualib/Makefile for why __ISO_C_VISIBLE=1999 is needed
CFLAGS := -D__ISO_C_VISIBLE=1999 -fPIC -O3 -fno-builtin -nostdinc -nostdlib -nostartfiles -fvisibility=hidden -fdata-sections -ffunction-sections -I lualib -I lualib/c-stdlib -I include/ckb-c-stdlib -I include/ckb-c-stdlib/libc -I include/ckb-c-stdlib/molecule -Wall -Werror -Wno-nonnull -Wno-nonnull-compare -Wno-unused-function -Wno-error=maybe-uninitialized -g

LDFLAGS := -nostdlib -nostartfiles -fno-builtin -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections

DOCKER_USER := $(shell id -u):$(shell id -g)
DOCKER_EXTRA_FLAGS ?=
# docker pull nervos/ckb-riscv-gnu-toolchain:gnu-bionic-20191012
BUILDER_DOCKER := nervos/ckb-riscv-gnu-toolchain@sha256:aae8a3f79705f67d505d1f1d5ddc694a4fd537ed1c7e9622420a470d59ba2ec3
PORT ?= 9999

all: lualib/liblua.a build/lua-loader build/libckblua.so build/dylibtest build/dylibexample build/spawnexample

all-via-docker:
	docker run --rm -v `pwd`:/code ${BUILDER_DOCKER} bash -c "cd /code && make"

docker-interactive:
	docker run --user ${DOCKER_USER} --rm -it -v "${CURRENT_DIR}:/code" --workdir /code --entrypoint /bin/bash ${DOCKER_EXTRA_FLAGS} ${BUILDER_DOCKER}

lualib/liblua.a:
	make -C lualib liblua.a

build/dylibtest: tests/test_cases/dylibtest.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< lualib/liblua.a $(shell $(CC) --print-search-dirs | sed -n '/install:/p' | sed 's/install:\s*//g')libgcc.a 

build/dylibexample: examples/dylib.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< lualib/liblua.a $(shell $(CC) --print-search-dirs | sed -n '/install:/p' | sed 's/install:\s*//g')libgcc.a

build/spawnexample: examples/spawn.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< lualib/liblua.a $(shell $(CC) --print-search-dirs | sed -n '/install:/p' | sed 's/install:\s*//g')libgcc.a

build/lua-loader.o: lua-loader/lua-loader.c
	$(CC) -c $(CFLAGS) -o $@ $<

# We need the soft floating point number support from libgcc.
# Note when -nostdlib is specified, libgcc is not linked to the program automatically.
# Also note libgcc.a must be appended to the file list, simply -lgcc does not for some reason.
# It seems gcc does not search libgcc in the install path.
build/lua-loader: build/lua-loader.o lualib/liblua.a
	$(LD) $(LDFLAGS) -o $@ $^ $(shell $(CC) --print-search-dirs | sed -n '/install:/p' | sed 's/install:\s*//g')libgcc.a
	cp $@ $@.debug
	$(OBJCOPY) --strip-debug --strip-all $@

build/libckblua.so: build/lua-loader.o lualib/liblua.a
	$(LD) $(LDFLAGS) -Wl,--dynamic-list lua-loader/libckblua.syms -fpic -shared -o $@ $^ $(shell $(CC) --print-search-dirs | sed -n '/install:/p' | sed 's/install:\s*//g')libgcc.a
	cp $@ $@.debug
	$(OBJCOPY) --strip-debug --strip-all $@

fmt:
	clang-format -style="{BasedOnStyle: google, IndentWidth: 4, SortIncludes: false}" -i lualib/*.c lualib/*.h lua-loader/*.h lua-loader/*.c include/*.c include/*.h tests/test_cases/*.c

clean-local:
	rm -f build/*.o
	rm -f build/lua-loader
	rm -f build/lua-loader*
	rm -f build/libckblua*
	rm -f build/dylibtest
	rm -f build/dylibexample
	rm -f build/spawnexample*

clean: clean-local
	make -C lualib clean

run-docker:
	docker run --rm --network host -it -v `pwd`:/code ${BUILDER_DOCKER} bash

run-gdb:
	riscv64-unknown-linux-gnu-gdb -ex "target remote 127.0.0.1:${PORT}" build/lua-loader.debug
