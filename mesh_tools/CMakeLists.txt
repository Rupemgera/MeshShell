cmake_minimum_required (VERSION 3.8)

PROJECT(mesh_tools)

include(eigen3)
import_eigen()

## OpenVolumeMesh
include(OpenVolumeMesh)
import_openvolumemesh()

# aux_source_directory(. src_files)
file(GLOB_RECURSE mesh_files *.cxx *.cpp *.h *.hh *.hpp)

# add_library(mesh_tools STATIC ${src_files})
add_library(mesh_tools STATIC ${mesh_files})

target_link_libraries(mesh_tools PUBLIC
    ${OVM_LIB}
)