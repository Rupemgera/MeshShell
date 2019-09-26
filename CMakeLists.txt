# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set (PROJECT_NAME "MeshShell")
project (${PROJECT_NAME})

# C++ standard 
# C++11 may be set to C++14 in the future
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_STANDARD 14)

#     静态库输出目录 => 构建目录/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
#     动态库输出目录 => 构建目录/bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
#     可执行程序输出目录 => 构建目录/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Include sub-projects.

Include(eigen3)
import_eigen3()

add_subdirectory("mesh_tools")
include_directories("mesh_tools")

add_subdirectory ("MeshShell")

