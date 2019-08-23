macro(import_openvolumemesh)
	## build type
	if (CMAKE_CONFIGURATION_TYPES MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "Debug")
		message("Debug mode")
		set(OVM_LIB "OpenVolumeMeshd")
	elseif (CMAKE_CONFIGURATION_TYPES MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "Release")
		message("Release mode")
		set(OVM_LIB "OpenVolumeMesh")
	endif()

	## OpenVolumeMesh
	if(WIN32)
		set (OVM_DIR "$ENV{OVM2_INSTALL_DIR}")
		message("OpenVolumeMesh : " "${OVM_DIR}")
		include_directories ("${OVM_DIR}/include")
		link_directories ("${OVM_DIR}/lib")
	endif()
endmacro(import_openvolumemesh)