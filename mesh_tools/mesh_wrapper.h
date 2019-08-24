#pragma once

/***********
本文件包含处理mesh的工具
1. 读入inp文件，返回OpenVolumeMesh对象
************/

#include <Eigen/Dense>
#include <string>
#include <vector>

namespace meshtools {

/*
前置声明
*/
class MeshImpl;
template <typename T>
struct Triple {
  T x, y, z;
};

class MeshWrapper {
 public:
  MeshWrapper();

  ~MeshWrapper();

  /*********** Functions begin **************/

  void readMesh(std::string filename);

  void saveToOVM(std::string filename);

  // 分离文件的路径,文件名和后缀名
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  void get_face_data(std::vector<Eigen::Vector3d> &points,
                std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  std::string get_mesh_name();

	size_t n_vertices();
	size_t n_edges();
	size_t n_faces();
	size_t n_cells();

/*********** Functions end **************/



 protected:
 private:
  MeshImpl *impl;
};

}  // namespace meshtools