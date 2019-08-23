#include "mesh_shell.h"

MeshShell::MeshShell(VtkWrapper *viewer) { _viewer = viewer; }

void MeshShell::drawMesh() {
  std::vector<Eigen::Vector3d> points;
  std::vector<Eigen::Matrix<long long, 3, 1>> faces;

  MeshWrapper::get_face_data(points, faces);

  std::vector<viewtools::Point3d> vtk_points;
  vtk_points.reserve(points.size());
  for (auto i : points) {
    vtk_points.push_back(i.data());
  }
  std::vector<viewtools::Triangle> vtk_faces;
  vtk_faces.reserve(faces.size());
  for (auto j : faces) {
    vtk_faces.push_back(j.data());
  }

  auto actor = _viewer->processMesh(vtk_points, vtk_faces);
  auto mesh_name = MeshWrapper::get_mesh_name();
  auto map_item = map_actors.find(mesh_name);
  if (map_item != map_actors.end()) {
    map_item->second = actor;
  } else {
    map_actors.insert(ActorMap::value_type(mesh_name, actor));
  }

  _viewer->renderActor(actor);
}