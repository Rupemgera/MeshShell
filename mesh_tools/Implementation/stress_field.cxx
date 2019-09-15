#include "stress_field.h"

#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <sstream>
#include <stdio.h>

/*
 hypermesh tensor stress order :
 XX YY ZZ XY YZ ZX

 tensor matrix:
 | XX XY XZ |
 | YX YY YZ |
 | ZX ZY ZZ |
*/
namespace meshtools {

void StressTensor::init(double *tensor_component, int order) {
  if (order == 0) {
    _tensor << tensor_component[0], tensor_component[3], tensor_component[5],
        tensor_component[3], tensor_component[1], tensor_component[4],
        tensor_component[5], tensor_component[4], tensor_component[2];
  } else {
    _tensor << tensor_component[0], tensor_component[3], tensor_component[4],
        tensor_component[3], tensor_component[1], tensor_component[5],
        tensor_component[4], tensor_component[5], tensor_component[2];
  }

  Eigen::SelfAdjointEigenSolver<Matrix_3> solver(_tensor);
  auto real_values = solver.eigenvalues();
  double values[] = {real_values(0), real_values(1), real_values(2)};
  int rank[] = {0, 1, 2};
  Matrix_3 vectors = solver.eigenvectors();

  // sort eigen values decreasingly
  for (int i = 0; i < 2; i++) {
    for (int j = i + 1; j < 3; j++) {
      if (values[i] < values[j]) {
        std::swap(values[i], values[j]);
        std::swap(rank[i], rank[j]);
      }
    }
  }

  // save eigen value
  for (int i = 0; i < 3; i++) {
    eig_values[i] = values[i];
    eig_vectors.col(i) = vectors.col(rank[i]);
  }

  // check for right hand
  Eigen::Vector3d c = eig_vectors.col(0).cross(eig_vectors.col(1));

  // if not right-handed
  if (c.dot(eig_vectors.col(2)) < 0) {
    eig_vectors.col(2) = c;
  }
}

StressTensor::StressTensor() {
  double zeros[] = {0, 0, 0, 0, 0, 0};
  init(zeros);
}

StressTensor::StressTensor(double *tensor_component, int order) {
  init(tensor_component, order);
}

void StressTensor::reset(double *tensor_component, int order) {
  init(tensor_component, order);
}

double StressTensor::diff(StressTensor &b) {
  // if compiler support C++14
#if __cplusplus > 201300L
  auto sin = [](auto &u, auto &v) {
    Eigen::Vector3d w = u.cross(v);
    return w.norm();
  };
#else
  using T = decltype(eig_vectors.col(0));
  auto sin = [](T &u, T &v) -> double {
    Eigen::Vector3d w = u.cross(v);
    return w.norm();
  };
#endif // __cplusplus > 201300L

  double dff = 0;
  /* compare major principal vectors */

  dff += sin(this->eig_vectors.col(0), b.eig_vectors.col(0));

  /* compare mid pricipal vectors */

  dff += sin(this->eig_vectors.col(1), b.eig_vectors.col(1));

  /* compare minor pricipal vectors */

  dff += sin(this->eig_vectors.col(2), b.eig_vectors.col(2));

  return dff;
}

double StressTensor::major_diff(StressTensor &b) {
  /* get major principal vectors */
  auto u = this->eig_vectors.col(0).cross(b.eig_vectors.col(0));
  return u.norm();
}

/************************ StressTensor end ***********************************/
/************************ PrincipalStressField begin *************************/
bool PrincipalStressField::readInStress(std::string filename, VMeshPtr mesh,
                                        bool save) {
  std::string tmp;
  std::ifstream stress_fin(filename);

  //读入文件头，判断单元类型
  std::getline(stress_fin, tmp);
  if (tmp.find("Node") != tmp.npos) {
    element_type = STRESS_ELEMENT_TYPE::Node;
  } else if (tmp.find("Element") != tmp.npos) {
    element_type = STRESS_ELEMENT_TYPE::Cell;
  } else {
    // no mesh, only stress
    readStressAbaqusStyle(stress_fin);
    return false;
  }

  // make sure mesh is not NULL
  if (mesh == nullptr) {
    std::cout << "mesh is NULL, please read in mesh first" << std::endl;
  }
  readStressHypermeshStyle(stress_fin, mesh);

  if (save == true) {
    // save to another form
    size_t dot_position = filename.find_last_of('.');
    std::string extension = filename.substr(dot_position + 1);
    std::string rest_filename = filename.substr(0, dot_position + 1);
    std::ofstream fout(rest_filename + "txt");
    saveStress(fout);
  }

  return true;
}

bool PrincipalStressField::saveStress(std::ofstream &fout) {
  for (int i = 0; i < location.size(); ++i) {
    fout << i << ' ';
    fout << tensors[i].eig_vectors(0, 0) << ' ' << tensors[i].eig_vectors(1, 0)
         << ' ' << tensors[i].eig_vectors(2, 0) << ' ';
    fout << tensors[i].eig_vectors(0, 1) << ' ' << tensors[i].eig_vectors(1, 1)
         << ' ' << tensors[i].eig_vectors(2, 1) << ' ';
    fout << tensors[i].eig_vectors(0, 2) << ' ' << tensors[i].eig_vectors(1, 2)
         << ' ' << tensors[i].eig_vectors(2, 2) << ' ';
    fout << location[i][0] << ' ' << location[i][1] << ' ' << location[i][2]
         << std::endl;
  }
  return true;
}

void PrincipalStressField::singularityLoaction(
    std::vector<Eigen::Vector3d> &loc, double tolerance) {
  size_t n = tensors.size();
  auto diff = [tolerance](double &x, double &y, double &z) {
    return std::abs(x - y) < tolerance || std::abs(z - y) < tolerance;
  };
  for (size_t i = 0; i < n; i++) {
    StressTensor &t = tensors[i];
    if (diff(t.eig_values[0], t.eig_values[1], t.eig_values[2])) {
      loc.push_back(Eigen::Vector3d(location[i].data()));
    }
  }
}

// calculate the position of center point of tets
bool PrincipalStressField::setCellCenter(VMeshPtr mesh) {
  location.resize(mesh->n_cells());
  for (auto citer = mesh->cells_begin(); citer != mesh->cells_end(); ++citer) {
    MeshPoint c(0, 0, 0);
    for (auto viter = mesh->cv_iter(*citer); viter.valid(); ++viter) {
      MeshPoint p = mesh->vertex(*viter);
      c += p;
    }
    location[citer->idx()] = Eigen::Vector3d((c / 4).data());
  }
  return true;
}

PrincipalStressField::PrincipalStressField() {
  /* initialize */
  /*_stress.clear();
  _frames.clear();*/
  tensors.clear();
  _n = -1;
}

PrincipalStressField::~PrincipalStressField() {}

const bool PrincipalStressField::set_mesh(VMeshPtr mesh) {
  _mesh = mesh;
  return true;
}

void PrincipalStressField::get_locations(std::vector<Eigen::Vector3d> &ret) {
  ret.reserve(location.size());
  for (auto d : location)
    ret.push_back(Eigen::Vector3d(d.data()));
}

void PrincipalStressField::get_principal_dirs(std::vector<Eigen::Vector3d> &ret,
                                              int P) {
  ret.resize(tensors.size());
  for (int i = 0; i < tensors.size(); ++i) {
    ret[i][0] = tensors[i].eig_vectors(0, P);
    ret[i][1] = tensors[i].eig_vectors(1, P);
    ret[i][2] = tensors[i].eig_vectors(2, P);
  }
}

// bool PrincipalStressField::singular_edges_extraction(VMeshPtr mesh)
//{
//	//
//
//	//先将原来的singularities全部清空
//	std::unordered_set<OvmEgH>().swap(singular_ehs);
//	std::unordered_set<OvmEgH>().swap(regular_singular_ehs);
//	std::unordered_set<OvmEgH>().swap(irregular_singular_ehs);
//	std::unordered_set<OvmEgH>().swap(postive_singularities);
//	std::unordered_set<OvmEgH>().swap(negative_singularities);
//
//	//SOM3
//	meshtools::Frame::set_SOM3();
//	const auto SOM3 = meshtools::Frame::SOM3;
//	const auto regular_inx = meshtools::Frame::regular_inx;
//
//	std::unordered_set<int> sin_idx;
//	////将奇异线输出
//	//std::cout<<"Before writing!"<<std::endl;
//	//std::ofstream fout("regular singularities.txt");
//	for (int s_ind = 0; s_ind < 10; s_ind++)
//	{
//		sin_idx.insert(regular_inx[s_ind]);
//		//	std::cout<<regular_inx[s_ind]<<": "<<std::endl;
//		//	fout<<regular_inx[s_ind]<<":"<<std::endl;
//		//	for (int row_id = 0;row_id < 3;row_id++)
//		//	{
//		//		for (int col_id = 0;col_id < 3;col_id++)
//		//		{
//		//
// fout<<SOM3[regular_inx[s_ind]][row_id][col_id]<<" ";
//		//
// std::cout<<SOM3[regular_inx[s_ind]][row_id][col_id]<<" ";
//		//		}
//		//		std::cout<<std::endl;
//		//		fout<<std::endl;
//		//	}
//	}
//	//std::cout<<"After writting"<<std::endl;
//	//fout.close();
//	//system("pause");
//	//////////////////////////////////////////////////////////////////////////
//	for (auto iter = mesh->edges_begin(); iter != mesh->edges_end(); iter++)
//	{
//		_Matrix_3 fm;
//		if (mesh->is_boundary(*iter))
//		{
//			OvmHaEgH current_heh = mesh->halfedge_handle(*iter, 0);
//			std::vector<OvmHaFaH> adj_hfhs;
//			for (auto hehf_it = mesh->hehf_iter(current_heh);
// hehf_it; hehf_it++)
//			{
//				adj_hfhs.push_back(*hehf_it);
//			}
//			OvmCeH start_cell = mesh->incident_cell(adj_hfhs[0]);
//			OvmCeH end_cell =
// mesh->incident_cell(mesh->opposite_halfface_handle(adj_hfhs[adj_hfhs.size() -
// 1])); 			OvmVec3d a = frames[start_cell].x_vec; OvmVec3d
// b
// =
// frames[end_cell].x_vec; 			_Matrix_3 M; if
// ((a
// - b).length() < 1e-7)
//			{
//				M[0][0] = 1;
//				M[0][1] = 0;
//				M[0][2] = 0;
//				M[1][0] = 0;
//				M[1][1] = 1;
//				M[1][2] = 0;
//				M[2][0] = 0;
//				M[2][1] = 0;
//				M[2][2] = 1;
//			}
//			else
//			{
//				OvmVec3d u = cross(a, b);
//				u = u.normalize();
//				double cro_len = cross(a, b).length();
//				if (cro_len > 1)
//				{
//					cro_len = 1;
//				}
//				double miu = atan2(cro_len, dot(a, b));
//				M[0][0] = cos(miu) + (1 - cos(miu))*u[0] * u[0];
//				M[0][1] = (1 - cos(miu))*u[0] * u[1] -
// sin(miu)*u[2]; 				M[0][2] = (1 - cos(miu))*u[0] *
// u[2]
// + sin(miu)*u[1]; 				M[1][0] = (1 - cos(miu))*u[1] *
// u[0] + sin(miu)*u[2]; 				M[1][1] = cos(miu) + (1
//-
// cos(miu))*u[1] * u[1]; 				M[1][2] = (1 -
// cos(miu))*u[1]
// *
// u[2] - sin(miu)*u[0]; 				M[2][0] = (1 -
// cos(miu))*u[2]
// * u[0] - sin(miu)*u[1]; M[2][1] = (1 - cos(miu))*u[2] * u[1] + sin(miu)*u[0];
// M[2][2] = cos(miu) + (1 - cos(miu))*u[2] * u[2];
//			}
//			Matrix_3 Re(M);
//			Matrix_3 new_F0 = Re *
//(frames[start_cell].m().transpose()); 			OvmVec3d x_v =
// OvmVec3d(new_F0(0,0), new_F0(1,0), new_F0(2,0));
// OvmVec3d y_v = OvmVec3d(new_F0(0,1), new_F0(1,1), new_F0(2,1));
// OvmVec3d z_v = OvmVec3d(new_F0(0,2), new_F0(1,2), new_F0(2,2));
//			meshtools::Frame new_f0(frames[start_cell].center, x_v,
// y_v,
// z_v); 			int index =
// meshtools::Frame::matching_Matrix(new_f0, frames[end_cell]);
// _Matrix_3 final_m = { SOM3[index][0][0], SOM3[index][0][1],
// SOM3[index][0][2], SOM3[index][1][0], SOM3[index][1][1], SOM3[index][1][2],
// SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
// Matrix_3 Final_M(final_m); 			for (int hf_i = 0; hf_i <
// adj_hfhs.size(); hf_i++)
//			{
//				index =
// matching_matrices[adj_hfhs[adj_hfhs.size()
//-
// 1
//- hf_i]]; 				_Matrix_3 m = { SOM3[index][0][0],
// SOM3[index][0][1], SOM3[index][0][2], SOM3[index][1][0], SOM3[index][1][1],
// SOM3[index][1][2], SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
// Final_M = Final_M * Matrix_3(m);
//			}
//			for (int row = 0; row < 3; row++)
//			{
//				for (int col = 0; col < 3; col++)
//				{
//					fm[row][col] = Final_M(row,col);
//				}
//			}
//		}
//		else
//		{
//			std::vector<OvmHaFaH> adj_hfhs;
//			OvmHaEgH current_heh = mesh->halfedge_handle(*iter, 0);
//			for (auto hehf_it = mesh->hehf_iter(current_heh);
// hehf_it; hehf_it++)
//			{
//				adj_hfhs.push_back(*hehf_it);
//			}
//			Matrix_3 Final_M(1);
//			for (int hf_i = 0; hf_i < adj_hfhs.size(); hf_i++)
//			{
//				int index =
// matching_matrices[adj_hfhs[adj_hfhs.size()
//- 1 - hf_i]]; 				_Matrix_3 m = {
// SOM3[index][0][0], SOM3[index][0][1], SOM3[index][0][2], SOM3[index][1][0],
// SOM3[index][1][1],
// SOM3[index][1][2], SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
// Final_M = Final_M * Matrix_3(m);
//			}
//			for (int row = 0; row < 3; row++)
//			{
//				for (int col = 0; col < 3; col++)
//				{
//					fm[row][col] = Final_M.M[row][col];
//				}
//			}
//		}
//		int clo_idx = meshtools::Frame::find_closet_Matrix_index(fm);
//
//		if (clo_idx != regular_inx[0])
//		{
//			singular_ehs.insert(*iter);
//			if (sin_idx.find(clo_idx) != sin_idx.end())
//			{
//				//正常奇异线
//				//std::cout<<"Regular singularity
//"<<clo_idx<<std::endl;
// regular_singular_ehs.insert(*iter);
//
//			}
//			else
//			{
//				//std::cout<<"Irrgular singularity"<<std::endl;
//				irregular_singular_ehs.insert(*iter);
//				//非有效奇异线
//			}
//		}
//	}
//}

void PrincipalStressField::readStressHypermeshStyle(std::ifstream &stress_fin,
                                                    VMeshPtr mesh) {
  //读入数据
  int n;
  std::string tmp;
  double sigma[6];

  // lambda function for read
  auto read_sigma = [&sigma](std::string &line, int &n) {
#ifdef __linux
    sscanf(line.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n, sigma, sigma + 1,
           sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // __linux
#ifdef _WIN64
    sscanf_s(line.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n, sigma, sigma + 1,
             sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // _WIN64
  };

  if (mesh != nullptr) {
    //预分配vector的大小
    if (element_type == STRESS_ELEMENT_TYPE::Node) {
      reserve(mesh->n_vertices());
    } else if (element_type == STRESS_ELEMENT_TYPE::Cell) {
      reserve(mesh->n_cells());
      // calculate tets center poisition
      setCellCenter(mesh);
    }
  }

  while (std::getline(stress_fin, tmp)) {
    if (tmp == "")
      continue;
    /**
       读取stress的六个独立分量
    */
    read_sigma(tmp, n);

    // hypermesh way, default, order = 1
    tensors.push_back(StressTensor(sigma));
  }

  _n = tensors.size();
}

void PrincipalStressField::readStressAbaqusStyle(std::ifstream &stress_fin) {
  double sigma[6];
  double coo[3];
  std::string tmp;

  // initialization

  location.clear();

  // lambda function for read
  auto read_sigma = [&sigma, &coo](std::string &line) {
#ifdef __linux
    sscanf(line.c_str(), "%lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf",
           coo, coo + 1, coo + 2, sigma, sigma + 1, sigma + 2, sigma + 3,
           sigma + 4, sigma + 5);
#endif // __linux
#ifdef _WIN64
    sscanf_s(line.c_str(),
             "%lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf", coo,
             coo + 1, coo + 2, sigma, sigma + 1, sigma + 2, sigma + 3,
             sigma + 4, sigma + 5);
#endif // _WIN64
  };

  while (std::getline(stress_fin, tmp)) {
    if (tmp == "")
      continue;
    /**
       读取stress的六个独立分量
    */
    read_sigma(tmp);

    // abaqus way, order set to 1
    tensors.push_back(StressTensor(sigma, 1));
    location.push_back(Eigen::Vector3d(coo));
  }
}

bool PrincipalStressField::resize(size_t elements_number) {
  _n = elements_number;
  tensors.resize(elements_number);
  location.resize(elements_number);
  return true;
}

bool PrincipalStressField::reserve(size_t elements_number) {
  _n = elements_number;
  tensors.reserve(elements_number);
  location.resize(elements_number);
  return true;
}

} // namespace meshtools