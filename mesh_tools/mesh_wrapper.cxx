/********************************/
// @author : lyz
// @content: implement of mesh wrapper
// @birth : 2019-09-08
/********************************/
#include "mesh_wrapper.h"
#include "Implementation/mesh_implement.h"
#include "Implementation/stress_field.h"
#include "dlib/optimization.h"

namespace meshtools {
MeshWrapper::MeshWrapper()
    : impl(new MeshImpl), field(new PrincipalStressField) {}

/**
 *@delete impl,field
 */
MeshWrapper::~MeshWrapper() {
  delete impl;
  delete field;
}

void MeshWrapper::readMesh(std::string filename) { impl->readMesh(filename); }

bool MeshWrapper::readStressField(std::string filename) {
  // old way
  /* if (impl->mesh_loaded) {
     if (!field->readInStress(filename, impl->ovm_mesh))
       return false;
   } else {
     if (!field->readInStress(filename))
       return false;
   }*/

  // new way, read processed data

  field->readProcessedStress(filename, impl->ovm_mesh);

  std::cout << "field : " << field->cell_tensors_.size() << std::endl;

  // construct matching graph
  impl->construct_matching_graph(field->cell_tensors_);

  saveElementTensors("element.csv");

  return true;
}

bool MeshWrapper::saveElementTensors(std::string filename) {
  std::ofstream fout(filename);

  fout << "# element " << field->cell_tensors_.size() << std::endl;

  for (auto s : field->cell_tensors_) {
    fout << s.matrix_(0, 0) << ' ' << s.matrix_(1, 1) << ' ' << s.matrix_(2, 2)
         << ' ' << s.matrix_(0, 1) << ' ' << s.matrix_(1, 2) << ' '
         << s.matrix_(0, 2) << std::endl;
  }

  return true;
}

void MeshWrapper::get_principal_vectors(std::vector<V3d> &loc,
                                        std::vector<V3d> &major,
                                        std::vector<V3d> &middle,
                                        std::vector<V3d> &minor) {
  // field->get_locations(loc);
  request_cell_centers(loc);
  field->get_principal_dirs(major, 0);
  field->get_principal_dirs(middle, 1);
  field->get_principal_dirs(minor, 2);
}

void MeshWrapper::singularityLoaction(std::vector<V3d> &loc, double tolerance) {
  field->singularityLoaction(loc, tolerance);
}

void MeshWrapper::divideCells(std::vector<int> &split_face_ids,
                              double tolerance) {
  impl->divideCells(field->cell_tensors_, split_face_ids, tolerance);
}

void MeshWrapper::request_cell_centers(std::vector<V3d> &retrieve_val) {
  if (impl->request_cell_centers()) {
    retrieve_val.reserve(impl->ovm_mesh->n_cells());
    for (auto c : impl->cell_centers) {
      retrieve_val.push_back(c);
    }
  }
}

std::vector<V3d> &MeshWrapper::request_cell_centers() {
  return impl->cell_centers;
}

void MeshWrapper::request_von_mises(std::vector<double> &von_mises) {
  field->get_von_mises(von_mises);
}

std::vector<V3d>
MeshWrapper::request_smoothed_stress_field(double w_consistance,
                                           double w_smooth) {
  std::vector<V3d> smoothed_vectors;
  field->get_principal_dirs(smoothed_vectors, 0);

  VectorField optimizer(impl->ovm_mesh, smoothed_vectors);

  return optimizer.smooth_vector_field(w_consistance, w_smooth);
}

void MeshWrapper::test() {
  // test matching graph

  impl->construct_matching_graph(field->cell_tensors_);

  std::vector<int> loop;
  OvmHaEgH he = impl->ovm_mesh->halfedge((OvmVeH)0, (OvmVeH)6);
  impl->get_cell_loop(he, loop);
}

void MeshWrapper::saveToOVM(std::string filename) { impl->saveToOVM(filename); }

std::vector<std::string> MeshWrapper::separateFilename(std::string filename) {
  return impl->separateFilename(filename);
}

void MeshWrapper::getFaceData(std::vector<V3d> &points,
                              std::vector<FaceVertices<3>> &faces) {
  impl->getFaceData(points, faces);
}

void MeshWrapper::getCellSegmentData(std::vector<int> &cell_ids,
                                     std::vector<V3d> &points) {
  points.reserve(cell_ids.size());
  for (auto i : cell_ids) {
    points.push_back(V3d(impl->cell_centers[i]));
  }
}

std::vector<V3d> MeshWrapper::getCellSegmentData(std::vector<int> &cell_ids) {
  std::vector<V3d> points;
  points.reserve(cell_ids.size());
  for (auto i : cell_ids) {
    points.push_back(V3d(impl->cell_centers[i]));
  }
  return points;
}

std::vector<V3d> MeshWrapper::getEdgeData(std::vector<int> &edge_ids) {
  std::vector<V3d> pairs;
  pairs.reserve(edge_ids.size() * 2);
  for (int i : edge_ids) {
    OvmEgH eh(i);
    OvmVeH u = impl->ovm_mesh->edge(eh).to_vertex();
    OvmVeH v = impl->ovm_mesh->edge(eh).from_vertex();
    pairs.push_back(V3d(impl->ovm_mesh->vertex(u).data()));
    pairs.push_back(V3d(impl->ovm_mesh->vertex(v).data()));
  }
  return pairs;
}

void MeshWrapper::getBoundaryFaceIds(std::vector<int> &faceids_list) {
  impl->getBoundaryFaceIds(faceids_list);
}

void MeshWrapper::getShrinkMesh(
    std::vector<V3d> &points,
    std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  impl->getShrinkMesh(points, faces);
}

std::string MeshWrapper::get_mesh_name() { return impl->mesh_name; }

double MeshWrapper::cellSize() { return impl->cellSize(); }

int MeshWrapper::get_matching_index(int edge_idx) { return 0; }

bool MeshWrapper::get_singular_edges(std::vector<V3d> &edge_points) {
  std::vector<OvmEgH> edge_ids;
  for (auto eh : impl->ovm_mesh->edges()) {
    int index =
        impl->get_edge_matching_index(impl->ovm_mesh->halfedge_handle(eh, 0));
    if (index != 0) {
      edge_ids.push_back(eh);
    }
  }
  impl->get_edge_data(edge_ids, edge_points);
  return true;
}

bool MeshWrapper::get_singular_edges(std::vector<FaceVertices<2>> &vertex_ids) {
  vertex_ids.clear();
  for (auto eh : impl->ovm_mesh->edges()) {
    // we don't consider boundary edges now
    if (impl->ovm_mesh->is_boundary(eh))
      continue;
    int index =
        impl->get_edge_matching_index(impl->ovm_mesh->halfedge_handle(eh, 0));
    if (index != 0) {
      OvmVeH u = impl->ovm_mesh->edge(eh).to_vertex();
      OvmVeH v = impl->ovm_mesh->edge(eh).from_vertex();
      vertex_ids.push_back(FaceVertices<2>(u.idx(), v.idx()));
    }
  }
  return true;
}

std::string MeshWrapper::meshInfo() {
  std::string info = "";

  std::stringstream ss;

  ss << "vertices : ";
  ss << impl->ovm_mesh->n_vertices();
  ss << '\n';
  ss << "edges : ";
  ss << impl->ovm_mesh->n_edges();
  ss << '\n';
  ss << "faces : ";
  ss << impl->ovm_mesh->n_faces();
  ss << '\n';
  ss << "cells : ";
  ss << impl->ovm_mesh->n_cells();
  ss << '\n';
  ss >> info;

  return ss.str();
}

int MeshWrapper::get_cell_loop(int from_v_id, int to_v_id,
                               std::vector<int> &cell_loop) {
  OvmVeH to_v_handle;
  if (to_v_id == -1) {
    // to_v_id == -1, then we choose one from from_v's adjacent halfedge
    to_v_handle = *(impl->ovm_mesh->vv_iter((OvmVeH)from_v_id));
  } else {
    to_v_handle = (OvmVeH)to_v_id;
  }
  OvmHaEgH he = impl->ovm_mesh->halfedge((OvmVeH)from_v_id, to_v_handle);
  return impl->get_cell_loop(he, cell_loop);
}

} // namespace meshtools