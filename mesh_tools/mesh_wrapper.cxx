#include "mesh_wrapper.h"
#include "mesh_implement.h"


namespace meshtools {
MeshWrapper::MeshWrapper() : impl(new MeshImpl) {}

MeshWrapper::~MeshWrapper() { delete impl; }

void MeshWrapper::readMesh(std::string filename) { impl->readMesh(filename); }

void MeshWrapper::saveToOVM(std::string filename) { impl->saveToOVM(filename); }

std::vector<std::string> MeshWrapper::separateFilename(std::string filename) {
  return impl->separateFilename(filename);
}

void MeshWrapper::get_face_data(
    std::vector<Eigen::Vector3d>& points,
                           std::vector<Eigen::Matrix<long long, 3, 1>>& faces) {
  impl->get_face_data(points, faces);
}

std::string MeshWrapper::get_mesh_name() { return impl->mesh_name; }

// void MeshImpl::faceData(VMeshPtr mesh, std::vector<double[3]> &points,
// std::vector<int[3]> &faces)
// {
// 	int nv = mesh->n_vertices();
// 	points.resize(nv);
// 	int nf = mesh->n_faces();
// 	faces.resize(nf);

// 	// get points data
// 	for (auto viter : mesh->vertices())
// 	{
// 		auto p = mesh->vertex(viter);
// 		double v3[3] = {p[0], p[1], p[2]};
// 		points.push_back(v3);
// 	}

// 	// get faces data
// 	for (auto fiter : mesh->faces())
// 	{
// 		int v[3];
// 		int k = 0;
// 		for (auto j = mesh->fv_iter(fiter); j.valid(); ++j)
// 		{
// 			v[k] = j->idx();
// 			k++;
// 		}
// 		faces.push_back(v);
// 	}
// }

}  // namespace meshtools