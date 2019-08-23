macro(import_vtk)

## VTK
find_package(VTK 8.9 REQUIRED)
if (VTK_FOUND)
	message ("VTK found. VERSION: " ${VTK_VERSION})
	#include (${VTK_USE_FILE})
else()
	# VTK版本低于8.9
	find_package(VTK 8.2 REQUIRED)
	if (VTK_FOUND)
		message ("VTK found. VERSION: " ${VTK_VERSION})
		include (${VTK_USE_FILE})
	else()
		message (FATAL_ERROR "VTK not found")
	endif()
endif()

if (VTK_VERSION VERSION_LESS "8.90.0")
# vtk_module_autoinit is needed
vtk_module_autoinit(
    TARGETS Dodecahedron
    MODULES ${VTK_LIBRARIES}
)
endif()

endmacro(import_vtk)