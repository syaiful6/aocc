add_library(aocc_common STATIC src/aocc_common.c)

target_include_directories(aocc_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_SYSTEM_PROCESOR MATCHES "arm64|aarch64")
  target_compile_options(aocc_common PRIVATE -mcpu=apple-m1)
else()
  target_compile_options(aocc_common PRIVATE -march=native)
endif()
