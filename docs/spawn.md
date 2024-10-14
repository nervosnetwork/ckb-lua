Note: the syscall spawn is a planned feature that is currently unavailable for use in ckb.
We need a new hard fork to ship the spawn syscall. We will update the table below
when the availablity of spawn syscall changes.

| network | readiness |
|---------|-----------|
| mainnet | NOT ready |
| testnet | NOT ready |

# Running ckb-lua-vm as a subprocess to the main script
There are quite a few benefits in running ckb-lua-vm as a subprocess to the main script.
- The main script's context is saved, and can continue to run when ckb-lua-vm exits.
- The ckb-lua-vm instances are called with command line arguments which can be used to differentiate different tasks.
- Ckb-lua may return data of any format by the function `ckb.write`.

To demostrate how to extend the capabilities of a main script with ckb-lua-vm, we provide
an example (an admittedly contrived one) that spawn a ckb-lua-vm subprocess which simply concatenate
the command line arguments and return the result to the main script.

[The main script of this example](../examples/spawn.c) is written in c.
This program can be built with `make all-via-docker`. Afterwards, you may run it
with `make -C tests/test_cases spawnexample`.
