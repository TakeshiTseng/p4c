
set (MY_ARCH_SRCS
  myarch.cpp
  midend.cpp
  my-arch-gen.cpp
  )
set (MY_ARCH_HDRS
  midend.h
  my-arch-gen.h
  my-arch-config.h
  )

add_cpplint_files (${CMAKE_CURRENT_SOURCE_DIR} "${MY_ARCH_SRCS};${MY_ARCH_HDRS}")

build_unified(MY_ARCH_SRCS ALL)
add_executable(my-arch ${MY_ARCH_SRCS})
target_link_libraries (my-arch ${P4C_LIBRARIES} ${P4C_LIB_DEPS})
add_dependencies(my-arch genIR)

install (TARGETS my-arch
  RUNTIME DESTINATION ${P4C_RUNTIME_OUTPUT_DIRECTORY})

add_custom_target(link-binary
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/my-arch ${P4C_BINARY_DIR}/p4c-my-arch
)
