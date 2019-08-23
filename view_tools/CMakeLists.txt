cmake_minimum_required (VERSION 3.8)

PROJECT(view_tools)

## VTK
include(VTK)
import_vtk()

aux_source_directory(. src_files)

add_library(view_tools ${src_files})

target_link_libraries(view_tools PRIVATE
    ${VTK_LIBRARIES}
)