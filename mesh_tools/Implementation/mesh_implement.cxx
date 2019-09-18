#include "mesh_implement.h"
#include <assert.h>
#include <regex>
#include <set>

namespace meshtools {
void MeshImpl::readFromInp(std::ifstream &fin) {
  /* 读入.inp文件 */

  std::string line_str;

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
  std::regex element_format("^([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+"
                            "),([\\d\\.Ee\\s]+),([\\d\\.Ee\\s]+)$");

  /*  读入点信息  */

  std::map<TF, OvmFaH> faces;
  faces.clear();

  std::string tmp1, tmp2;
  bool node_section = false, element_section = false;

  while (std::getline(fin, line_str)) {
    if (line_str == "")
      continue;
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
#endif // __linux
#ifdef _WIN64
        sscanf_s(line_str.c_str(), "%d, %lf , %lf , %lf", &id, &x, &y, &z);
#endif // _WIN64 \
	//插入网格点
        ovm_mesh->add_vertex(MeshPoint(x, y, z));
      } else if (element_section) {
        int id;
        int v[4];
#ifdef __linux
        sscanf(line_str.c_str(), "%d, %d ,%d , %d , %d", &id, v, v + 1, v + 2,
               v + 3);
#endif // __linux
#ifdef _WIN64
        sscanf_s(line_str.c_str(), "%d, %d ,%d , %d , %d", &id, v, v + 1, v + 2,
                 v + 3);
#endif // _WIN64 \
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

void MeshImpl::getFaceData(std::vector<Eigen::Vector3d> &points,
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

void MeshImpl::getBoundaryFaceIds(std::vector<int> &faceids_list) {
  faceids_list.clear();
  for (auto fiter : ovm_mesh->faces()) {
    if (ovm_mesh->is_boundary(fiter)) {
      faceids_list.push_back(fiter.idx());
    }
  }
}

void MeshImpl::getShrinkMesh(
    std::vector<Eigen::Vector3d> &points,
    std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  double rate = 0.2;
  size_t n = ovm_mesh->n_cells();
  faces.clear();
  points.clear();
  faces.reserve(n * 4);
  points.reserve(n * 4);
  /*faces.resize(n * 4);
  points.resize(n * 4);*/
  int k = 0, idf = 0;
  for (auto citer : ovm_mesh->cells()) {
    std::vector<OvmVec3d> vertices;
    OvmVec3d tet_center(0.0, 0.0, 0.0);
    for (auto vciter = ovm_mesh->cv_iter(citer); vciter.valid(); ++vciter) {
      vertices.push_back(ovm_mesh->vertex(*vciter));
      tet_center += ovm_mesh->vertex(*vciter);
    }
    tet_center /= vertices.size();
    for (auto &v : vertices) {
      v = v + (tet_center - v) * rate;
      points.push_back(Eigen::Vector3d(v.data()));
    }
    long long v[4] = {k, k + 1, k + 2, k + 3};
    k += 4;
    // 将四个顶点构成的tet的四个面push进faces
    tetFaces(faces, v);
  }

  assert(faces.size() == n * 4);
  assert(points.size() == n * 4);
}

void MeshImpl::get_edge_data(std::vector<OvmEgH> &edge_ids,
                             std::vector<Eigen::Vector3d> &edge_points) {
  edge_points.reserve(edge_ids.size()*2);
  for (auto eh : edge_ids) {
    OvmVeH u = ovm_mesh->edge(eh).to_vertex();
    OvmVeH v = ovm_mesh->edge(eh).from_vertex();
    edge_points.push_back(Eigen::Vector3d(ovm_mesh->vertex(u).data()));
    edge_points.push_back(Eigen::Vector3d(ovm_mesh->vertex(v).data()));
  }
}

double MeshImpl::cellSize() {
  double maxr = 0, minr = 1e20;
  // size is defined as  body center to vertex distance
  for (auto citer = ovm_mesh->cells_begin(); citer != ovm_mesh->cells_end();
       ++citer) {
    MeshPoint c(0, 0, 0);
    MeshPoint p[4];
    int k = 0;
    for (auto viter = ovm_mesh->cv_iter(*citer); viter.valid(); ++viter) {
      p[k] = ovm_mesh->vertex(*viter);
      c += p[k];
      k++;
    }
    c = c / 4;
    double cr = 1e20;
    for (size_t i = 0; i < 4; i++) {
      double tmp = (p[i] - c).norm();
      if (tmp < cr)
        cr = tmp;
    }
    if (cr < minr)
      minr = cr;
    if (cr > maxr)
      maxr = cr;
  }
  return (maxr + minr) / 2;
}

OvmFaH MeshImpl::commonFace(OvmCeH ch1, OvmCeH ch2) {
  OvmFaH com_fh = ovm_mesh->InvalidFaceHandle;
  auto hfhs_vec1 = ovm_mesh->cell(ch1).halffaces(),
       hfhs_vec2 = ovm_mesh->cell(ch2).halffaces();

  std::set<OvmFaH> fhs_set1, fhs_set2;
  std::for_each(hfhs_vec1.begin(), hfhs_vec1.end(), [&](OvmHaFaH hfh) {
    fhs_set1.insert(ovm_mesh->face_handle(hfh));
  });
  std::for_each(hfhs_vec2.begin(), hfhs_vec2.end(), [&](OvmHaFaH hfh) {
    fhs_set2.insert(ovm_mesh->face_handle(hfh));
  });

  std::vector<OvmFaH> com_fhs;
  std::set_intersection(fhs_set1.begin(), fhs_set1.end(), fhs_set2.begin(),
                        fhs_set2.end(), std::back_inserter(com_fhs));
  if (com_fhs.size() == 1)
    com_fh = com_fhs.front();
  return com_fh;
}

void MeshImpl::construct_matching_graph(std::vector<StressTensor> &stresses) {
  if (matching_graph != nullptr) {
    // delete matching_graph;
    // all ready exits
    return;
  }
  size_t n = ovm_mesh->n_cells();
  matching_graph = new MatchingGraph(n);

  MatchingEdge me;
  // add edge
  for (auto ci : ovm_mesh->cells()) {
    for (auto cci = ovm_mesh->cc_iter(ci); cci.valid(); ++cci) {
      auto &c_stress = stresses[ci.idx()];
      auto &cc_stress = stresses[cci->idx()];
      me.diff = c_stress.diff(cc_stress);
      me.matching_index = c_stress.get_matching_index(cc_stress);
      boost::add_edge(ci.idx(), cci->idx(), me, *matching_graph);
    }
  }
}

int MeshImpl::get_cell_loop(OvmHaEgH halfedge, std::vector<int> &cell_loop) {
  if (matching_graph != nullptr) {
    cell_loop.clear();
    // start cell
    auto citer = ovm_mesh->hec_iter(halfedge);
    OvmCeH ch = *(citer);
    VertexIter u, last_u, start_u;
    EdgeIter jump_edge;
    u = boost::vertex(ch.idx(), *matching_graph);
    last_u = u;
    start_u = u;
    // store all cells adjacent to halfedge
    std::set<int> adjacent_cells;
    for (; citer.valid(); ++citer) {
      adjacent_cells.insert(citer->idx());
    }
    // get edge property
    // boost::property_map<MatchingGraph,boost::edge_weight_t>::type edge_map =
    // boost::get(boost::edge_weight,*matching_graph);
    auto edge_map = boost::get(boost::edge_weight, *matching_graph);
    //
    Permutation_3 edge_permute = Permutation_3::permutations[0];
    int edge_index = 0;
    bool loop_not_found;
    VertexIter v = 0;
    // find the loop
    do {
      loop_not_found = true;
      auto ei = boost::out_edges(u, *matching_graph);
      for (auto next = ei.first; next != ei.second; ++next) {
        v = boost::target(*next, *matching_graph);
        // if v is a cell adjacent to halfedge and is not visited
        if (v != last_u && adjacent_cells.find((int)v) != adjacent_cells.end()) {
          // do match transform
          auto trans_match = edge_map[*next];
          edge_index =
              Permutation_3::transform(edge_index, trans_match.matching_index);

          // for test
          // std::cout<<trans_match.matching_index<<" -->
          // "<<edge_index<<std::endl;

          loop_not_found = false;
          break;
        }
      }
      // loop is not exits
      if (loop_not_found) {
        return -1;
      }
      // jump to next
      cell_loop.push_back((int)u);
      last_u = u;
      u = v;
    } while (v != start_u);
    return edge_index;
  }
  return -1;
}

int MeshImpl::get_edge_matching_index(OvmHaEgH halfedge_handle) {
  if (matching_graph != nullptr) {
    // start cell
    auto citer = ovm_mesh->hec_iter(halfedge_handle);
    OvmCeH ch = *(citer);
    VertexIter u, last_u, start_u;
    EdgeIter jump_edge;
    u = boost::vertex(ch.idx(), *matching_graph);
    last_u = u;
    start_u = u;
    // store all cells adjacent to halfedge
    std::set<int> adjacent_cells;
    for (; citer.valid(); ++citer) {
      adjacent_cells.insert(citer->idx());
    }
    auto edge_map = boost::get(boost::edge_weight, *matching_graph);
    //
    Permutation_3 edge_permute = Permutation_3::permutations[0];
    int edge_index = 0;
    bool loop_not_found;
    VertexIter v = 0;
    // find the loop
    do {
      loop_not_found = true;
      auto ei = boost::out_edges(u, *matching_graph);
      for (auto next = ei.first; next != ei.second; ++next) {
        v = boost::target(*next, *matching_graph);
        // if v is a cell adjacent to halfedge and is not visited
        if (adjacent_cells.find((int)v) != adjacent_cells.end() && v != last_u) {
          // do match transform
          edge_index = Permutation_3::transform(edge_index,
                                                edge_map[*next].matching_index);
          loop_not_found = false;
          break;
        }
      }
      // loop is not exits
      if (loop_not_found) {
        return -1;
      }
      // jump to next
      last_u = u;
      u = v;
    } while (v != start_u);
    return edge_index;
  }
  return -1;
}

void MeshImpl::assignCellStress(std::vector<StressTensor> &tensors) {
  assert(tensors.size() == ovm_mesh->n_cells());
  size_t n = ovm_mesh->n_cells();
  auto cell_stress =
      ovm_mesh->request_cell_property<StressTensor>("stress_tensor");
  // otherwise cell_stress will be released
  for (int i = 0; i < n; i++) {
    cell_stress[static_cast<OvmCeH>(i)] = tensors[i];
  }
  ovm_mesh->set_persistent(cell_stress);
}

bool MeshImpl::request_cell_centers() {
  // if property is not calculated
  if (!cell_center_exits) {
    cell_centers.resize(ovm_mesh->n_cells());
    for (auto citer : ovm_mesh->cells()) {
      MeshPoint c(0, 0, 0);
      for (auto viter = ovm_mesh->cv_iter(citer); viter.valid(); ++viter) {
        c += ovm_mesh->vertex(*viter);
      }
      cell_centers[citer.idx()] = Eigen::Vector3d((c / 4).data());
    }
    cell_center_exits = true;
  }
  return true;
}

void MeshImpl::divideCells(std::vector<StressTensor> &tensors,
                           std::vector<int> &split_face_ids, double tolerance) {
  split_face_ids.clear();
  for (auto ceh : ovm_mesh->cells()) {
    for (auto cci = ovm_mesh->cc_iter(ceh); cci.valid(); ++cci) {
      double dff = tensors[ceh.idx()].major_diff(tensors[cci->idx()]);
      if (dff > tolerance) {
        split_face_ids.push_back(commonFace(ceh, *cci).idx());
      }
    }
  }
}

bool MeshImpl::isSameHalfface(const std::vector<int> &f1,
                              const std::vector<int> &f2) {
  if ((f1[0] == f2[0] && f1[1] == f2[1]) ||
      (f1[1] == f2[0] && f1[2] == f2[1]) || (f1[2] == f2[0] && f1[0] == f2[1]))
    return true;
  return false;
}

/**
 *添加面时顶点顺序为右手法则法向朝外
 */
void MeshImpl::addCell(std::vector<OvmVeH> &v, std::map<TF, OvmFaH> &faces) {
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

void MeshImpl::tetFaces(std::vector<Eigen::Matrix<long long, 3, 1>> &faces,
                        long long v[4]) {
  for (int i = 0; i < 4; ++i) {
    Eigen::Matrix<long long, 3, 1> face_vertices;
    int k = 0;
    for (auto j = 0; j < 4; ++j) {
      if (j != i) {
        face_vertices[k] = v[j];
        k++;
      }
    }
    faces.push_back(face_vertices);
  }
}

MeshImpl::MeshImpl() {
  ovm_mesh = VMeshPtr(new VMesh);
  // field = new PrincipalStressField();
}

/**
 *delete matching_graph
 */
MeshImpl::~MeshImpl() {
  if (matching_graph != nullptr) {
    delete matching_graph;
  }
} // delete field; }

void MeshImpl::readMesh(std::string filename) {
  std::ifstream fin(filename);

  // 若之前已经载入过mesh了，则将之前的注销重新new一个空mesh
  if (mesh_loaded) {
    ovm_mesh = VMeshPtr(new VMesh);
  }

  auto names = separateFilename(filename);
  mesh_name = names[3];
  if (names[2] == "inp") {
    readFromInp(fin);
    std::string ovm_file_name = names[3] + ".ovm";
    saveToOVM(ovm_file_name);
  } else if (names[2] == "ovm") {
    readFromOvm(fin);
  }

  mesh_loaded = true;

  // get cell centers
  request_cell_centers();
}
} // namespace meshtools