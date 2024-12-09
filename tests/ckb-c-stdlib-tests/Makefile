TARGET := riscv64-unknown-linux-gnu
CC := $(TARGET)-gcc
LD := $(TARGET)-gcc
OBJCOPY := $(TARGET)-objcopy
CFLAGS := \
	-fPIC -O3 -fno-builtin -nostdinc -nostdlib -nostartfiles -fvisibility=hidden -fdata-sections -ffunction-sections \
	-I ../../lualib/c-stdlib  -I ../../lualib/c-stdlib/src -I ../../lualib/c-stdlib/my_gnu -I ../../lualib/c-stdlib/my_gnu/src  -I ../../include/ckb-c-stdlib -I ../../include/ckb-c-stdlib/libc \
	-Wall -Werror -Wno-nonnull -Wno-nonnull-compare -Wno-unused-function -g
LDFLAGS := -nostartfiles -Wl,-static -Wl,--gc-sections

# docker pull nervos/ckb-riscv-gnu-toolchain:gnu-bionic-20191012
BUILDER_DOCKER := nervos/ckb-riscv-gnu-toolchain@sha256:aae8a3f79705f67d505d1f1d5ddc694a4fd537ed1c7e9622420a470d59ba2ec3
PORT := 9099


all: ../../build/test_malloc

all-via-docker: ${PROTOCOL_HEADER}
	docker run --rm -v `pwd`/../../:/code ${BUILDER_DOCKER} bash -c "cd /code/tests/ckb-c-stdlib-tests && make"

../../build/test_malloc: test_malloc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

fmt:
	clang-format -style="{BasedOnStyle: google, IndentWidth: 4}" -i c/*.c

run:
	RUST_LOG=debug ckb-debugger --bin ../../build/test_malloc

ci:
	RUST_LOG=debug ckb-debugger --bin ../../build/test_malloc 2>&1 | fgrep 'Run result: 0'

run-debugger:
	ckb-debugger --mode gdb --gdb-listen 127.0.0.1:${PORT} --bin ../../build/test_malloc

run-gdb:
	riscv64-unknown-linux-gnu-gdb -ex "target remote 127.0.0.1:${PORT}" ../../build/test_malloc

run-docker:
	docker run --net=host --rm -it -v `pwd`/../..:/code ${BUILDER_DOCKER} bash

clean:
	rm -f ../../build/test_malloc
