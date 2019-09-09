/********************************/
// @author : lyz
// @content: implement of mesh wrapper
// @birth : 2019-09-08
/********************************/
#include "mesh_wrapper.h"
#include "Implementation/mesh_implement.h"
#include "Implementation/stress_field.h"

namespace meshtools {
MeshWrapper::MeshWrapper()
    : impl(new MeshImpl), field(new PrincipalStressField) {}

MeshWrapper::~MeshWrapper() { delete impl; }

void MeshWrapper::readMesh(std::string filename) { impl->readMesh(filename); }

void MeshWrapper::readStressField(std::string filename) {
  // impl->readStressField(filename);
	if(impl->mesh_loaded){
		field->readInStress(filename,impl->ovm_mesh);
	}else{
		field->readInStress(filename);
  }
	std::cout<<"field : "<<field->tensors.size()<<std::endl;
}

void MeshWrapper::get_principal_vectors(std::vector<Eigen::Vector3d> &loc,
                                        std::vector<Eigen::Vector3d> &major,
                                        std::vector<Eigen::Vector3d> &middle,
                                        std::vector<Eigen::Vector3d> &minor) {
	field->get_locations(loc);
  field->get_principal_dirs(major,0);
	field->get_principal_dirs(middle,1);
	field->get_principal_dirs(minor,2);
}

void MeshWrapper::singularityLoaction(std::vector<Eigen::Vector3d> &loc,
                                      double tolerance) {
  field->singularityLoaction(loc, tolerance);
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