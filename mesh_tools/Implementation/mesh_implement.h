////////////////////////////////////
/// @file mesh_implement.h
/// @brief class mesh
/// @author  lyz
/// @date 2019-09-08
////////////////////////////////////

#include "meshDefs.h"
//#include "stress_field.h"
#include <vector>

namespace meshtools {
/*
data struct defines
*/

// TF is a struct to store 3 vertices of a halfface
// the order of vertices is ignored,
// which means hf1 = 1-2-3 equals hf2 = 2-3-1
// while hf1 = 1-2-3 does not equals hf2 = 3-2-1
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

class MeshImpl {
private:
  /*********** Functions begin **************/

  void readFromInp(std::ifstream &fin);

  void readFromOvm(std::ifstream &fin);

  /** insert a tetrahedral cell to mesh
      @param v  vertices of the tet
      @param faces  map from halfface(TF) to face, tell whether the face
                    it belong to had been added. faces shold be empty when
                    first use addCell.
  */
  void addCell(std::vector<OvmVeH> &v, std::map<TF, OvmFaH> &faces);

  void tetFaces(std::vector<Eigen::Matrix<long long, 3, 1>> &faces,
                long long v[4]);

  bool isSameHalfface(const std::vector<int> &f1, const std::vector<int> &f2);

  /*********** Functions end **************/

  /*********** Properties begin **************/

  /*********** Properties end **************/

public:
  // new ovm_mesh
  MeshImpl();
  // ovm_mesh is a shared_ptr, which does not need delete
  ~MeshImpl();

  /*********** Functions begin **************/

  void readMesh(std::string filename);

  void saveToOVM(std::string filename);

  // 分离文件的路径,文件名和后缀名
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  void getFaceData(std::vector<Eigen::Vector3d> &points,
                   std::vector<Eigen::Matrix<long long, 3, 1>> &inner_faces,
                   std::vector<Eigen::Matrix<long long, 3, 1>> &boundary_face);

  void getShrinkMesh(std::vector<Eigen::Vector3d> &points,
                     std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  /**
          reture average size of cells, for frame render
  */
  double cellSize();

  /*********** Functions end **************/

  /*********** Properties begin **************/

  VMeshPtr ovm_mesh = nullptr;

  bool mesh_loaded = false;

  std::string mesh_name = "";

  /*********** Properties end **************/
};
} // namespace meshtools