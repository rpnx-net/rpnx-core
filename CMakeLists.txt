cmake_minimum_required(VERSION 3.15)

project(RPNXCore)

add_library(rpnx-core)
set_target_properties(rpnx-core PROPERTIES CXX_STANDARD 17)

target_include_directories(rpnx-core PUBLIC public/headers/all)

list ( APPEND RPNX_CORE_HEADERS public/headers/all/rpnx/network_error.hpp  public/headers/all/rpnx/indirect_variant.hpp  public/headers/all/rpnx/apply.hh)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")

  target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/windows/network.cpp)
  target_include_directories(rpnx-core PUBLIC public/headers/windows)
  list ( APPEND RPNX_CORE_HEADERS public/headers/windows/rpnx/network.hpp)
  target_compile_definitions(rpnx-core PRIVATE _MBCS)
  target_link_libraries(rpnx-core ws2_32)

endif()


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

  target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/linux/network.cpp)
  target_include_directories(rpnx-core PUBLIC public/headers/linux)
  list ( APPEND RPNX_CORE_HEADERS public/headers/linux/rpnx/network.hpp)
  
endif()


target_sources(rpnx-core PRIVATE ${RPNX_CORE_HEADERS} )

add_executable(rpnx-core-test1)
set_target_properties(rpnx-core-test1 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test1 PRIVATE private/sources/all/test1.cpp)
target_link_libraries(rpnx-core-test1 rpnx-core)


add_executable(rpnx-core-test2)
set_target_properties(rpnx-core-test2 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test2 PRIVATE private/sources/all/test2.cpp)
target_link_libraries(rpnx-core-test2 rpnx-core)

install(TARGETS rpnx-core EXPORT rpnx_exports)
export(EXPORT rpnx_exports FILE RPNXCoreConfig.cmake  NAMESPACE RPNX::)
