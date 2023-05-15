set(LLVM_PASS_PLUGIN_LIST
  LoopIndVarBound
)

set(
	LoopIndVarBound_Src
  LoopIndVarBound.cpp
)

foreach( plugin ${LLVM_PASS_PLUGIN_LIST} )
  add_library(
    ${plugin}
    SHARED
    ${${plugin}_Src}
  )

  target_include_directories(
    ${plugin}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  )

  target_link_libraries(
    ${plugin}
    "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
  )
endforeach()
