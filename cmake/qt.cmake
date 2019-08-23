macro(import_qt5)

## QT
set (Qt5DIR $ENV{Qt5DIR})
message ("${QT5DIR}")
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "${CMAKE_SOURCE_DIR}")
message ("${CMAKE_SOURCE_DIR}/QtVtkFrame")

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (QT5MODULE Qt5::Core Qt5::Gui Qt5::Widgets)

# 自动生成的文件的目标文件夹
#set (AUTOGEN_TARGETS_FOLDER "./visualization")
#message (${AUTOGEN_TARGETS_FOLDER})

# 包含ui文件生成的对应头文件
#qt5_wrap_ui(WRAP_FILES ${file_ui})
# include ui source files
FILE(GLOB_RECURSE file_ui ./*.ui)
message("ui files : " ${file_ui})

endmacro(import_qt5)