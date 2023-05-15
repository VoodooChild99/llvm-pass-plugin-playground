# LLVM Pass Plugin Playground

Ready-to-use environment that makes LLVM pass plugin development easy,
inspired by [llvm-tutor](https://github.com/banach-space/llvm-tutor).

## How to use

- Add new plugins to [lib](./lib) and modify `CMakeFiles.txt` accordingly.

- To build plugins, run `cmake-config.sh` first, then `cd build && make`.

- To use different LLVM versions, set the env var `LLVM_CONFIG` before you run `cmake-config.sh`,
e.g., `LLVM_CONFIG=llvm-config-15 ./cmake-config.sh` if you want to use LLVM-15/clang-15. By default,
`llvm-config` is used.
