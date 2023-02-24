

CKB-DEBUGGER ?= ckb-debugger
PORT ?= 9999
TEST ?= tmp
MAX-CYCLES ?= 99999999999

define run
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file $(1)  --bin ../../build/lua-loader.debug -- -r   
endef

define run_ci
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file $(1)  --bin ../../build/lua-loader.debug -- -r  2>&1 | fgrep 'Run result: 0'
endef

define run_with_mocked_tx
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --tx-file sample_data1.json --script-group-type=type --script-hash=0xca505bee92c34ac4522d15da2c91f0e4060e4540f90a28d7202df8fe8ce930ba --read-file $(1)  --bin ../../build/lua-loader.debug -- -r  2>&1 | fgrep 'Run result: 0'
endef

define run_pretty_result
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file sample_data1.json --script-group-type=type --script-hash=0xca505bee92c34ac4522d15da2c91f0e4060e4540f90a28d7202df8fe8ce930ba --read-file $(1)  --bin ../../build/lua-loader.debug -- -r |& sed 's/DEBUG.*SCRIPT>//g' | sed ':a;N;$!ba;s/\n/NEWLINE/g' | sed 's/NEWLINENEWLINE/\n/g' | sed 's/NEWLINE//g'
endef

save-and-load-file-system-data:
	RUST_LOG=debug $(CKB-DEBUGGER) --bin ../../build/lua-loader.debug -- -t 2>&1 | fgrep 'Run result: 0'

evaluate-lua-code-in-cell-data:
	# The data are lua code `print('hello world')`
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file lua_code_in_cell_data.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- 2>&1 | fgrep 'hello world'
	# The data are lua bytecode of `print('hello world')`
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file lua_bytecode_in_cell_data.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- 2>&1 | fgrep 'hello world'
	# The data are lua file system with a `main.lua` whose content is `print('hello world!')`
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file lua_fs_code_in_cell_data.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- -f 2>&1 | fgrep 'hello world!'
	# The data are lua file system with a `main.lua` which requires a module from the file system
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file lua_fs_code_with_module_in_cell_data.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- -f 2>&1 | fgrep 'hello world!'
	# The data are lua file system with a `main.lua` which mounts a module from output cell data
	RUST_LOG=debug $(CKB-DEBUGGER) --tx-file lua_mount_fs_function.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- -f 2>&1 | fgrep 'hello world!'
	# Check ckb.mount's behavior is expected
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file test_mount.lua --tx-file lua_mount_fs.json --script-group-type=type --cell-index=0 --cell-type=output --bin ../../build/lua-loader.debug -- -l -f 2>&1 | fgrep 'Run result: 0'

memory_leak.json:
	./gen_tx_with_large_witnesses.sh $@ 81920

memory_leak: memory_leak.json
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --tx-file $^ --script-group-type=type --script-hash=0xca505bee92c34ac4522d15da2c91f0e4060e4540f90a28d7202df8fe8ce930ba --read-file test_$@.lua --bin ../../build/lua-loader.debug -- -r  2>&1 | fgrep 'Run result: 0'

partial_loading.json:
	./gen_tx_with_large_witnesses.sh $@ 8192000

partial_loading: partial_loading.json
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --tx-file $^ --script-group-type=type --script-hash=0xca505bee92c34ac4522d15da2c91f0e4060e4540f90a28d7202df8fe8ce930ba --read-file test_partial_loading.lua --bin ../../build/lua-loader.debug -- -r  2>&1 | fgrep 'Run result: 0'

dylibtest:
	cd tests_rust; cargo test run_dylib_tests -- --nocapture 2>&1 | grep -v -F 'Code after exit_script should be unreachable' | grep -q -F 'hello world'

lua-fs-util:
	./lua-fs-pack-and-unpack.sh
	./lua-fs-unpack-existing.sh

all:
	$(call run, test_require.lua)
	$(call run, test_loadfile.lua)
	$(call run_with_mocked_tx, test_ckbsyscalls.lua)
	$(call run, bn.lua)

ci: evaluate-lua-code-in-cell-data save-and-load-file-system-data partial_loading memory_leak dylibtest lua-fs-util
	$(call run_ci, test_require.lua)
	$(call run_ci, test_loadfile.lua)
	$(call run_with_mocked_tx, test_ckbsyscalls.lua)
	$(call run, out_of_memory.lua) 2>&1 | fgrep 'not enough memory'
	$(call run, out_of_memory2.lua) 2>&1 | fgrep 'not enough memory'
	$(call run_ci, bn.lua)
	$(call run_ci, msgpack-tests.lua)

benchmark:
	perf stat $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file msgpack-benchmark.bc --bin ../../build/lua-loader.debug -- -r
	perf stat lua msgpack-benchmark.bc


test:
	RUST_LOG=debug $(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file bn.lua --bin ../../build/lua-loader.debug -- -r

hello_world:
	$(call run, hello_world.lua)

run-pretty:
	$(call run_pretty_result, test_$(TEST).lua)

run-debugger:
	$(CKB-DEBUGGER) --mode gdb --gdb-listen 127.0.0.1:${PORT} --read-file out_of_memory.lua --bin ../../build/lua-loader.debug -- -r
