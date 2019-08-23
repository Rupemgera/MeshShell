#pragma once

/***********
���ļ���������mesh�Ĺ���
1. ����inp�ļ�������OpenVolumeMesh����
************/

#include <Eigen/Dense>
#include <string>
#include <vector>

namespace meshtools {

/*
ǰ������
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

  // �����ļ���·��,�ļ����ͺ�׺��
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  void get_face_data(std::vector<Eigen::Vector3d> &points,
                std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  std::string get_mesh_name();

/*********** Functions end **************/



 protected:
 private:
  MeshImpl *impl;
};

}  // namespace meshtools