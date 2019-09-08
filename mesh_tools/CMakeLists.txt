cmake_minimum_required (VERSION 3.8)

project(mesh_tools)


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

include_directories(${PROJECT_SOURCE_DIR})

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
acg_append_files (impl_headers "*.h" "./Implementation")
acg_append_files (impl_sources "*.cxx" "./Implementation")
acg_append_files (wrap_sources "*.cxx" .)
acg_append_files (wrap_headers "*h" .)

set(source_list 
${sources} 
${headers} 
${wrap_sources} 
${wrap_headers} 
${impl_headers} 
${impl_sources}
)


add_library(mesh_tools STATIC ${source_list})

if(MSVC)
	foreach(srcf ${source_list})
		file(RELATIVE_PATH source "${PROJECT_SOURCE_DIR}" "${srcf}")
		get_filename_component(source_path "${source}" DIRECTORY)
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()