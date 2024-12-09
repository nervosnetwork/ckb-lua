[![CI](https://github.com/nervosnetwork/ckb-lua-vm/actions/workflows/ci.yml/badge.svg)](https://github.com/nervosnetwork/ckb-lua-vm/actions/workflows/ci.yml)

# ckb-lua-vm
A project to help developers writing script in Lua on [CKB-VM](https://github.com/nervosnetwork/ckb-vm).


## Build

```bash
make all-via-docker
```

## Playground
There are a lot of tests located in 'tests' folder. Before running, please install ckb-debugger described in [CI scripts](.github/workflows/ci.yml).
Here is a "hello,world" example:

```bash
make -C tests/test_cases hello_world
```

## Usages

There are 2 ways to use this project:
1. Embedded (Recommended)

Use `build/libckblua.so` as a dynamic library. See [dylib.md](./docs/dylib.md) for a detailed tutorial on how to use `build/libckblua.so`.

2. Standalone

Use `build/lua-loader` as a script. Require hacking for further requirement.

## Command Line Arguments

A couple of arguments may be passed to ckb-lua-vm.
If no command line arguments is passed to ckb-lua-vm, ckb-lua-vm will run the script contained in cell data,
which is assumed to be a valid exectuable lua file.

The following are the supported command line arguments.
To test the `ADDITIONAL_ARGUMENTS` locally, we can run `ckb-debugger --bin ./build/lua-loader.debug -- ADDITIONAL_ARGUMENTS`

- `-e` is used to evaluate some lua script. For example, running `ckb-debugger --bin ./build/lua-loader.debug -- -e 'print("abcdefg")'` will print out `abcdefg` in to console.
- `-f` is used to enable [file system access](./docs/fs.md). For example, running `ckb-debugger --bin ./build/lua-loader.debug -- -f` would evaluate the `main.lua` file within the file system in the cell data.
- `-r` is used to execute coded loaded from ckb-debugger. For example, running `ckb-debugger ---read-file strings.lua --bin ./build/lua-loader.debug -- -r` will execute the lua file `strings.lua`. Normally, ckb-lua-vm can not read files from local file system, we add this parameter (along with the `--read-file` parameter of `ckb-debugger`) to facilitate testing of running local lua files.


## LLVM & GCC
The project uses LLVM for compilation by default but also supports GCC (via `Makefile.gcc`).
However, please note that LLVM does not support dynamic libraries, so GCC must be used in such cases.
