#pragma once
/***
####################

####################
***/

#include "frame_field.h"
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace meshtools{

/*
常量及别名定义
*/
//列向量形式保存点的坐标
using Point = Eigen::Vector3d;
//列向量形式保存方向向量
using Direction = Eigen::Vector3d;

//四面体网格
/*
四面体网格使用OpenVolumemesh，在mesh_processing_tools.h中定义：
using VMesh = OpenVolumeMesh::GeometricPolyhedralMeshV3d;
*/

//定义场是基于何种单元。如顶点stress或体stress
enum STRESS_ELEMENT_TYPE { Node, Cell, NotDefined };

/**
    eig_value    ordered decreasingly
    eig_vectors    orders correspond to eig_value
*/
class StressTensor {
private:
  void init(double *tensor_component);

public:
  /*
  data begin
  */

  Matrix_3 _tensor;
  double eig_values[3];
  Matrix_3 eig_vectors;

  /*
  data end
  */
  StressTensor();
  StressTensor(double *tensor_component);
  void reset(double *tensor_component);
};

/**
主应力场类
成员变量：
成员函数：
*/
class PrincipalStressField {
public:
  /*
  data begin
  */

  std::vector<StressTensor> tensors;

	std::vector<MeshPoint> location;

  /*
  data end
  */
  /*deprecated

  //读入mesh
  bool readInMesh(std::ifstream &mesh_fin, std::string filename_extension);

  PrincipalStressField(VMeshPtr mesh);

  PrincipalStressField(std::string mesh_filename, std::string stress_filename);
  */

  PrincipalStressField();

  ~PrincipalStressField();

  /*类成员函数*/

  /**
      @param save    if true, save stress field to vector form. default false.
      @see saveStress
  */
  bool readInStress(std::string filename, VMeshPtr mesh = nullptr,
                    bool save = false);

  /**
      one line for one cell
      1 int represent cell id, then follows 9 decimals ,every 3 represent a
     vector. Each vector is a unit vector
  */
  bool saveStress(std::ofstream &stress_fout);

  /**
      @param loc  coordinates of singularites
      find stresses that two of three eig_value are less than tolerance,save to
     loc
  */
  void singularityLoaction(std::vector<Eigen::Vector3d> &loc, double tolerance);

  //设定每个单元的中心点的位置
  bool setCellCenter(VMeshPtr mesh);

  //设定mesh
  const bool set_mesh(VMeshPtr mesh);
  void get_locations(std::vector<Eigen::Vector3d> &ret);
  void get_principal_dirs(std::vector<Eigen::Vector3d> &ret, int P = 0);
  // const std::vector<Matrix_3> &get_dirs();

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

  // std::unordered_set<OvmEgH> singular_ehs;//网格奇异边的集合
  // std::unordered_set<OvmEgH> regular_singular_ehs;//有效奇异线的集合
  // std::unordered_set<OvmEgH> irregular_singular_ehs;//无效奇异线的集合
  // std::unordered_set<OvmEgH> negative_singularities;//负奇异线集合
  // std::unordered_set<OvmEgH> postive_singularities;//正奇异线集合

  /*deprecated


  */

  /*成员函数*/

  bool resize(size_t element_number);
};
} // namespace meshtools