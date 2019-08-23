#pragma once

/***********
本文件包含处理mesh的工具
1. 读入inp文件，返回OpenVolumeMesh对象
************/
#include "meshDefs.h"

#include <fstream>
#include <map>

namespace meshtools {

/*
                data struct defines
                */
struct TF {
  int first;
  int second;
  int third;

  TF(int x, int y, int z) {
    std::vector<int> ve;
    ve.push_back(x), ve.push_back(y), ve.push_back(z);
    std::sort(ve.begin(), ve.end());
    first = ve[0], second = ve[1], third = ve[2];
  }
  bool operator<(const TF &tf2) const {
    if ((first < tf2.first) || (first == tf2.first && second < tf2.second) ||
        (first == tf2.first && second == tf2.second && third < tf2.third)) {
      return true;
    } else {
      return false;
    }
  }
  bool operator==(const TF &tf2) const {
    if (first == tf2.first && second == tf2.second && third == tf2.third) {
      return true;
    } else {
      return false;
    }
  }
  std::size_t operator()(const TF &tf) const {
    using std::hash;
    using std::size_t;

    return ((hash<int>()(tf.first) ^ (hash<int>()(tf.second) << 1)) >> 1) ^
           (hash<int>()(tf.third) << 1);
  }
};

class MeshWrapper {
 public:
  MeshWrapper();
  /*********** Functions begin **************/

  void readMesh(std::string filename);

  void saveToOVM(std::string filename);

  /*********** Functions end **************/

  /*********** Properties begin **************/
  VMeshPtr ovm_mesh = nullptr;
  /*********** Properties end **************/
 protected:
  /*********** Properties begin **************/

  bool mesh_loaded = false;

  std::string mesh_name = "";

  /*********** Properties end **************/
 private:
  /*********** Functions begin **************/

  void readFromInp(std::ifstream &fin);

  void readFromOvm(std::ifstream &fin);

  void addCell(std::vector<OvmVeH> &v, std::map<TF, OvmFaH> &faces);

  bool isSameHalfface(const std::vector<int> &f1, const std::vector<int> &f2);

  // 分离文件的路径,文件名和后缀名
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  /*********** Functions end **************/
};

}  // namespace meshtools