add_executable(custom-clang compiler.cpp)

target_compile_definitions(custom-clang PRIVATE
  C_COMPILER_BIN="${CMAKE_C_COMPILER}"
  CXX_COMPILER_BIN="${CMAKE_CXX_COMPILER}"
  CUSTOM_CC="custom-clang"
  CUSTOM_CXX="custom-clang++"
)

add_custom_command(TARGET custom-clang
  POST_BUILD COMMAND ln -sf "custom-clang" "custom-clang++")

target_link_libraries(
  custom-clang
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
  LLVMSupport
)