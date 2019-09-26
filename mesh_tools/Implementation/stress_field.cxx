#include "stress_field.h"

#include <stdio.h>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <sstream>

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
  /** different CAD software have different order
   * 0 hypermesh
   * 1 abaqus
   * GAUSS
   */
  if (order == 0) {
    _tensor << tensor_component[0], tensor_component[3], tensor_component[5],
        tensor_component[3], tensor_component[1], tensor_component[4],
        tensor_component[5], tensor_component[4], tensor_component[2];
  } else if (order == 1) {
    _tensor << tensor_component[0], tensor_component[3], tensor_component[4],
        tensor_component[3], tensor_component[1], tensor_component[5],
        tensor_component[4], tensor_component[5], tensor_component[2];
  } else {
    _tensor << tensor_component[0], tensor_component[5], tensor_component[4],
        tensor_component[5], tensor_component[1], tensor_component[3],
        tensor_component[4], tensor_component[3], tensor_component[2];
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
  //#if __cplusplus >= 201300L
  // auto sin = [](auto &u, auto &v) { return (u.cross(v)).norm(); };
  //#else
  // using T = decltype(eig_vectors.col(0));
  // auto sin = [](T &u, T &v) -> double { return (u.cross(v)).norm(); };
  //#endif // __cplusplus > 201300L

  double dff = 0;
  double dff1, dff2;
  dff1 = dff2 = 0;

  /* compare major principal vectors */

  dff += sin(this->eig_vectors.col(0), b.eig_vectors.col(0));

  /* compare mid pricipal vectors */

  dff1 += sin(this->eig_vectors.col(1), b.eig_vectors.col(1));
  dff2 += sin(this->eig_vectors.col(1), b.eig_vectors.col(2));
  /* compare minor pricipal vectors */

  dff1 += sin(this->eig_vectors.col(2), b.eig_vectors.col(2));
  dff2 += sin(this->eig_vectors.col(2), b.eig_vectors.col(1));

  return std::min(dff + dff1, dff + dff2);
}

double StressTensor::major_diff(StressTensor &b) {
  /* get major principal vectors */
  auto u = this->eig_vectors.col(0).cross(b.eig_vectors.col(0));
  return u.norm();
}

double StressTensor::permute_diff(StressTensor &b, Permutation_3 permute) {
  // if compiler support C++14
  //#if __cplusplus >= 201300L
  //  auto sin = [](auto &u, auto &v) { return (u.cross(v)).norm(); };
  //#else
  // using T = decltype(eig_vectors.col(0));
  // auto sin = [](T &u, T &v) -> double { return (u.cross(v)).norm(); };
  //#endif  // __cplusplus > 201300L

  double dff = 0;
  /* compare major principal vectors */

  dff += sin(this->eig_vectors.col(0), b.eig_vectors.col(permute[0]));

  /* compare mid pricipal vectors */

  dff += sin(this->eig_vectors.col(1), b.eig_vectors.col(permute[1]));

  /* compare minor pricipal vectors */

  dff += sin(this->eig_vectors.col(2), b.eig_vectors.col(permute[2]));

  return dff;
}

int StressTensor::get_matching_index(StressTensor &v) {
  double min_diff = 1e20;
  double tmp;
  int index = -1;
  for (int i = 0; i < 6; ++i) {
    tmp = permute_diff(v, Permutation_3::permutations[i]);
    if (tmp < min_diff) {
      min_diff = tmp;
      index = i;
    }
  }
  return index;
}

/************************ StressTensor end ***********************************/

/************************ PrincipalStressField begin *************************/

bool PrincipalStressField::readInStress(std::string filename, VMeshPtr mesh,
                                        bool save) {
  std::string tmp;
  std::ifstream stress_fin(filename);

  // determine if file exits
  if (!stress_fin.good()) {
    return false;
  }

  //读入文件头，判断单元类型
  std::getline(stress_fin, tmp);

  // first char is #, file is processed
  if (tmp[0] == '#'){
    // return to file header
    stress_fin.seekg(0);
    if (tmp.find("node") != tmp.npos)
      readProcessedStress(node_tensors_, stress_fin);
    else
      readProcessedStress(element_tensors_, stress_fin);

    return true;
  }
  
  if (tmp.find("Node") != tmp.npos) {
    element_type = STRESS_ELEMENT_TYPE::Node;
  } else if (tmp.find("Element") != tmp.npos) {
    element_type = STRESS_ELEMENT_TYPE::Cell;
  }

  if (tmp.find("GAUSS") != tmp.npos) {
    readStressGaussStyle(stress_fin);
    return true;
  }
  if (tmp.find("Abaqus") != tmp.npos) {
    // no mesh, only stress
    readStressAbaqusStyle(stress_fin);
    return true;
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
} // namespace meshtools

bool PrincipalStressField::saveStress(std::ofstream &fout) {
  for (int i = 0; i < location.size(); ++i) {
    fout << i << ' ';
    fout << element_tensors_[i].eig_vectors(0, 0) << ' ' << element_tensors_[i].eig_vectors(1, 0)
         << ' ' << element_tensors_[i].eig_vectors(2, 0) << ' ';
    fout << element_tensors_[i].eig_vectors(0, 1) << ' ' << element_tensors_[i].eig_vectors(1, 1)
         << ' ' << element_tensors_[i].eig_vectors(2, 1) << ' ';
    fout << element_tensors_[i].eig_vectors(0, 2) << ' ' << element_tensors_[i].eig_vectors(1, 2)
         << ' ' << element_tensors_[i].eig_vectors(2, 2) << ' ';
    fout << location[i][0] << ' ' << location[i][1] << ' ' << location[i][2]
         << std::endl;
  }
  return true;
}

void PrincipalStressField::singularityLoaction(
    std::vector<Eigen::Vector3d> &loc, double tolerance) {
  size_t n = element_tensors_.size();
  auto diff = [tolerance](double &x, double &y, double &z) {
    return std::abs(x - y) < tolerance || std::abs(z - y) < tolerance;
  };
  for (size_t i = 0; i < n; i++) {
    StressTensor &t = element_tensors_[i];
    if (diff(t.eig_values[0], t.eig_values[1], t.eig_values[2])) {
      loc.push_back(Eigen::Vector3d(location[i].data()));
    }
  }
}

/**
 *@brief calculate the position of center point of tets, store results in loc
 */
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
  element_tensors_.clear();
  _n = -1;
}

PrincipalStressField::~PrincipalStressField() {}

const bool PrincipalStressField::set_mesh(VMeshPtr mesh) {
  _mesh = mesh;
  return true;
}

void PrincipalStressField::get_locations(std::vector<Eigen::Vector3d> &ret) {
  ret.reserve(location.size());
  for (auto d : location) ret.push_back(Eigen::Vector3d(d.data()));
}

void PrincipalStressField::get_principal_dirs(std::vector<Eigen::Vector3d> &ret,
                                              int P) {
  ret.resize(element_tensors_.size());
  for (int i = 0; i < element_tensors_.size(); ++i) {
    ret[i][0] = element_tensors_[i].eig_vectors(0, P);
    ret[i][1] = element_tensors_[i].eig_vectors(1, P);
    ret[i][2] = element_tensors_[i].eig_vectors(2, P);
  }
}

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
#endif  // __linux
#ifdef _WIN64
    sscanf_s(line.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n, sigma, sigma + 1,
             sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif  // _WIN64
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
    if (tmp == "") continue;
    /**
       读取stress的六个独立分量
    */
    read_sigma(tmp, n);

    // hypermesh way, default, order = 1
    element_tensors_.push_back(StressTensor(sigma));
  }

  _n = element_tensors_.size();
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
#endif  // __linux
#ifdef _WIN64
    sscanf_s(line.c_str(),
             "%lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf , %lf", coo,
             coo + 1, coo + 2, sigma, sigma + 1, sigma + 2, sigma + 3,
             sigma + 4, sigma + 5);
#endif  // _WIN64
  };

  while (std::getline(stress_fin, tmp)) {
    if (tmp == "") continue;
    /**
       读取stress的六个独立分量
    */
    read_sigma(tmp);

    // abaqus way, order set to 1
    element_tensors_.push_back(StressTensor(sigma, 1));
    location.push_back(Eigen::Vector3d(coo));
  }
}

// GAUSS style XX YY ZZ YZ XZ XY
void PrincipalStressField::readStressGaussStyle(std::ifstream &stress_fin) {
  std::string line;
  double sigma[6];
  //
  while (stress_fin >> sigma[0]) {
    for (size_t i = 1; i < 6; i++) {
      stress_fin >> sigma[i];
    }
    element_tensors_.push_back(StressTensor(sigma, 2));
  }
}

void PrincipalStressField::readProcessedStress(std::vector<StressTensor>& tensors, std::ifstream &stress_fin, int n_lines) {
  std::string flag;
  int n;
  double sigma[6];
  // first line : # type number
  if (n_lines > 0)
    tensors.reserve(n);

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < 6; j++) {
      stress_fin>>sigma[j];
    }
    tensors.push_back(StressTensor(sigma));
  }
}

bool PrincipalStressField::resize(size_t elements_number) {
  _n = elements_number;
  element_tensors_.resize(elements_number);
  location.resize(elements_number);
  return true;
}

bool PrincipalStressField::reserve(size_t elements_number) {
  _n = elements_number;
  element_tensors_.reserve(elements_number);
  location.resize(elements_number);
  return true;
}

}  // namespace meshtools