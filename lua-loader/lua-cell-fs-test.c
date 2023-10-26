#include "lua-ckb.h"

int dochunk(lua_State *L, int status);

void exit(int c);

FSFile *ckb_must_get_file(char *filename) {
    FSFile *file = 0;
    if (ckb_get_file(filename, &file) != 0) {
        exit(-1);
    }
    return file;
}

// TODO: write to an IO stream instead of copying to a buffer.
int ckb_save_fs(const FSFile *files, uint32_t count, void *buf,
                uint64_t *buflen) {
    if (buflen == NULL && buf == NULL) {
        return -1;
    }

    uint32_t bytes_len = 0;

    bytes_len += sizeof(uint32_t);
    bytes_len += count * sizeof(FSEntry);

    uint32_t *metadata_start = buf;
    char *data_start = buf + bytes_len;

    for (uint32_t i = 0; i < count; i++) {
        FSFile file = files[i];
        bytes_len += strlen(file.filename) + 1;
        bytes_len += file.size;
    }

    if (buflen != NULL && buf != NULL && *buflen < bytes_len) {
        return -1;
    }

    if (buflen != NULL) {
        if (*buflen < bytes_len && buf != NULL) {
            *buflen = bytes_len;
            return -1;
        }
        *buflen = bytes_len;
    }

    if (buf == NULL) {
        return 0;
    }

    metadata_start[0] = count;
    uint32_t offset = 0;
    uint32_t size;
    FSEntry *entries = (FSEntry *)((char *)metadata_start + sizeof(count));
    for (uint32_t i = 0; i < count; i++) {
        FSFile file = files[i];
        FSEntry *entry = entries + i;

        size = strlen(file.filename) + 1;
        entry->filename.offset = offset;
        entry->filename.length = size;
        memcpy(data_start + offset, file.filename, size);
        offset = offset + size;

        size = file.size;
        entry->content.offset = offset;
        entry->content.length = size;
        memcpy(data_start + offset, file.content, size);
        offset = offset + size;
    }
    return 0;
}

int evaluate_file(lua_State *L, const FSFile *file) {
    return dochunk(L, luaL_loadbuffer(L, file->content, file->size,
                                      "=(read file system)"));
}

int test_make_file_system_evaluate_code(lua_State *L, FSFile *files,
                                        uint32_t count) {
    int ret;

    uint64_t buflen = 0;
    ret = ckb_save_fs(files, count, NULL, &buflen);
    if (ret) {
        return ret;
    }

    void *buf = malloc(buflen);
    if (buf == NULL) {
        return LUA_ERROR_OUT_OF_MEMORY;
    }

    ret = ckb_save_fs(files, count, buf, &buflen);
    if (ret) {
        return ret;
    }

    ckb_reset_fs();
    ret = ckb_load_fs(buf, buflen);
    if (ret) {
        return ret;
    }

    FSFile *f = ckb_must_get_file("main.lua");
    return evaluate_file(L, f);
}

int run_code_from_file_system(lua_State *L) {
    const char *code = "print('hello world!')";
    FSFile file = {"main.lua", (void *)code, (uint64_t)strlen(code)};
    return test_make_file_system_evaluate_code(L, &file, 1);
}

int run_code_with_modules(lua_State *L) {
    const char *main = "require'mymodule'.foo()";
    const char *module =
        "local mymodule = {}\n"
        "function mymodule.foo()\n"
        "    print('hello world!')\n"
        "end\n"
        "return mymodule\n";
    FSFile files[2];
    FSFile mainf = {"main.lua", (void *)main, (uint64_t)strlen(main)};
    FSFile modulef = {"mymodule.lua", (void *)module, (uint64_t)strlen(module)};
    files[0] = mainf;
    files[1] = modulef;
    return test_make_file_system_evaluate_code(L, files, 2);
}

static int run_file_system_tests(lua_State *L) {
    int status;
    status = run_code_from_file_system(L);
    if (status != LUA_OK) {
        return status;
    }
    status = run_code_with_modules(L);
    if (status != LUA_OK) {
        return status;
    }
    return 0;
}
