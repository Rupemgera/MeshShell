#include "mesh_shell.h"

MeshShell::MeshShell(VtkWrapper *viewer) { _viewer = viewer; }

void MeshShell::drawMesh() {
  std::vector<viewtools::Point3d> points;
  std::vector<viewtools::Triangle> faces;

  if (mesh_loaded == false) {
    std::cout << "mesh not loaded" << std::endl;
    return;
  }
  int nv = ovm_mesh->n_vertices();
  points.resize(nv);
  int nf = ovm_mesh->n_faces();
  faces.resize(nf);

  // get points data
  for (auto viter : ovm_mesh->vertices()) {
    auto p = ovm_mesh->vertex(viter);
    points[viter.idx()] = p.data();
  }

  // get faces data
  for (auto fiter : ovm_mesh->faces()) {
    long long v[3];
    int k = 0;
    for (auto j = ovm_mesh->fv_iter(fiter); j.valid(); ++j) {
      v[k] = j->idx();
      k++;
    }
    faces[fiter.idx()] = v;
  }

  auto actor = _viewer->processMesh(points, faces);
  auto map_item = map_actors.find(mesh_name);
  if (map_item != map_actors.end()) {
    map_item->second = actor;
  } else {
    map_actors.insert(ActorMap::value_type(mesh_name, actor));
  }

  _viewer->renderActor(actor);
}