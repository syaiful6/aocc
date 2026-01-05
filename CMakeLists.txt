cmake_minimum_required(VERSION 3.19)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(aocc)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

function(add_chapter name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE aocc_common)
endfunction()

add_subdirectory(common)
add_subdirectory(day01)
