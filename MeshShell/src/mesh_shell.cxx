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
  for (auto u : _actors_table) {
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

    _main_actor = new MeshActorControler(
        "main_actor", _viewer->processMesh(points, vtk_faces));
    mesh_name = ovm_mesh->get_mesh_name();
    /*auto map_item = _actors_table.find(mesh_name);
    if (map_item != _actors_table.end()) {
            map_item->second = actor;
    } else {
            _actors_table.insert(ActorMap::value_type(mesh_name, actor));
            }*/


    _viewer->renderActor(_main_actor->get_actor());
		// first render, reset camera
		_viewer->resetCamera();
  }

  if (_shrink_actor != nullptr)
    _shrink_actor->setVisibility(false);

  // 1 : render edge 2 : render face 3 = 2 + 1
  _main_actor->setRenderSyle(nRenderStyle);
  //_main_actor->setColor();
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

void MeshShell::updateFaceOpacity(double opacity, int geometryStyle) {
	if (geometryStyle == 1){
		_main_actor->setOpacity(opacity);
	}else if(geometryStyle == 2){
		_shrink_actor->setOpacity(opacity);
	}
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

    _shrink_actor = new MeshActorControler(
        "strink_actor", _viewer->processMesh(points, vtk_faces));

    _viewer->renderActor(_shrink_actor->get_actor());
  }

  if (_main_actor != nullptr)
    _main_actor->setVisibility(false);

  // 1 : render edge 2 : render face 3 = 2 + 1
  _shrink_actor->setRenderSyle(nRenderStyle);
  //_shrink_actor->setColor();
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
  
	ActorControler *major_ptr;
	ActorControler *middle_ptr;
	ActorControler *minor_ptr;

	// if we have got data
  if (_actors_table.find("major_principals") != _actors_table.end()) {
		major_ptr = _actors_table.find("major_principals")->second;
		middle_ptr = _actors_table.find("middle_principals")->second;
		minor_ptr = _actors_table.find("minor_principals")->second;
  } else {
    // we get the data
    std::vector<Eigen::Vector3d> loc;
    std::vector<Eigen::Vector3d> major_v;
    std::vector<Eigen::Vector3d> middle_v;
    std::vector<Eigen::Vector3d> minor_v;

		// major principal vector
    ovm_mesh->get_principal_vectors(loc, major_v, middle_v, minor_v);
		major_ptr = new ActorControler("major_principals",_viewer->processHedgehog(loc,major_v));
		major_ptr->setColor(viewtools::Color(1.0,0.0,0.0));
		_actors_table.insert(major_ptr);

		// middle principal vector
		middle_ptr = new ActorControler("middle_principals",_viewer->processHedgehog(loc,middle_v));
		_actors_table.insert(middle_ptr);
		middle_ptr->setColor(viewtools::Color(0.0,1.0,0.0));

		// minor principal vector
		minor_ptr = new ActorControler("minor_principals",_viewer->processHedgehog(loc,minor_v));
		_actors_table.insert(minor_ptr);
		minor_ptr->setColor(viewtools::Color(0.0,0.0,1.0));

		// register actor
		_viewer->renderActor(major_ptr->get_actor());
		_viewer->renderActor(middle_ptr->get_actor());
		_viewer->renderActor(minor_ptr->get_actor());
  }
	
	// decide visibility
	major_ptr->setVisibility(major);
	middle_ptr->setVisibility(middle);
	minor_ptr->setVisibility(minor);

	_viewer->refresh();
}

void MeshShell::stressSingularity(double tolerance) {
	ActorControler *si_view;
  std::vector<Eigen::Vector3d> singularites;
  ovm_mesh->singularityLoaction(singularites, tolerance);
  size_t n = singularites.size();
	std::cout<<"tolerance : "<<tolerance<<" singularities : "<<n<<std::endl;

	// singularity render already exists
	// then delete it
	auto target = _actors_table.find("singularity");
	if (target != _actors_table.end()) {
		si_view = target->second;
		_viewer->removeActor(si_view->get_actor());
		// pointer to ActorControl deleted in remove()
		_actors_table.remove(target);
	}

	// render new one
	
  si_view =
      new PointsActorControler("singularity", _viewer->processPoints(singularites));
  si_view->setSize(15.0);
  si_view->setColor(viewtools::Color(1, 0, 0));
  _actors_table.insert(si_view);
  _viewer->renderActor(si_view->get_actor());
  _viewer->refresh();
}
