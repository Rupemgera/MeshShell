#include "mesh_shell.h"
#include <iostream>

#define NORMAL_NAME "normal"
#define SHRINKED_NAME "shrinked"

TetMeshData::TetMeshData() {
  points.clear();
  faces.clear();
  boundary_faces.clear();
}

void TetMeshData::getFaceData(std::vector<int> &face_ids,
                              std::vector<meshtools::FaceList<3>> &ext) {
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
  if (!_viewer->exit(NORMAL_NAME)) {
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
  if (!_viewer->exit(SHRINKED_NAME)) {
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

// void MeshShell::setVertexScalars(std::vector<double> &scalars,
//                                 double lower_bound, double upper_bound) {
//  _viewer->setVertexScalars(scalars, lower_bound, upper_bound,
//                            _main_actor->get_actor());
//}

// void MeshShell::renderScalars(vtkSmartPointer<vtkActor> actor, bool flag) {
//  auto mapper = actor->GetMapper();
//  mapper->SetScalarVisibility(flag);
//}

void MeshShell::setVisibility(std::string name, bool visi) {
  _viewer->setVisibility(name, visi);
}

void MeshShell::setVertexScalars(std::string name, std::vector<double> &scalars,
                                 double lower_bound, double upper_bound) {
  _viewer->setVertexScalars(name, scalars, upper_bound, lower_bound);
}

void MeshShell::readStressField(std::string filename) {
  mesh_wrapper->readStressField(filename);
}

void MeshShell::drawStressField(bool major, bool middle, bool minor) {
  // we get the data
  std::vector<Eigen::Vector3d> loc;
  std::vector<Eigen::Vector3d> major_v;
  std::vector<Eigen::Vector3d> middle_v;
  std::vector<Eigen::Vector3d> minor_v;
  double major_c[] = {1.0, 0.0, 0.0};
  double middle_c[] = {0.0, 1.0, 0.0};
  double minor_c[] = {0.0, 0.0, 1.0};

  // get cell size
  double cell_size = mesh_wrapper->cellSize();

  mesh_wrapper->get_principal_vectors(loc, major_v, middle_v, minor_v);

  // major principal vector
  _viewer->drawVector("major", loc, major_v, cell_size);
  _viewer->setColor("major", major_c);

  // middle principal vector
  _viewer->drawVector("middle", loc, middle_v, cell_size);
  _viewer->setColor("middle", middle_c);

  // minor principal vector
  _viewer->drawVector("minor", loc, minor_v, cell_size);
  _viewer->setColor("minor", minor_c);

  // decide visibility
  _viewer->setVisibility("major", major);
  _viewer->setVisibility("middle", middle);
  _viewer->setVisibility("minor", minor);

  _viewer->refresh();
}

void MeshShell::stressSingularity(double tolerance, double point_size) {
  std::vector<Eigen::Vector3d> singularites;
  mesh_wrapper->singularityLoaction(singularites, tolerance);
  size_t n = singularites.size();
  std::cout << "tolerance : " << tolerance << " singularities : " << n
            << std::endl;

  _viewer->drawPoints("Singularity", singularites, point_size);

  _viewer->refresh();
}

void MeshShell::singularitySizeChange(int pointSize) {}

void MeshShell::divideCells(double tolerance) {
  std::vector<int> split_face_ids;
  std::vector<meshtools::FaceList<3>> split_faces;
  mesh_wrapper->divideCells(split_face_ids, tolerance);
  mesh_data.getFaceData(split_face_ids, split_faces);

  _viewer->drawTetMesh("splited_faces", mesh_data.points, split_faces);
  double color[] = {0.5450980392156862, 0.4588235294117647, 0.0};
  _viewer->setColor("splited_faces", color);

  _viewer->refresh();
}

void MeshShell::test() {
  std::vector<int> loop;
  int u = 5250;
  int index = mesh_wrapper->find_cell_loop(u, -1, loop);
  std::cout << "test : matching index " << index << std::endl;
  std::vector<std::vector<Eigen::Vector3d>> segs;
  segs.push_back(mesh_wrapper->getSegmentData(loop));
  _viewer->drawLines("loop", segs, true);
}
