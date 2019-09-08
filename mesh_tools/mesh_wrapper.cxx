/********************************/
// @author : lyz
// @content: implement of mesh wrapper
// @birth : 2019-09-08
/********************************/
#include "mesh_wrapper.h"
#include "Implementation/mesh_implement.h"

namespace meshtools {
MeshWrapper::MeshWrapper() : impl(new MeshImpl) {}

MeshWrapper::~MeshWrapper() { delete impl; }

void MeshWrapper::readMesh(std::string filename) { impl->readMesh(filename); }

void MeshWrapper::readStressField(std::string filename) {
  impl->readStressField(filename);
}

void MeshWrapper::saveToOVM(std::string filename) { impl->saveToOVM(filename); }

std::vector<std::string> MeshWrapper::separateFilename(std::string filename) {
  return impl->separateFilename(filename);
}

void MeshWrapper::getFaceData(
    std::vector<Eigen::Vector3d> &points,
    std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  impl->getFaceData(points, faces);
}

void MeshWrapper::getShrinkMesh(
    std::vector<Eigen::Vector3d> &points,
    std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  impl->getShrinkMesh(points, faces);
}

std::string MeshWrapper::get_mesh_name() { return impl->mesh_name; }

size_t MeshWrapper::n_vertices() { return impl->ovm_mesh->n_vertices(); }

size_t MeshWrapper::n_edges() { return impl->ovm_mesh->n_edges(); }

size_t MeshWrapper::n_faces() { return impl->ovm_mesh->n_faces(); }

size_t MeshWrapper::n_cells() { return impl->ovm_mesh->n_cells(); }

} // namespace meshtools