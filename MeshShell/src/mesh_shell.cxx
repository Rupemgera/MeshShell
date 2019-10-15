#include "mesh_shell.h"
#include <iostream>

#define NORMAL_NAME "normal_mesh"
#define SHRINKED_NAME "shrinked_mesh"

TetMeshData::TetMeshData() {
  points.clear();
  faces.clear();
  boundary_faces.clear();
}

void TetMeshData::getFaceData(std::vector<int> &face_ids,
                              std::vector<meshtools::FaceVertices<3>> &ext) {
  ext.clear();
  for (auto id : face_ids) {
    ext.push_back(faces[id]);
  }
}

MeshShell::MeshShell(ViewManager *viewer) : _viewer(viewer) {
  mesh_wrapper = new MeshWrapper();
}

/**
 *viewer should be released by caller
 */
MeshShell::~MeshShell() { delete mesh_wrapper; }

void MeshShell::drawMesh(int nRenderStyle) {
  if (!_viewer->exist(NORMAL_NAME)) {
    std::vector<int> boundary_face_id_list;

    mesh_wrapper->getFaceData(mesh_data.points, mesh_data.faces);
    mesh_wrapper->getBoundaryFaceIds(boundary_face_id_list);
    mesh_data.getFaceData(boundary_face_id_list, mesh_data.boundary_faces);

    mesh_name = mesh_wrapper->get_mesh_name();

    _viewer->drawTetMesh(NORMAL_NAME, mesh_data.points,
                         mesh_data.boundary_faces);

    // first render, reset camera
    _viewer->resetCamera();
  } else {
    _viewer->setVisibility(NORMAL_NAME, true);
  }

  _viewer->setRenderStyle(NORMAL_NAME, nRenderStyle);

  //_main_actor->setColor();
  _viewer->setVisibility(SHRINKED_NAME, false);
  shrinked = false;

  _viewer->refresh();
}

void MeshShell::readMesh(std::string filename) {
  mesh_wrapper->readMesh(filename);
  mesh_loaded = true;
}

void MeshShell::updateMeshRenderStyle(int nRenderStyle) {
  /*if (nRenderStyle == 0) {
    _viewer->removeActor(_main_actor->get_actor());
  } else {
    if (_main_actor->render_status.edge_on == false &&
        _main_actor->render_status.face_on == false)
      _viewer->renderActor(_main_actor->get_actor());
    _main_actor->setRenderSyle(nRenderStyle);
  }*/
  if (shrinked) {
    _viewer->setRenderStyle(SHRINKED_NAME, nRenderStyle);
  } else {
    _viewer->setRenderStyle(NORMAL_NAME, nRenderStyle);
  }

  _viewer->refresh();
}

void MeshShell::updateFaceOpacity(double opacity, int geometryStyle) {
  if (geometryStyle == 1) {
    _viewer->setOpacity(NORMAL_NAME, opacity);
  } else if (geometryStyle == 2) {
    _viewer->setOpacity(SHRINKED_NAME, opacity);
  }
}

void MeshShell::drawShrink(int nRenderStyle) {
  if (!_viewer->exist(SHRINKED_NAME)) {
    std::vector<Eigen::Vector3d> points;
    std::vector<Eigen::Matrix<long long, 3, 1>> faces;

    mesh_wrapper->getShrinkMesh(points, faces);

    _viewer->drawTetMesh(SHRINKED_NAME, points, faces);
  } else {
    _viewer->setVisibility(SHRINKED_NAME, true);
  }

  _viewer->setVisibility(NORMAL_NAME, false);
  // 1 : render edge 2 : render face 3 = 2 + 1
  _viewer->setRenderStyle(SHRINKED_NAME, nRenderStyle);
  //_shrink_actor->setColor();
  shrinked = true;

  _viewer->refresh();
}

bool MeshShell::readStressField(std::string filename) {
  bool flag = mesh_wrapper->readStressField(filename);
  mesh_wrapper->request_cell_centers(mesh_data.cell_centors);
  return flag;
}

void MeshShell::drawStressField(bool major, bool middle, bool minor) {
  // we get the data
  // std::vector<Eigen::Vector3d> mesh_data.cell_centors;
  std::vector<Eigen::Vector3d> major_v;
  std::vector<Eigen::Vector3d> middle_v;
  std::vector<Eigen::Vector3d> minor_v;
  std::vector<double> scalars;
  double major_c[] = {1.0, 0.0, 0.0};
  double middle_c[] = {0.0, 1.0, 0.0};
  double minor_c[] = {0.0, 0.0, 1.0};

  // get cell size
  double cell_size = mesh_wrapper->cellSize();
  // scale vectors' length
  double scaled_size = cell_size * 0.7;

  mesh_wrapper->get_principal_vectors(major_v, middle_v, minor_v);
  mesh_wrapper->request_von_mises(scalars);

  // major principal vector
  //_viewer->drawVector("major", mesh_data.cell_centors, major_v, scaled_size);
  _viewer->drawVectorWithScalars("major", mesh_data.cell_centors, major_v,
                                 scalars, scaled_size);
  //_viewer->setColor("major", major_c);

  // middle principal vector
  _viewer->drawVector("middle", mesh_data.cell_centors, middle_v, scaled_size);
  _viewer->setColor("middle", middle_c);

  // minor principal vector
  _viewer->drawVector("minor", mesh_data.cell_centors, minor_v, scaled_size);
  _viewer->setColor("minor", minor_c);

  // decide visibility
  _viewer->setVisibility("major", major);
  _viewer->setVisibility("middle", middle);
  _viewer->setVisibility("minor", minor);

  _viewer->refresh();
}

std::string MeshShell::stressSingularity(double tolerance, double point_size) {
  std::string name("singularity");
  std::vector<Eigen::Vector3d> singularites;
  mesh_wrapper->singularityLoaction(singularites, tolerance);
  size_t n = singularites.size();
  std::cout << "tolerance : " << tolerance << name + " : " << n << std::endl;

  _viewer->drawPoints(name, singularites, point_size);

  _viewer->refresh();
  return name;
}

void MeshShell::singularitySizeChange(int pointSize) {}

std::string MeshShell::splitFaces(double tolerance) {
  std::string name("splited_faces");
  std::vector<int> split_face_ids;
  std::vector<meshtools::FaceVertices<3>> split_faces;
  mesh_wrapper->divideCells(split_face_ids, tolerance);
  mesh_data.getFaceData(split_face_ids, split_faces);

  _viewer->drawTetMesh(name, mesh_data.points, split_faces);
  double color[] = {0.5450980392156862, 0.4588235294117647, 0.0};
  _viewer->setColor(name, color);

  _viewer->refresh();

  return name;
}

std::string MeshShell::extractSingularLines() {
  std::string name("singular_edges");
  std::vector<Eigen::Matrix<long long, 2, 1>> vertices_pairs;
  mesh_wrapper->get_singular_edges(vertices_pairs);
  _viewer->drawSegments(name, mesh_data.points, vertices_pairs);
  return name;
}

std::string MeshShell::smoothStressField(int index) {
  std::string name("smoothed_field");
  std::vector<double> von_mises;
  double w_consistance = 1.0, w_smooth = 0.5;

  // get cell size
  double cell_size = mesh_wrapper->cellSize();

  // get von mises stress
  mesh_wrapper->request_von_mises(von_mises);

  auto smoothed_field =
      mesh_wrapper->request_smoothed_stress_field(w_consistance, w_smooth);

  // rendering
  _viewer->drawVectorWithScalars(name, mesh_data.cell_centors, smoothed_field,
                                 von_mises, cell_size);

  return name;
}

void MeshShell::test() {
  std::vector<int> loop;
  int u = 5250;
  int index = mesh_wrapper->get_cell_loop(u, -1, loop);
  std::cout << "test : matching index " << index << std::endl;
  std::vector<std::vector<Eigen::Vector3d>> segs;
  segs.push_back(mesh_wrapper->getCellSegmentData(loop));
  _viewer->drawLines("loop", segs, true);
}
