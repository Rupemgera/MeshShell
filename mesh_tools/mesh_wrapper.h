#pragma once
/********************************/
// @author : lyz
// @content: provide interfaces to lib users
// @birth : 2019-09-08
/********************************/

#include <Eigen/Dense>
#include <string>
#include <vector>

namespace meshtools {

/// 前置声明

class MeshImpl;

class PrincipalStressField;

// class StressTensor;

/// defines

template <int n> using FaceVertices = Eigen::Matrix<long long, n, 1>;
using V3d = Eigen::Vector3d;

///

/*
class define
*/

// template <typename T> struct Triple { T x, y, z; };

class MeshWrapper {
public:
  MeshWrapper();

  ~MeshWrapper();

  /*********** geometry begin **************/

  void readMesh(std::string filename);

  void saveToOVM(std::string filename);

  // 分离文件的路径,文件名和后缀名
  // 0:path
  // 1:filename
  // 2:extension
  // 3:filename without extension
  std::vector<std::string> separateFilename(std::string filename);

  void getFaceData(std::vector<V3d> &points,
                   std::vector<FaceVertices<3>> &faces);

  /**
   *@brief given a list of cell's ids, return their coordinates in corresponding
   *order
   *@param points  retrieve coordinates of every cell centor of cells in
   *cell_ids
   */
  void getCellSegmentData(std::vector<int> &cell_ids, std::vector<V3d> &points);

  /**
   *@brief given a list of cell's ids, return their coordinates in corresponding
   *order
   *@return a vector stores coordinates of cell centor corresponding to cell_ids
   */
  std::vector<V3d> getCellSegmentData(std::vector<int> &cell_ids);

  std::vector<V3d> getEdgeData(std::vector<int> &edge_ids);

  void getBoundaryFaceIds(std::vector<int> &faceids_list);

  void getShrinkMesh(std::vector<V3d> &points,
                     std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  std::string get_mesh_name();

  double cellSize();

  /*********** geometry end **************/

  /********************* mesh related *****************************/

  /**
   *@return matching index of cell loop around an edge
   */
  int get_matching_index(int edge_idx);

  /**
   *@brief find all edges that is singular
   *@return a vector of Vector3d, every two elements represents edges's two
   *points
   */
  bool get_singular_edges(std::vector<V3d> &edge_points);

  bool get_singular_edges(std::vector<FaceVertices<2>> &vertex_ids);

  std::string meshInfo();

  int get_cell_loop(int from_v_id, int to_v_id, std::vector<int> &cell_loop);
  /*************************** stress related *********************************/
  /**
      input two groups of data, separated by ','
      first group contains 1 integer, that is the id of the cell
      second group contains 6 decimal, that the 6 tensor stress component,
      ordered by XX YY ZZ XY YZ ZX
  */
  bool readStressField(std::string filename);

  bool saveElementTensors(std::string filename);

  void get_principal_vectors(std::vector<V3d> &loc, std::vector<V3d> &major,
                             std::vector<V3d> &middle, std::vector<V3d> &minor);

  /**
      @param loc  coordinates of singularites
      find stresses that two of three eig_value are less than tolerance,save to
     loc
  */
  void singularityLoaction(std::vector<V3d> &loc, double tolerance);

  /**
   *@brief
   */
  void divideCells(std::vector<int> &split_face_ids, double tolerance);

  /**
   *@brief returen coordinates of cell centers
   */
  void request_cell_centers(std::vector<V3d> &retrieve_val);

  std::vector<V3d> &request_cell_centers();

  void request_von_mises(std::vector<double> &von_mises);

  std::vector<V3d> request_smoothed_stress_field(double w_consistance = 1.0,
                                                 double w_smooth = 1.0);

  /********** stress related functions  end  **********/

  void test();

protected:
private:
  MeshImpl *impl;
  PrincipalStressField *field;
};

} // namespace meshtools