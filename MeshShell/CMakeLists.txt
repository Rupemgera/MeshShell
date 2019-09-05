# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

PROJECT(MeshShell)

## boost
find_package(Boost)
### test if boost is found
if (Boost_FOUND)
	message ("boost found " ${Boost_VERSION})
	include_directories(${boost_INCLUDE_DIRS})
else()
	#message (FATAL_ERROR "can not found Boost")
	message ("can not found Boost")
endif()

## QT5
include(qt)
import_qt5()

## VTK
include(VTK)
import_vtk()

## eigen3
include(eigen3)
import_eigen()

## OpenVolumeMesh
#include(OpenVolumeMesh)
#import_openvolumemesh()

## source files
 aux_source_directory(./src src_files)
 aux_source_directory(./widgets widgets_files)
 aux_source_directory(./view_tools vtk_files)


 add_executable (MeshShell ${src_files} ${widgets_files} ${file_ui} ${vtk_files})
 #add_executable (${PROJECT_NAME} ${src_files} ${mesh_files} ${stress_files} ${visualization_files})

 ## add link libraries
 target_link_libraries(MeshShell PRIVATE
	${QT5MODULE}
	${VTK_LIBRARIES}
	mesh_tools
 )

# TODO: Add tests and install targets if needed.  
