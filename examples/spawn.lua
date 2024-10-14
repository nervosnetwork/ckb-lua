local callee_code = [[
local m = arg[2] .. arg[3]
local inherited_fds, err = ckb.inherited_fds()
local n, err = ckb.write(inherited_fds[1], m)
ckb.close(inherited_fds[1])
assert(n == 10)
local pid = ckb.process_id()
assert(pid >= 1)
]]

local fds, err = ckb.pipe()
assert(err == nil)
local pid, err = ckb.spawn(0, ckb.SOURCE_CELL_DEP, 0, 0, {"-e", callee_code, "hello", "world"}, {fds[2]})
assert(err == nil)
local ret = ckb.read(fds[1], 10)
assert(ret == "helloworld")
ckb.wait(pid)

local code_hash, err = ckb.load_cell_by_field(0, ckb.SOURCE_CELL_DEP, ckb.CELL_FIELD_DATA_HASH)
assert(err == nil)
local fds, err = ckb.pipe()
assert(err == nil)
local pid, err = ckb.spawn_cell(code_hash, 4, 0, 0, {"-e", callee_code, "hello", "world"}, {fds[2]})
assert(err == nil)
local ret = ckb.read(fds[1], 10)
assert(ret == "helloworld")
ckb.wait(pid)
