[![CI](https://github.com/nervosnetwork/ckb-lua/actions/workflows/ci.yml/badge.svg)](https://github.com/nervosnetwork/ckb-lua/actions/workflows/ci.yml)

# ckb-lua
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
