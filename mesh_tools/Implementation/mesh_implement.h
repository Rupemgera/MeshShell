#pragma once

////////////////////////////////////
/// @file mesh_implement.h
/// @brief class mesh
/// @author  lyz
/// @date 2019-09-08
////////////////////////////////////

#include "graphs.h"
#include "meshDefs.h"
#include "stress_field.h"
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

  /**
   *@brief given 4 vertices of a tet, push all 4 faces to the vector
   */
  void tetFaces(std::vector<Eigen::Matrix<long long, 3, 1>> &faces,
                long long v[4]);

  bool isSameHalfface(const std::vector<int> &f1, const std::vector<int> &f2);

  /*********** Functions end **************/

  /*********** Properties begin **************/

  bool cell_center_exits = false;

  /*********** Properties end **************/

public:
  // new ovm_mesh
  MeshImpl();
  // ovm_mesh is a shared_ptr
  ~MeshImpl();

  /*********** IO related **************/

  void readMesh(std::string filename);

  void saveToOVM(std::string filename);

  // 分离文件的路径,文件名和后缀名
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  /********************** geometry ****************************/

  void getFaceData(std::vector<Eigen::Vector3d> &points,
                   std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  void getBoundaryFaceIds(std::vector<int> &faceids_list);

  void getShrinkMesh(std::vector<Eigen::Vector3d> &points,
                     std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  /**
   *@reture average size(radius) of cells, for frame render
   */
  double cellSize();

  /**
   *@brief retrieve coordinates of cell centers
   */
  bool request_cell_centers();

  /*********** Functions end **************/

  /******************* mesh related *************************/

  OvmFaH commonFace(OvmCeH ch1, OvmCeH ch2);

  /**
   *@brief construct a directed graph, stored in this->matching_graph.
   * edge u->v's value is matching index of u->v 
   */
  void construct_matching_graph(std::vector<StressTensor> &stresses);

  /**
   *@brief find cells around an edge, ordered clockwise or anticlockwise
   *@warning make sure construct_matching_graph being run, and matching_graph exits
   */
  int find_cell_loop(OvmHaEgH halfedge, std::vector<size_t> &cell_loop);

  /*********** stress related begin **************/

  /**
   *create property "stress_tensor" for each cell.
   *Not recommended!
   *property can be got as below:
   *	auto properties =
   *ovm_mesh->request_cell_property<property_type>("property_name");
   *	property_type p = properties[CellHandle];
   */
  void assignCellStress(std::vector<StressTensor> &tensors);

  /**
   *@brief divide cell according to their major principal
   *@param split_face_ids retrive ids of the faces that its two cells have
   *bigger diff than tolerance
   */
  void divideCells(std::vector<StressTensor> &tensors,
                   std::vector<int> &split_face_ids, double tolerance);
  /*********** stress related end ****************/

  /*********** Properties begin **************/

  VMeshPtr ovm_mesh = nullptr;

  std::vector<Eigen::Vector3d> cell_centers;

  /**
   *@brief value of edge u->v is the matching index of the trans matrix u->v
   */
  MatchingGraph *matching_graph = nullptr  ;

  bool mesh_loaded = false;

  std::string mesh_name = "";

  /*********** Properties end **************/
};
} // namespace meshtools