#include "mesh_implement.h"
#include <regex>

namespace meshtools {
void MeshImpl::readFromInp(std::ifstream &fin) {
  /* 读入.inp文件 */

  std::string line_str;

  /*  new a VMesh */

  VMeshPtr mesh_ptr(new VMesh);

  /* 正则表达式  */

  //匹配 *NODE信息
  std::regex node_regex("^\\*node", std::regex::icase);

  //匹配 *ELEMENT信息
  std::regex element_regex("^\\*element", std::regex::icase);

  //匹配 *
  std::regex comments_regex("\\*{2}|^$");

  //输入格式

  std::regex node_format(
      "^([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+)$");
  std::regex element_format(
      "^([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+"
      "),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+)$");

  /*  读入点信息  */

  std::map<TF, OvmFaH> faces;
  faces.clear();

  std::string tmp1, tmp2;
  bool node_section = false, element_section = false;

  while (std::getline(fin, line_str)) {
    if (line_str == "") continue;
    if (line_str[0] == '*') {
      if (std::regex_search(line_str, node_regex)) {
        node_section = true;
        element_section = false;
      } else if (std::regex_search(line_str, element_regex)) {
        node_section = false;
        element_section = true;
      }
    } else {
      if (node_section) {
        //匹配到*NODE，开始从tmp1读入点信息
        int id;
        double x, y, z;
        // std::cout << tmp1.c_str() <<std::endl;
#ifdef __linux
        sscanf(line_str.c_str(), "%d, %lf , %lf , %lf", &id, &x, &y, &z);
#endif  // __linux
#ifdef _WIN64
        sscanf_s(line_str.c_str(), "%d, %lf , %lf , %lf", &id, &x, &y, &z);
#endif  // _WIN64 \
	//插入网格点
        mesh_ptr->add_vertex(MeshPoint(x, y, z));
      } else if (element_section) {
        int id;
        int v[4];
#ifdef __linux
        sscanf(line_str.c_str(), "%d, %d ,%d , %d , %d", &id, v, v + 1, v + 2,
               v + 3);
#endif  // __linux
#ifdef _WIN64
        sscanf_s(line_str.c_str(), "%d, %d ,%d , %d , %d", &id, v, v + 1, v + 2,
                 v + 3);
#endif  // _WIN64 \
	//int转换为handle
        std::vector<OvmVeH> vs;
        vs.clear();
        // inp index start from 1 while OVM from 0
        vs.push_back((OvmVeH)(v[0] - 1));
        vs.push_back((OvmVeH)(v[1] - 1));
        vs.push_back((OvmVeH)(v[2] - 1));
        vs.push_back((OvmVeH)(v[3] - 1));
        // add cell
        addCell(vs, faces);
      }
    }
  }
}

void MeshImpl::readFromOvm(std::ifstream &fin) {
  OpenVolumeMesh::IO::FileManager manager;
  manager.readStream(fin, *ovm_mesh);
}

void MeshImpl::saveToOVM(std::string filename) {
  OpenVolumeMesh::IO::FileManager manager;
  // OpenVolumeMesh::GeometricPolyhedralMeshV3d *mesh;
  manager.writeFile(filename, *ovm_mesh);
}

std::vector<std::string> MeshImpl::separateFilename(std::string filename) {
  std::vector<std::string> ret;
  ret.clear();

  size_t slash_position = filename.find_last_of('/');
  if (slash_position == filename.npos) {
    slash_position = 0;
  }
  size_t dot_position = filename.find_last_of('.');

  /********** path *************/
  ret.push_back(filename.substr(0, slash_position));
  /********** filename *************/
  ret.push_back(filename.substr(slash_position + 1));
  /********** extension *************/
  ret.push_back(filename.substr(dot_position + 1));
  /********** filename without extension *************/
  ret.push_back(filename.substr(0, dot_position));

  return ret;
}

void MeshImpl::get_face_data(
    std::vector<Eigen::Vector3d> &points,
    std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  if (mesh_loaded == false) {
    std::cout << "mesh not loaded" << std::endl;
    return;
  }
  size_t nv = ovm_mesh->n_vertices();
  points.resize(nv);
  size_t nf = ovm_mesh->n_faces();
  faces.resize(nf);

  // get points data
  for (auto viter : ovm_mesh->vertices()) {
    auto p = ovm_mesh->vertex(viter);
    points[viter.idx()] = Eigen::Vector3d(p.data());
  }

  // get faces data
  for (auto fiter : ovm_mesh->faces()) {
    Eigen::Matrix<long long, 3, 1> v;
    int k = 0;
    for (auto j = ovm_mesh->fv_iter(fiter); j.valid(); ++j) {
      v[k] = j->idx();
      k++;
    }
    faces[fiter.idx()] = v;
  }
}

bool MeshImpl::isSameHalfface(const std::vector<int> &f1,
                              const std::vector<int> &f2) {
  if ((f1[0] == f2[0] && f1[1] == f2[1]) ||
      (f1[1] == f2[0] && f1[2] == f2[1]) || (f1[2] == f2[0] && f1[0] == f2[1]))
    return true;
  return false;
}

void MeshImpl::addCell(std::vector<OvmVeH> &v, std::map<TF, OvmFaH> &faces) {
  /*
          变量定义
          */

  OvmFaH f0, f1, f2, f3;
  std::vector<OvmHaFaH> hf;
  std::vector<OvmVeH> fv;

  // f0 : 0->1->2
  TF tf0(v[0].idx(), v[1].idx(), v[2].idx());
  fv.clear();
  fv.push_back(v[0]);
  fv.push_back(v[1]);
  fv.push_back(v[2]);
  if (faces.find(tf0) == faces.end()) {
    f0 = ovm_mesh->add_face(fv);
    faces[tf0] = f0;
    hf.push_back(ovm_mesh->halfface_handle(f0, 0));
  } else {
    f0 = faces.find(tf0)->second;
    hf.push_back(ovm_mesh->halfface_handle(f0, 1));
  }

  // f1 : 0->2->3
  TF tf1(v[0].idx(), v[2].idx(), v[3].idx());
  fv.clear();
  fv.push_back(v[0]);
  fv.push_back(v[2]);
  fv.push_back(v[3]);
  if (faces.find(tf1) == faces.end()) {
    f1 = ovm_mesh->add_face(fv);
    faces[tf1] = f1;
    hf.push_back(ovm_mesh->halfface_handle(f1, 0));
  } else {
    f1 = faces.find(tf1)->second;
    hf.push_back(ovm_mesh->halfface_handle(f1, 1));
  }

  // f2 : 0->3->1
  TF tf2(v[0].idx(), v[3].idx(), v[1].idx());
  fv.clear();
  fv.push_back(v[0]);
  fv.push_back(v[3]);
  fv.push_back(v[1]);
  if (faces.find(tf2) == faces.end()) {
    f2 = ovm_mesh->add_face(fv);
    faces[tf2] = f2;
    hf.push_back(ovm_mesh->halfface_handle(f2, 0));
  } else {
    f2 = faces.find(tf2)->second;
    hf.push_back(ovm_mesh->halfface_handle(f2, 1));
  }

  // f3 : 1->3->2
  TF tf3(v[1].idx(), v[3].idx(), v[2].idx());
  fv.clear();
  fv.push_back(v[1]);
  fv.push_back(v[3]);
  fv.push_back(v[2]);
  if (faces.find(tf3) == faces.end()) {
    f3 = ovm_mesh->add_face(fv);
    faces[tf3] = f3;
    hf.push_back(ovm_mesh->halfface_handle(f3, 0));
  } else {
    f3 = faces.find(tf3)->second;
    hf.push_back(ovm_mesh->halfface_handle(f3, 1));
  }

  // add cell
  ovm_mesh->add_cell(hf);
}

MeshImpl::MeshImpl() { ovm_mesh = VMeshPtr(new VMesh); }

void MeshImpl::readMesh(std::string filename) {
  std::ifstream fin(filename);

  if (mesh_loaded) {
    ovm_mesh = VMeshPtr(new VMesh);
  }

  auto names = separateFilename(filename);
  mesh_name = names[3];
  if (names[2] == "inp") {
    readFromInp(fin);
  } else if (names[2] == "ovm") {
    readFromOvm(fin);
  }

  mesh_loaded = true;
}
}  // namespace meshtools