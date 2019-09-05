# source code directories
cmake_minimum_required (VERSION 3.8)
set (directories 
  .
  ./Attribs
  ./Core
  ./Config
  ./FileManager
  ./Geometry
  ./Mesh
)

aux_source_directory(./Attribs Attribs_files)
aux_source_directory(./Core Core_files)
aux_source_directory(./Config Config_files)
aux_source_directory(./FileManager FileManager_files)
aux_source_directory(./Geometry Geometry_files)
aux_source_directory(./Mesh Mesh_files)

add_library(OpenVolumeMesh STATIC 
${Attribs_files} 
${Core_files} 
${Config_files} 
${FileManager_files}
${Geometry_files}
${Mesh_files}
)