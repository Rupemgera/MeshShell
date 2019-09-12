# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

PROJECT(MeshShell)

## boost
if (POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW)
endif()
if (Boost_FOUND)
	# message ("boost found " ${Boost_VERSION})
	include_directories(${boost_INCLUDE_DIRS})
else()
	find_package(Boost)
	include_directories(${boost_INCLUDE_DIRS})
	#message (FATAL_ERROR "can not found Boost")
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

## source files
 # aux_source_directory(./src src_files)
 # aux_source_directory(./widgets widgets_files)
 # aux_source_directory(./view_tools vtk_files)
 file(GLOB_RECURSE src_files *.cxx *.cpp *.h *.hh *.hpp)
 set(source_list ${src_files} ${file_ui})
 add_executable(MeshShell ${source_list})
 #add_executable (MeshShell ${src_files} ${widgets_files} ${file_ui} ${vtk_files})
 #add_executable (${PROJECT_NAME} ${src_files} ${mesh_files} ${stress_files} ${visualization_files})

 ## add link libraries
 target_link_libraries(MeshShell PRIVATE
	${QT5MODULE}
	${VTK_LIBRARIES}
	mesh_tools
 )

 # group source files in Visual Studio in folder way
 if(MSVC)
	foreach(srcf ${source_list})
		file(RELATIVE_PATH source "${PROJECT_SOURCE_DIR}" "${srcf}")
		get_filename_component(source_path "${source}" DIRECTORY)
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
 endif()

# TODO: Add tests and install targets if needed.  
