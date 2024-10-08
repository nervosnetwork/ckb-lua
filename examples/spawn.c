#include <stdint.h>
#include <string.h>

#include "ckb_syscalls.h"

int main() {
    int err = 0;

    uint64_t rw_pipe[2] = {0};
    err = ckb_pipe(rw_pipe);
    if (err != 0) {
        return err;
    }

    const char *argv[] = {
        "-e",
        "local m = arg[2] .. arg[3]; \
       local inherited_fds, err = ckb.inherited_fds(); \
       local n, err = ckb.write(inherited_fds[1], m); \
       ckb.close(inherited_fds[1]); \
       assert(n == 10);\
       local pid = ckb.process_id(); \
       assert(pid == 1); \
        ",
        "hello",
        "world",
    };
    uint64_t pid = 0;
    uint64_t inherited_fds[2] = {rw_pipe[1], 0};
    spawn_args_t spgs = {
        .argc = 4,
        .argv = argv,
        .process_id = &pid,
        .inherited_fds = inherited_fds,
    };

    err = ckb_spawn(1, 3, 0, 0, &spgs);
    if (err != 0) {
        return err;
    }
    uint8_t buffer[80] = {0};
    uint64_t length = 80;
    err = ckb_read(rw_pipe[0], buffer, &length);
    if (err != 0) {
        return err;
    }
    int8_t exit = 0;
    err = ckb_wait(pid, &exit);
    if (err != 0) {
        return err;
    }
    if (exit != 0) {
      return 1;
    }
    if (strcmp((char *)buffer, "helloworld") != 0) {
        return 1;
    }
    return 0;
}
