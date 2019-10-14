#include "stress_field.h"

#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <cmath>
#include <sstream>
#include <stdio.h>

/**
alias
*/

using V3d = Eigen::Vector3d;

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
    matrix_ << tensor_component[0], tensor_component[3], tensor_component[5],
        tensor_component[3], tensor_component[1], tensor_component[4],
        tensor_component[5], tensor_component[4], tensor_component[2];
  } else if (order == 1) {
    matrix_ << tensor_component[0], tensor_component[3], tensor_component[4],
        tensor_component[3], tensor_component[1], tensor_component[5],
        tensor_component[4], tensor_component[5], tensor_component[2];
  } else {
    matrix_ << tensor_component[0], tensor_component[5], tensor_component[4],
        tensor_component[5], tensor_component[1], tensor_component[3],
        tensor_component[4], tensor_component[3], tensor_component[2];
  }
}

void StressTensor::decompose() {
  Eigen::SelfAdjointEigenSolver<Matrix_3> solver(matrix_);
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
  V3d c = eig_vectors.col(0).cross(eig_vectors.col(1));

  // if not right-handed
  if (c.dot(eig_vectors.col(2)) < 0) {
    eig_vectors.col(2) = c;
  }
}

StressTensor::StressTensor() {
  double zeros[] = {0, 0, 0, 0, 0, 0};
  init(zeros);
  decompose();
}

StressTensor::StressTensor(Matrix_3 matrix) {
  matrix_ = matrix;
  decompose();
}

StressTensor::StressTensor(double *tensor_component, int order) {
  init(tensor_component, order);
  decompose();
}

void StressTensor::reset(double *tensor_component, int order) {
  init(tensor_component, order);
  decompose();
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

double StressTensor::von_mises() {
  double s1 = eig_values[0] - eig_values[1];
  double s2 = eig_values[1] - eig_values[2];
  double s3 = eig_values[2] - eig_values[0];
  auto v = V3d(s1, s2, s3);
  return sqrt(v.dot(v) / 2);
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
    saveEigenVectors(fout);
  }

  return true;
} // namespace meshtools

bool PrincipalStressField::readProcessedStress(std::string filename,
                                               VMeshPtr mesh) {
  std::ifstream stress_fin(filename);
  std::string flag;
  int n;
  stress_fin >> flag >> flag >> n;

  auto read = [&stress_fin, n](std::vector<StressTensor> &tensors) {
    double sigma[6];
    tensors.reserve(n);
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < 6; j++) {
        stress_fin >> sigma[j];
      }
      tensors.push_back(StressTensor(sigma));
    }
  };

  if (flag == "node") {
    read(node_tensors_);
    init_element_tensors(mesh);
  } else
    read(cell_tensors_);
  return true;
}

bool PrincipalStressField::saveEigenVectors(std::ofstream &fout) {
  for (int i = 0; i < location.size(); ++i) {
    fout << i << ' ';
    fout << cell_tensors_[i].eig_vectors(0, 0) << ' '
         << cell_tensors_[i].eig_vectors(1, 0) << ' '
         << cell_tensors_[i].eig_vectors(2, 0) << ' ';
    fout << cell_tensors_[i].eig_vectors(0, 1) << ' '
         << cell_tensors_[i].eig_vectors(1, 1) << ' '
         << cell_tensors_[i].eig_vectors(2, 1) << ' ';
    fout << cell_tensors_[i].eig_vectors(0, 2) << ' '
         << cell_tensors_[i].eig_vectors(1, 2) << ' '
         << cell_tensors_[i].eig_vectors(2, 2) << ' ';
    fout << location[i][0] << ' ' << location[i][1] << ' ' << location[i][2]
         << std::endl;
  }
  return true;
}

bool PrincipalStressField::init_element_tensors(VMeshPtr mesh) {
  assert(mesh->n_vertices() == node_tensors_.size());

  cell_tensors_.resize(mesh->n_cells());

  for (auto ci : mesh->cells()) {
    int k = 0;
    Matrix_3 tmp;
    tmp.setZero();
    for (auto cvi = mesh->cv_iter(ci); cvi.valid(); ++cvi) {
      tmp += node_tensors_[cvi->idx()].matrix_;
      k++;
    }
    if (k != 0) {
      tmp /= k;
    }
    cell_tensors_[ci.idx()] = StressTensor(tmp);
  }

  return true;
}

void PrincipalStressField::singularityLoaction(std::vector<V3d> &loc,
                                               double tolerance) {
  size_t n = cell_tensors_.size();
  auto diff = [tolerance](double &x, double &y, double &z) {
    return std::abs(x - y) < tolerance || std::abs(z - y) < tolerance;
  };
  for (size_t i = 0; i < n; i++) {
    StressTensor &t = cell_tensors_[i];
    if (diff(t.eig_values[0], t.eig_values[1], t.eig_values[2])) {
      loc.push_back(V3d(location[i].data()));
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
    location[citer->idx()] = V3d((c / 4).data());
  }

  return true;
}

PrincipalStressField::PrincipalStressField() {
  /* initialize */
  /*_stress.clear();
  _frames.clear();*/
  cell_tensors_.clear();
  _n = -1;
}

PrincipalStressField::~PrincipalStressField() {}

const bool PrincipalStressField::set_mesh(VMeshPtr mesh) {
  _mesh = mesh;
  return true;
}

void PrincipalStressField::get_locations(std::vector<V3d> &ret) {
  ret.reserve(location.size());
  for (auto d : location)
    ret.push_back(V3d(d.data()));
}

void PrincipalStressField::get_principal_dirs(std::vector<V3d> &ret, int P) {
  ret.resize(cell_tensors_.size());
  for (int i = 0; i < cell_tensors_.size(); ++i) {
    ret[i][0] = cell_tensors_[i].eig_vectors(0, P);
    ret[i][1] = cell_tensors_[i].eig_vectors(1, P);
    ret[i][2] = cell_tensors_[i].eig_vectors(2, P);
  }
}

void PrincipalStressField::get_von_mises(std::vector<double> &von_mises) {
  von_mises.clear();
  von_mises.reserve(cell_tensors_.size());
  for (auto &u : cell_tensors_) {
    von_mises.push_back(u.von_mises());
    // von_mises.push_back(std::abs(u.eig_values[0]));
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
    cell_tensors_.push_back(StressTensor(sigma));
  }

  _n = cell_tensors_.size();
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
    cell_tensors_.push_back(StressTensor(sigma, 1));
    location.push_back(V3d(coo));
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
    cell_tensors_.push_back(StressTensor(sigma, 2));
  }
}

bool PrincipalStressField::resize(size_t elements_number) {
  _n = elements_number;
  cell_tensors_.resize(elements_number);
  location.resize(elements_number);
  return true;
}

bool PrincipalStressField::reserve(size_t elements_number) {
  _n = elements_number;
  cell_tensors_.reserve(elements_number);
  location.resize(elements_number);
  return true;
}

V3d VectorField::Euler2Vector(double theta, double phi) {
  double z = std::cos(phi);
  double v_z = std::sin(phi);
  double x = std::cos(theta) * v_z;
  double y = std::sin(theta) * v_z;
  return V3d(x, y, z);
}

void VectorField::Vector2Euler(Eigen::Vector3d v, double &theta, double &phi) {
  dlib::matrix<double, 1, 2> angle;
  auto v_z = V3d(v(0), v(1), 0);
  v_z.normalize();
  angle = {v_z.dot(V3d(1, 0, 0)), v.dot(V3d(0, 0, 1))};
  angle = dlib::acos(angle);
  theta = angle(0);
  phi = angle(1);
}

double VectorField::dot(double t1, double t2, double p1, double p2) {
  double sp1 = std::sin(p1);
  double sp2 = std::sin(p2);
  double cp1 = std::cos(p1);
  double cp2 = std::cos(p2);

  return sp1 * sp2 * std::cos(t1 - t2) + cp1 * cp2;
}

double VectorField::metric_func(double t1, double p1, double t2, double p2) {
  return std::pow(t1 - t2, 2) + std::pow(p1 - p2, 2);
}

double VectorField::metric_grad1(double t1, double t2, double p1, double p2) {
  return 2 * (t1 - t2);
}

double VectorField::metric_grad2(double t1, double p1, double t2, double p2) {
  return 2 * (p1 - p2);
}

double VectorField::obj_func(column_vector &m, double w_consist,
                             double w_smooth) {
  double obj = 0.0;
  double consist_value = 0.0;
  double smooth_value = 0.0;
  int n = euler1_.size(), n1, n2;
  double theta, phi;
  for (size_t i = 0; i < n; i++) {
    // consistance to original vector
    // consist_value += dot(theta, phi, euler1_[i], euler2_[i]);
    consist_value +=
        metric_func(m(i * 2), m(i * 2 + 1), euler1_[i], euler2_[i]);
  }

  // smoothness to neighbor vectors
  for (int j = 0; j < variable_pairs_.nr(); ++j) {
    int k = variable_pairs_(j, 0);
    int z = variable_pairs_(j, 1);
    smooth_value += metric_func(m(k * 2), m(k * 2 + 1), m(z * 2), m(z * 2 + 1));
  }

  obj = consist_value * w_consist + smooth_value * w_smooth;
  return obj;
}

column_vector VectorField::gradient_func(column_vector &m, double w_consist,
                                         double w_smooth) {
  int n = euler1_.size(), n1, n2;
  double consist_value, smooth_value, theta, phi;
  column_vector grad_consist = dlib::zeros_matrix<double>(m.nr(), 1);
  column_vector grad_smooth = dlib::zeros_matrix<double>(m.nr(), 1);

  for (size_t i = 0; i < n; i++) {
    n1 = i * 2;
    n2 = i * 2 + 1;
    grad_consist(n1) += metric_grad1(m(n1), m(n2), euler1_[i], euler2_[i]);
    grad_consist(n2) += metric_grad2(m(n1), m(n2), euler1_[i], euler2_[i]);
  }

  // smooth value
  for (size_t j = 0; j < variable_pairs_.nr(); j++) {
    n1 = variable_pairs_(j, 0) * 2;
    n2 = variable_pairs_(j, 0) * 2 + 1;
    int k1 = variable_pairs_(j, 1) * 2;
    int k2 = variable_pairs_(j, 1) * 2 + 1;
    grad_smooth(n1) += metric_grad1(m(n1), m(n2), m(k1), m(k2));
    grad_smooth(n2) += metric_grad2(m(n1), m(n2), m(k1), m(k2));
    grad_smooth(k1) += metric_grad1(m(k1), m(k2), m(n1), m(n2));
    grad_smooth(k2) += metric_grad2(m(k1), m(k2), m(n1), m(n2));
  }

  return grad_consist * w_consist + grad_smooth * w_smooth;
}

VectorField::VectorField(VMeshPtr mesh, std::vector<V3d> &vectors) {
  size_t _n = vectors.size();
  euler1_.reserve(_n);
  euler2_.reserve(_n);
  double phi, theta;
  for (auto v : vectors) {
    Vector2Euler(v, theta, phi);
    euler1_.push_back(theta);
    euler2_.push_back(phi);
  }

  auto edges = dlib::matrix<int>(_n * 4, 2);

  int k = 0;
  for (int i = 0; i < _n; ++i) {
    auto ch = OvmCeH(i);
    for (auto c = mesh->cc_iter(ch); c.valid(); ++c) {
      edges(k, 0) = i;
      edges(k, 1) = c->idx();
      k++;
    }
  }

  // discard excess parts
  variable_pairs_ = dlib::rowm(edges, dlib::range(0, k - 1));
}

std::vector<V3d> VectorField::smooth_vector_field(double w_consistance,
                                                  double w_smooth) {
  std::vector<V3d> smoothed_vectors;
  size_t _n = euler1_.size();
  column_vector variables(_n * 2);

  for (int i = 0; i < _n; ++i) {
    variables(i * 2) = euler1_[i];
    variables(i * 2 + 1) = euler2_[i];
  }

  // object function
  auto object_func = [this, w_consistance, w_smooth](column_vector &m) {
    return obj_func(m, w_consistance, w_smooth);
  };

  // derivative
  auto derivative_func = [this, w_consistance, w_smooth](column_vector &m) {
    return gradient_func(m, w_consistance, w_smooth);
  };

  // optimize
  dlib::find_min(dlib::bfgs_search_strategy(),
                 dlib::objective_delta_stop_strategy(1e-7), object_func,
                 derivative_func, variables, -1);

  smoothed_vectors.resize(_n);
  for (size_t i = 0; i < _n; i++) {
    smoothed_vectors[i] = Euler2Vector(variables(i * 2), variables(i * 2 + 1));
  }

  return smoothed_vectors;
}
} // namespace meshtools