#pragma once
/***
####################
此文件中包含：
1.读取点以及该点的应力，并计算主应力方向
####################
***/

#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <Eigen/Dense>

#include "frame_field.h"

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
enum STRESS_ELEMENT_TYPE
{
	Node,
	Cell,
	NotDefined
};

/*
主应力场类
成员变量：
点集：保存每点的坐标
向量集：每点的三个主应力方向，大小从小到大
成员函数：

*/
class PrincipalStressField
{
public:
	/*deprecated

	//读入mesh
	bool readInMesh(std::ifstream &mesh_fin, std::string filename_extension);

	PrincipalStressField(VMeshPtr mesh);

	PrincipalStressField(std::string mesh_filename, std::string stress_filename);
	*/

	/*构造函数*/

	PrincipalStressField();

	/*析构函数*/
	~PrincipalStressField();

	/*类成员函数*/
	//读入单元应力
	bool readInStress(std::string filename, VMeshPtr mesh = nullptr);

	//保存单元应力
	bool saveStress(std::ofstream &stress_fout);

	//设定每个单元的中心点的位置
	bool setCellCenter(VMeshPtr mesh);

	//设定mesh
	const bool set_mesh(VMeshPtr mesh);
	void get_locations(std::vector<MeshPoint> &ret);
	void get_principal_dirs(std::vector<MeshPoint> &ret, int P = 0);
	const std::vector<Matrix_3> &get_dirs();

	/*****************
	not completed yet

	bool singular_edges_extraction(VMeshPtr mesh);

	*******************/

	/*类成员变量*/

	STRESS_ELEMENT_TYPE element_type = STRESS_ELEMENT_TYPE::NotDefined;

private:
	/*成员变量*/
	size_t _n;
	std::vector<MeshPoint> _location;
	std::vector<Matrix_3> _stress;
	std::vector<Matrix_3> _frames;
	//mesh的指针
	std::shared_ptr<VMesh> _mesh = nullptr;

	//std::unordered_set<OvmEgH> singular_ehs;//网格奇异边的集合
	//std::unordered_set<OvmEgH> regular_singular_ehs;//有效奇异线的集合
	//std::unordered_set<OvmEgH> irregular_singular_ehs;//无效奇异线的集合
	//std::unordered_set<OvmEgH> negative_singularities;//负奇异线集合
	//std::unordered_set<OvmEgH> postive_singularities;//正奇异线集合

	/*deprecated

	
	*/

	/*成员函数*/

	Matrix_3 cal_principal_direction(Matrix_3 &sigma);

	bool resize(size_t element_number);

	/* inline function */
	inline Matrix_3 vectorFormToMatrixForm(double sigma[6]);
};