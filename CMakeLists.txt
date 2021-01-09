cmake_minimum_required(VERSION 3.15)

project(RPNXCore)

find_package(Threads REQUIRED)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("int main() { return __builtin_clz(1); }" RPNX_HAVE_BUILTIN_CLZ)
check_cxx_source_compiles("#include <intrin.h>\nint main() { unsigned long index = 0; _BitScanReverse(&index, 1); }" RPNX_HAVE_BITSCANREVERSE)
check_cxx_source_compiles("#include <intrin.h>\nint main() { unsigned long index = 0; _BitScanReverse64(&index, 1ull); }" RPNX_HAVE_BITSCANREVERSE64)

add_library(rpnx-core)
set_target_properties(rpnx-core PROPERTIES CXX_STANDARD 17)

target_sources(rpnx-core PRIVATE
        private/sources/all/experimental/priority_dispatcher.cpp
        )

target_include_directories(rpnx-core PUBLIC public/headers/all)

list ( APPEND RPNX_CORE_HEADERS
        public/headers/all/rpnx/network_error.hpp
        public/headers/all/rpnx/experimental/network.hpp
        public/headers/all/rpnx/apply.hpp
        public/headers/all/rpnx/serial_traits.hpp
        public/headers/all/rpnx/derivator.hpp
        public/headers/all/rpnx/legacy/monoque.hpp
        public/headers/all/rpnx/meta.hpp
        public/headers/all/rpnx/experimental/thread_pool.hpp
        public/headers/all/rpnx/assert.hpp
        public/headers/all/rpnx/experimental/processor.hpp
        public/headers/all/rpnx/experimental/priority_dispatcher.hpp
        public/headers/all/rpnx/experimental/monoque.hpp
        public/headers/all/rpnx/experimental/bitwise.hpp
        public/headers/all/rpnx/experimental/cpuarchinfo.hpp
        public/headers/all/rpnx/experimental/scoped_action.hpp
        public/headers/all/rpnx/experimental/avl_tree.hpp
    )

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")

  target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/windows/network.cpp)
  #target_include_directories(rpnx-core PUBLIC public/headers/windows)
  #list ( APPEND RPNX_CORE_HEADERS public/headers/windows/rpnx/network.hpp)
  target_compile_definitions(rpnx-core PRIVATE _MBCS)
  target_link_libraries(rpnx-core ws2_32)

endif()


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/linux/network.cpp)
  #target_include_directories(rpnx-core PUBLIC public/headers/linux)
  #list ( APPEND RPNX_CORE_HEADERS public/headers/linux/rpnx/network.hpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/mac/network.cpp)
  #target_include_directories(rpnx-core PUBLIC public/headers/mac)
  #list ( APPEND RPNX_CORE_HEADERS public/headers/linux/rpnx/network.hpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
 target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/bsd/network.cpp)

 #target_include_directories(rpnx-core PUBLIC public/headers/bsd)
endif()


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
 target_sources(rpnx-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/sources/linux/network.cpp)
 #target_include_directories(rpnx-core PUBLIC public/headers/linux)
endif()


message("CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}")


target_sources(rpnx-core PRIVATE ${RPNX_CORE_HEADERS} )

target_link_libraries(rpnx-core Threads::Threads)

add_executable(rpnx-core-test1)
set_target_properties(rpnx-core-test1 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test1 PRIVATE private/sources/all/test1.cpp)
target_link_libraries(rpnx-core-test1 rpnx-core)


add_executable(rpnx-core-test2)
set_target_properties(rpnx-core-test2 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test2 PRIVATE private/sources/all/test2.cpp)
target_link_libraries(rpnx-core-test2 rpnx-core)

add_executable(rpnx-core-test3)
set_target_properties(rpnx-core-test3 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test3 PRIVATE private/sources/all/test3.cpp)
target_link_libraries(rpnx-core-test3 rpnx-core)

add_executable(rpnx-core-test4)
set_target_properties(rpnx-core-test4 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test4 PRIVATE private/sources/all/test4.cpp)
target_link_libraries(rpnx-core-test4 rpnx-core)

add_executable(rpnx-core-test5)
set_target_properties(rpnx-core-test5 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test5 PRIVATE private/sources/all/test5.cpp)
target_link_libraries(rpnx-core-test5 rpnx-core)

add_executable(rpnx-core-test6)
set_target_properties(rpnx-core-test6 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-test6 PRIVATE private/sources/all/test6.cpp)
target_link_libraries(rpnx-core-test6 rpnx-core)

#add_executable(rpnx-core-test7)
#set_target_properties(rpnx-core-test7 PROPERTIES CXX_STANDARD 17)
#target_sources(rpnx-core-test7 PRIVATE private/sources/all/test7.cpp)
#target_link_libraries(rpnx-core-test7 rpnx-core)

add_executable(rpnx-core-benchmark1)
set_target_properties(rpnx-core-benchmark1 PROPERTIES CXX_STANDARD 17)
target_sources(rpnx-core-benchmark1 PRIVATE private/sources/all/bm1.cpp)
target_link_libraries(rpnx-core-benchmark1 rpnx-core Threads::Threads)

install(TARGETS rpnx-core EXPORT rpnx_exports)
export(EXPORT rpnx_exports FILE RPNXCoreConfig.cmake  NAMESPACE RPNX::)
