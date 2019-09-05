cmake_minimum_required (VERSION 3.8)

PROJECT(mesh_tools)

include(eigen3)
import_eigen()

## OpenVolumeMesh
#include(OpenVolumeMesh)
#import_openvolumemesh()

add_subdirectory(OpenVolumeMesh)
include(OpenVolumeMesh)

aux_source_directory(. src_files)

add_library(mesh_tools STATIC ${src_files})

target_link_libraries(mesh_tools PUBLIC
    ${OpenVolumeMesh}
)