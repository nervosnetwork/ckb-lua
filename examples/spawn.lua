fds, err = ckb.pipe()
assert(err == nil)

local callee_code = [[
local m = arg[2] .. arg[3]
local inherited_fds, err = ckb.inherited_fds()
local n, err = ckb.write(inherited_fds[1], m)
ckb.close(inherited_fds[1])
assert(n == 10)
local pid = ckb.process_id()
assert(pid == 1)
]]

pid, err = ckb.spawn(0, 3, 0, 0, {"-e", callee_code, "hello", "world"}, {fds[2]})
assert(err == nil)
local ret = ckb.read(fds[1], 10)
assert(ret == "helloworld")
ckb.wait(pid)
