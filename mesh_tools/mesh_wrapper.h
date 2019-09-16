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

template <int n> using FaceList = Eigen::Matrix<long long, n, 1>;

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

  void getFaceData(std::vector<Eigen::Vector3d> &points,
                   std::vector<FaceList<3>> &faces);

  void getBoundaryFaceIds(std::vector<int> &faceids_list);

  void getShrinkMesh(std::vector<Eigen::Vector3d> &points,
                     std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  std::string get_mesh_name();

  double cellSize();

  /*********** geometry end **************/

  /********************* mesh related *****************************/

  /**
   *@brief matching index of cell loop around an edge
   */
  int match_index(int edge_idx);

  size_t n_vertices();
  size_t n_edges();
  size_t n_faces();
  size_t n_cells();

  /********** stress related functions begin **********/

  /**
      input two groups of data, separated by ','
      first group contains 1 integer, that is the id of the cell
      second group contains 6 decimal, that the 6 tensor stress component,
      ordered by XX YY ZZ XY YZ ZX
  */
  void readStressField(std::string filename);

  void get_principal_vectors(std::vector<Eigen::Vector3d> &loc,
                             std::vector<Eigen::Vector3d> &major,
                             std::vector<Eigen::Vector3d> &middle,
                             std::vector<Eigen::Vector3d> &minor);

  /**
      @param loc  coordinates of singularites
      find stresses that two of three eig_value are less than tolerance,save to
     loc
  */
  void singularityLoaction(std::vector<Eigen::Vector3d> &loc, double tolerance);

  /**
   *@brief
   */
  void divideCells(std::vector<int> &split_face_ids, double tolerance);

  /**
   *@brief returen coordinates of cell centers
   */
  void request_cell_centers(std::vector<Eigen::Vector3d> &retrieve_val);

  /********** stress related functions  end  **********/

  void test();

protected:
private:
  MeshImpl *impl;
  PrincipalStressField *field;
};

} // namespace meshtools