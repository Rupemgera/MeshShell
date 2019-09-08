cmake_minimum_required (VERSION 3.8)

PROJECT(mesh_tools)


if (MSVC)
## gcc, clang has no bigobj problem
	add_compile_options(-bigobj)
endif()
## define INCLUDE_TEMPLATES to enable OVM templates functions, like FileManager
add_definitions(-DINCLUDE_TEMPLATES)

include(eigen3)
import_eigen()

## OpenVolumeMesh
#include(OpenVolumeMesh)
#import_openvolumemesh()

set(directories 
./OpenVolumeMesh/Attribs
./OpenVolumeMesh/Core
./OpenVolumeMesh/Data
./OpenVolumeMesh/FileManager
./OpenVolumeMesh/Geometry
./OpenVolumeMesh/Mesh
./OpenVolumeMesh/Data
"./OpenVolumeMesh/System"
)

###### macro ##########
macro (acg_append_files ret ext)
  foreach (_dir ${ARGN})
    file (GLOB _files "${_dir}/${ext}")
    foreach (_file ${_files})
      get_filename_component (_filename ${_file} NAME)
      if (_filename MATCHES "^[.]")
	list (REMOVE_ITEM _files ${_file})
      endif ()
    endforeach ()
    list (APPEND ${ret} ${_files})
  endforeach ()
endmacro ()
#######################

# collect all header and source files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (mesh_sources "*.cxx" .)
acg_append_files (mesh_headers "*h" .)


add_library(mesh_tools STATIC ${sources} ${headers} ${mesh_sources} ${mesh_headers})