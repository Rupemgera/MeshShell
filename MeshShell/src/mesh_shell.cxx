#include "mesh_shell.h"

MeshShell::MeshShell(VtkWrapper *viewer) : _viewer(viewer) {
  ovm_mesh = new MeshWrapper();
}

MeshShell::~MeshShell() {
  delete ovm_mesh;
  if (_main_actor != nullptr) {
    delete _main_actor;
  }
  if (_shrink_actor != nullptr) {
    delete _shrink_actor;
  }
  for (auto u : map_actors) {
    delete u.second;
  }
}

void MeshShell::drawMesh(int nRenderStyle) {
  if (_main_actor == nullptr) {
    std::vector<Eigen::Vector3d> points;
    std::vector<Eigen::Matrix<long long, 3, 1>> faces;

    ovm_mesh->getFaceData(points, faces);

    //std::vector<viewtools::Point3d> vtk_points;
    //vtk_points.reserve(points.size());
    //for (auto i : points) {
    //  vtk_points.push_back(i.data());
    //}
    std::vector<viewtools::Triangle> vtk_faces;
    vtk_faces.reserve(faces.size());
    for (auto j : faces) {
      vtk_faces.push_back(j.data());
    }

    _main_actor = new ActorControler(
        "main_actor", _viewer->processMesh(points, vtk_faces));
    mesh_name = ovm_mesh->get_mesh_name();
    /*auto map_item = map_actors.find(mesh_name);
    if (map_item != map_actors.end()) {
            map_item->second = actor;
    } else {
            map_actors.insert(ActorMap::value_type(mesh_name, actor));
            }*/
    _viewer->renderActor(_main_actor->get_actor());
  }

  if (_shrink_actor != nullptr)
    _shrink_actor->setVisibility(false);

  // 1 : render edge 2 : render face 3 = 2 + 1
  _main_actor->setRenderSyle(nRenderStyle);
  _main_actor->setColor();
  shrinked = false;

  _viewer->refresh();
}

void MeshShell::readMesh(std::string filename) {
  ovm_mesh->readMesh(filename);
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
    _shrink_actor->setRenderSyle(nRenderStyle);
  } else {
    _main_actor->setRenderSyle(nRenderStyle);
  }

  _viewer->refresh();
}

void MeshShell::drawShrink(int nRenderStyle) {
  if (_shrink_actor == nullptr) {
    std::vector<Eigen::Vector3d> points;
    std::vector<Eigen::Matrix<long long, 3, 1>> faces;

    ovm_mesh->getShrinkMesh(points, faces);

    /*std::vector<viewtools::Point3d> vtk_points;
    vtk_points.reserve(points.size());
    for (auto i : points) {
      vtk_points.push_back(i.data());
    }*/
    std::vector<viewtools::Triangle> vtk_faces;
    vtk_faces.reserve(faces.size());
    for (auto j : faces) {
      vtk_faces.push_back(j.data());
    }

    _shrink_actor = new ActorControler(
        "strink_actor", _viewer->processMesh(points, vtk_faces));

    _viewer->renderActor(_shrink_actor->get_actor());
  }

  if (_main_actor != nullptr)
    _main_actor->setVisibility(false);

  // 1 : render edge 2 : render face 3 = 2 + 1
  _shrink_actor->setRenderSyle(nRenderStyle);
  _shrink_actor->setColor();
  shrinked = true;

  _viewer->refresh();
}

void MeshShell::setVertexScalars(std::vector<double> &scalars,
                                 double lower_bound, double upper_bound) {
  _viewer->setVertexScalars(scalars, lower_bound, upper_bound,
                            _main_actor->get_actor());
}

void MeshShell::renderScalars(vtkSmartPointer<vtkActor> actor, bool flag) {
  auto mapper = actor->GetMapper();
  mapper->SetScalarVisibility(flag);
}

void MeshShell::readStressField(std::string filename) {
  ovm_mesh->readStressField(filename);
}

void MeshShell::drawStressField(bool major, bool middle, bool minor) {
  // if we have got data
	ActorControler *major_ptr;
  if (map_actors.find("major_principals") != map_actors.end()) {
		major_ptr = map_actors.find("major_principals")->second;
  } else {
    // we get the data
    std::vector<Eigen::Vector3d> loc;
    std::vector<Eigen::Vector3d> major_v;
    std::vector<Eigen::Vector3d> middle_v;
    std::vector<Eigen::Vector3d> minor_v;
    ovm_mesh->get_principal_vectors(loc, major_v, middle_v, minor_v);
		major_ptr = new ActorControler("major_principals",_viewer->processHedgehog(loc,major_v));
		insert_actor(major_ptr);
  }
	_viewer->renderActor(major_ptr->get_actor());
	_viewer->refresh();
}

void MeshShell::stressSingularity(double tolerance) {
  std::vector<Eigen::Vector3d> singularites;
  ovm_mesh->singularityLoaction(singularites, tolerance);
  size_t n = singularites.size();
  auto si_view =
      new ActorControler("singularity", _viewer->processPoints(singularites));
  si_view->setPointSize(14.0);
  si_view->setColor(viewtools::Color(1, 0, 0));
  insert_actor(si_view);
  _viewer->renderActor(si_view->get_actor());
  _viewer->refresh();
}

void MeshShell::insert_actor(ActorControler *a) {
	map_actors.insert(std::make_pair(a->name,a));
}
