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

/*
前置声明
*/
class MeshImpl;

/*
class define
*/

template <typename T> struct Triple { T x, y, z; };

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

  void getFaceData(std::vector<Eigen::Vector3d> &points,
                   std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  void getShrinkMesh(std::vector<Eigen::Vector3d> &points,
                     std::vector<Eigen::Matrix<long long, 3, 1>> &faces);

  std::string get_mesh_name();

  size_t n_vertices();
  size_t n_edges();
  size_t n_faces();
  size_t n_cells();

  /*********** Functions end **************/

  /********** stress related functions begin **********/

  /**
      input two groups of data, separated by ','
      first group contains 1 integer, that is the id of the cell
      second group contains 6 decimal, that the 6 tensor stress component,
      ordered by XX YY ZZ XY YZ ZX
  */
  void readStressField(std::string filename);

  /**
      @param loc  coordinates of singularites
      find stresses that two of three eig_value are less than tolerance,save to
     loc
  */
  void singularityLoaction(std::vector<Eigen::Vector3d> &loc, double tolerance);

  /********** stress related functions  end  **********/

protected:
private:
  MeshImpl *impl;
};

} // namespace meshtools