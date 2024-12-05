CURRENT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ifeq ($(origin LLVM_VERSION),undefined)
	LLVM_VERSION = 18
endif

LLVM_SUFFIX = $(if $(LLVM_VERSION),-$(LLVM_VERSION),)
CC := clang${LLVM_SUFFIX}
LD := ld.lld${LLVM_SUFFIX}
OBJCOPY := llvm-objcopy${LLVM_SUFFIX}
AR := llvm-ar${LLVM_SUFFIX}
RANLIB := llvm-ranlib${LLVM_SUFFIX}

CFLAGS :=	--target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs \
			-g -O3 -nostdinc -fdata-sections -ffunction-sections

CFLAGS += -D__ISO_C_VISIBLE=1999 -DCKB_DECLARATION_ONLY -DCKB_MALLOC_DECLARATION_ONLY -DCKB_PRINTF_DECLARATION_ONLY
CFLAGS += -isystem deps/musl/release/include/ -Ideps/ckb  -Ic-stdlib
CFLAGS += -I include/ckb-c-stdlib/molecule -I include
CFLAGS += -I lualib -I lualib/c-stdlib

LDFLAGS= -static --gc-sections --nostdlib --sysroot deps/musl/release \
	-Ldeps/musl/release/lib -Ldeps/compiler-rt-builtins-riscv/build -lc -lgcc -lcompiler-rt

LDFLAGS += -wrap=fclose -wrap=fopen -wrap=freopen -wrap=getc -wrap=fread -wrap=fseek -wrap=ftell -wrap=feof -wrap=ferror

LICOMPILER_RT_CFLGAS = \
	--target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs -mabi=lp64 \
	-nostdinc \
	-I ../musl/release/include -I ../ckb-libcxx-builder/release/include \
	-Os \
	-fdata-sections -ffunction-sections -fno-builtin -fvisibility=hidden -fomit-frame-pointer \
	-I compiler-rt/lib/builtins \
	-DVISIBILITY_HIDDEN -DCOMPILER_RT_HAS_FLOAT16

# docker pull docker.io/cryptape/llvm-n-rust:20240630
DOCKER_IMAGE := docker.io/cryptape/llvm-n-rust@sha256:bafaf76d4f342a69b8691c08e77a330b7740631f3d1d9c9bee4ead521b29ee55

all: lualib/liblua.a build/lua-loader build/spawnexample

all-via-docker:
	docker run --rm -v `pwd`:/code ${DOCKER_IMAGE} bash -c "cd /code && make"

deps/musl/release/:
	cd deps/musl/src/stdio
	cd deps/musl/ && CLANG=$(CC) DISABLE_STD_FILE=true ./ckb/build.sh

deps/ckb-libcxx-builder/release/:
	cd deps/ckb-libcxx-builder && mkdir -p release && LLVM_VERSION= CLANG=$(CC) ./build.sh

deps/compiler-rt-builtins-riscv/build/libcompiler-rt.a: deps/musl/release/ deps/ckb-libcxx-builder/release/
	cd deps/compiler-rt-builtins-riscv && \
		make CC=$(CC) LD=$(LD) OBJCOPY=$(OBJCOPY) AR=$(AR) RANLIB=$(RANLIB) CFLAGS="$(LICOMPILER_RT_CFLGAS)"

lualib/liblua.a: deps/compiler-rt-builtins-riscv/build/libcompiler-rt.a
	make -C lualib -f Makefile CC=$(CC) LD=$(LD) OBJCOPY=$(OBJCOPY) AR="$(AR) rc" RANLIB=$(RANLIB) liblua.a

build/spawnexample.o: examples/spawn.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/spawnexample: build/spawnexample.o deps/compiler-rt-builtins-riscv/build/libcompiler-rt.a
	$(LD) $(LDFLAGS) -o $@ $^
	cp $@ $@.debug
	$(OBJCOPY) --strip-debug --strip-all $@

build/lua-loader.o: lua-loader/lua-loader.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/lua-loader: build/lua-loader.o lualib/liblua.a deps/compiler-rt-builtins-riscv/build/libcompiler-rt.a
	$(LD) $(LDFLAGS) -o $@ $^
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

clean-deps: clean
	make -C deps/musl/ clean
	rm -rf deps/musl/release/
	rm -rf deps/musl/config.mak
	rm -rf deps/compiler-rt-builtins-riscv/build/libcompiler-rt.a
	rm -rf deps/ckb-libcxx-builder/release/*

clean: clean-local
	make -C lualib clean

run-gdb:
	riscv64-unknown-linux-gnu-gdb -ex "target remote 127.0.0.1:${PORT}" build/lua-loader.debug
