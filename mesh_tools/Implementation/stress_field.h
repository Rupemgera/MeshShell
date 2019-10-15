#pragma once
/***
####################

####################
***/

#include "frame_field.h"
#include <Eigen/Dense>
#include <dlib/optimization.h>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace meshtools {

/*
常量及别名定义
*/
//列向量形式保存点的坐标
using Point = Eigen::Vector3d;
//列向量形式保存方向向量
using V3d = Eigen::Vector3d;
// euler angle
// using EulerAngle = dlib::matrix<double, 1, 2>;

using column_vector = dlib::matrix<double, 0, 1>;

//四面体网格
/*
 *四面体网格使用OpenVolumemesh，在mesh_processing_tools.h中定义：
 *using VMesh = OpenVolumeMesh::GeometricPolyhedralMeshV3d;
 */

//定义场是基于何种单元。如顶点stress或体stress
enum STRESS_ELEMENT_TYPE { Node, Cell, NotDefined };

/**
 *eig_value    ordered decreasingly
 *eig_vectors    orders correspond to eig_value
 */
class StressTensor {
private:
  void init(double *tensor_component, int order = 0);

  void decompose();

public:
  /*
  data begin
  */

  Matrix_3 matrix_;
  double eig_values[3];
  Matrix_3 eig_vectors;

  /*
  data end
  */
  StressTensor();

  StressTensor(Matrix_3 matrix);

  /**
   *order = 0 : hypermesh way 00 11 22 01 12 02
   *order = 1 : abaqus way 00 11 22 01 02 12
   *order = 2 : GAUSS way 00 11 22 12 02 01
   */
  StressTensor(double *tensor_component, int order = 0);

  void reset(double *tensor_component, int order = 0);

  /**
   * u,v must be unit vector, otherwise the result will be wrong
   */
  template <typename T> double sin(T u, T v) { return (u.cross(v)).norm(); }

  /**
   *compare frames difference of 2 stress
   */
  double diff(StressTensor &b);

  /**
   *@brief major vector difference of 2 stress
   */
  double major_diff(StressTensor &b);

  /**
   *@brief calculate the diffirence after *this do permutation
   */
  double permute_diff(StressTensor &b, Permutation_3 permute);

  /**
   *@brief calculate the matching index of *this->v
   */
  int get_matching_index(StressTensor &v);

  double von_mises();
};

/**
 *@brief 主应力场类
 */
class PrincipalStressField {
public:
  /*
  data begin
  */

  std::vector<StressTensor> cell_tensors_;

  std::vector<StressTensor> node_tensors_;

  std::vector<Eigen::Vector3d> location;

  /*
  data end
  */

  PrincipalStressField();

  ~PrincipalStressField();

  /*类成员函数*/

  /**
   *@param save    if true, save stress field to vector form. default false.
   *@see saveStress
   *@deprecated
   */
  bool readInStress(std::string filename, VMeshPtr mesh = nullptr,
                    bool save = false);
  /**
   *@brief read stress field from processed file
   *first line contains "# flag number". flag is 'node' or 'element', number is
   *the number of lines
   */
  bool readProcessedStress(std::string filename, VMeshPtr mesh);

  /**
   * one line for one cell
   *1 int represent cell id, then follows 9 decimals ,every 3 represent a
   *vector. Each vector is a unit vector
   */
  bool saveEigenVectors(std::ofstream &stress_fout);

  /**
   *@brief when read node stress tensors, run this to calculate element tensors
   */
  bool init_element_tensors(VMeshPtr mesh);

  /**
   *@param loc  coordinates of singularites
   * find stresses that two of three eig_value are less than tolerance,save to
   *loc
   */
  void singularityLoaction(std::vector<Eigen::Vector3d> &loc, double tolerance);

  //设定每个单元的中心点的位置
  bool setCellCenter(VMeshPtr mesh);

  //设定mesh
  const bool set_mesh(VMeshPtr mesh);

  void get_locations(std::vector<Eigen::Vector3d> &ret);

  /**
   *@para P = 0,1,2 corresponds to major,middle,minor principal vector
   */
  void get_principal_dirs(std::vector<Eigen::Vector3d> &ret, int P = 0);

  void get_von_mises(std::vector<double> &von_mises);

  /*****************
  not completed yet

  bool singular_edges_extraction(VMeshPtr mesh);

  *******************/

  /*类成员变量*/

  STRESS_ELEMENT_TYPE element_type = STRESS_ELEMENT_TYPE::NotDefined;

private:
  /*成员变量*/
  size_t _n;
  // mesh的指针
  std::shared_ptr<VMesh> _mesh = nullptr;

  /*deprecated


  */

  /*成员函数*/

  void readStressHypermeshStyle(std::ifstream &stress_fin,
                                VMeshPtr mesh = nullptr);

  void readStressAbaqusStyle(std::ifstream &stress_fin);

  void readStressGaussStyle(std::ifstream &stress_fin);

  bool resize(size_t elements_number);

  bool reserve(size_t elements_number);
};

class VectorField {
private:
  // vectors of theta and phi
  std::vector<double> euler1_, euler2_;

  dlib::matrix<int> variable_pairs_;

  Eigen::Vector3d Euler2Vector(double theta, double phi);

  void Vector2Euler(Eigen::Vector3d v, double &theta, double &phi);

  // euler angle form of inner product
  double dot(double t1, double t2, double p1, double p2);

  // metric function for vectors
  double metric_func(double t1, double p1, double t2, double p2);

  double metric_grad1(double t1, double t2, double p1, double p2);

  double metric_grad2(double t1, double p1, double t2, double p2);

  double obj_func(const column_vector &m, double w_consist, double w_smooth);

  column_vector gradient_func(const column_vector &m, double w_consist,
                              double w_smooth);

public:
  VectorField(VMeshPtr mesh, std::vector<Eigen::Vector3d> &vectors);

  std::vector<Eigen::Vector3d> smooth_vector_field(double w_consistance = 1.0,
                                                   double w_smooth = 1.0);
};
} // namespace meshtools