#include "view_wrapper.h"
#include <fstream>

namespace viewtools {
/*********** Color begin **************/
Color::Color(double *rgb) : Triple(rgb) {}

Color::Color(double r, double g, double b) : Triple(r, g, b) {}

Color::Color(std::string description) : Triple() {
  if (description == "red") {
    _data[0] = 1;
    _data[1] = 0;
    _data[2] = 0;
  } else if (description == "green") {
    _data[0] = 0;
    _data[1] = 1;
    _data[2] = 0;
  } else if (description == "blue") {
    _data[0] = 0;
    _data[1] = 0;
    _data[2] = 1;
  } else {
    _data[0] = 1;
    _data[1] = 1;
    _data[2] = 1;
  }
}

/*********** Color end **************/

/************************* ActorControler begin *************************/

ActorControler::ActorControler(std::string name,
                               vtkSmartPointer<vtkActor> actor)
    : name(name), _actor(actor) {}

ActorControler::~ActorControler() {}

void ActorControler::setVisibility(bool visibility) {
  _actor->SetVisibility(visibility);
}

/** 
	surface type : points(0), wireframe(1) or surface(2)
*/
void ActorControler::setRenderSyle(int nRenderStyle) {
		_actor->GetProperty()->SetRepresentation(nRenderStyle);
}

void ActorControler::setColor(Color color) {
  _actor->GetProperty()->SetColor(color.data());
}

void ActorControler::setSize(double size) {
	_actor->GetProperty()->SetLineWidth(size);
}

vtkSmartPointer<vtkActor> ActorControler::get_actor() { return _actor; }

void ActorControler::setOpacity(double opacity) {
	_actor->GetProperty()->SetOpacity(opacity);
}


/************************* ActorControler  end  *************************/

/************************* MeshActorControler  begein  *************************/

MeshActorControler::MeshActorControler(std::string name,
                                       vtkSmartPointer<vtkActor> actor): ActorControler(name,actor) {}

void MeshActorControler::setRenderSyle(int nRenderStyle) {
  if (nRenderStyle & 2) {
    _actor->VisibilityOn();
    // surface type : points(0), wireframe(1) or surface(2)
    _actor->GetProperty()->SetRepresentation(2);
    _actor->GetProperty()->SetColor(render_status.face_color.data());
    render_status.face_on = true;

    // also render edges
    if (nRenderStyle & 1) {
      _actor->GetProperty()->SetEdgeVisibility(true);
      _actor->GetProperty()->SetEdgeColor(render_status.edge_color.data());
      render_status.edge_on = true;
    } else {
      _actor->GetProperty()->SetEdgeVisibility(false);
      render_status.edge_on = false;
    }
  } else if (nRenderStyle & 1) // only render edges
  {
    _actor->VisibilityOn();
    _actor->GetProperty()->SetRepresentationToWireframe();
    _actor->GetProperty()->SetColor(render_status.edge_color.data());
    render_status.face_on = false;
    render_status.edge_on = true;
  } else {
    _actor->VisibilityOff();
    render_status.face_on = false;
    render_status.edge_on = false;
  }
}

/************************* MeshActorControler   end  *************************/

/************************* MeshActorControler  begein  *************************/

PointsActorControler::PointsActorControler(std::string name,
                               vtkSmartPointer<vtkActor> actor)
    : ActorControler(name,actor) {}

void PointsActorControler::setSize(double size) {
  render_status.point_size = size;
  _actor->GetProperty()->SetPointSize(size);
}

/************************* MeshActorControler   end  *************************/


/************************* ActorTable  end  *************************/

void ActorTable::insert(ActorControler *u) {
  auto t = _table.find(u->name);
  // if u's name exits, then replace the pointer
  if(t != _table.end()) {
    delete t->second;
    t->second = u;
  } else
	  _table.insert(std::make_pair(u->name,u));
}

void ActorTable::remove(const std::string key) {
	auto u = _table.find(key);
	if (u != _table.end()) {
		if (u->second != nullptr)
			delete u->second;
    _table.erase(u);
	}
}

void ActorTable::remove(std::map<std::string, ActorControler *>::iterator &id) {
	delete id->second;
	_table.erase(id);
}

std::map<std::string, ActorControler *>::iterator ActorTable::find(std::string key) {
	return _table.find(key);
}

std::map<std::string, ActorControler *>::iterator ActorTable::begin() {
  return _table.begin();
}

std::map<std::string, ActorControler *>::iterator ActorTable::end() {
  return _table.end();
}

/************************* ActorTable  end  *************************/

VtkWrapper::VtkWrapper(QVTKOpenGLWidget *Qwidget) {
  // Create the usual rendering stuff

  _renderer = vtkRenderer::New();

  _renderWindow = vtkGenericOpenGLRenderWindow::New();
  _renderWindow->AddRenderer(_renderer);

  _vtkWidget = Qwidget;
  _vtkWidget->setRenderWindow(_renderWindow);

  //_renderWindow = (vtkGenericOpenGLRenderWindow)_vtkWidget->renderWindow();

  /** interaction style

  */

  vtkNew<vtkInteractorStyleSwitch> style_switch;
  vtkNew<vtkInteractorStyleTrackballCamera> style_trackball_camera;
  _renderWindow->GetInteractor()->SetInteractorStyle(style_trackball_camera);

  /** Axes
   * add a Orthogonal coordinate to the bottom left of the screen
   */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

  _markerWidget = vtkOrientationMarkerWidget::New();

  _markerWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  _markerWidget->SetOrientationMarker(axes);
  _markerWidget->SetInteractor(_renderWindow->GetInteractor());
  _markerWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  _markerWidget->SetEnabled(1);
  _markerWidget->InteractiveOn();
}

VtkWrapper::~VtkWrapper() {}

int VtkWrapper::renderActor(vtkActor *actor) {
  // Add Actor to renderer
  _renderer->AddActor(actor);
  //_renderer->ResetCamera();
  _renderWindow->Render();
  //_vtkWidget->show();

  return _renderer->GetActors()->GetNumberOfItems();
}

int VtkWrapper::removeActor(vtkActor *actor) {
  _renderer->RemoveActor(actor);
  _renderWindow->Render();
  return _renderer->GetActors()->GetNumberOfItems();
}

int VtkWrapper::refresh() {
  _renderWindow->Render();
  return 0;
}

void VtkWrapper::resetCamera() {
	_renderer->ResetCamera();
}

vtkSmartPointer<vtkActor>
VtkWrapper::processHedgehog(const std::vector<Eigen::Vector3d> &points,
                            const std::vector<Eigen::Vector3d> &vectors,
                            double scale_factor, double line_width) {
  size_t n = points.size();
  vtkNew<vtkPoints> locs;
  locs->Allocate(n);
  vtkNew<vtkDoubleArray> dirs;
  dirs->SetNumberOfComponents(3);
  dirs->SetNumberOfTuples(n);

  double x[3] = {0, 0, 0};
  double v[3] = {1, 0, 0};
  for (int i = 0; i < n; ++i) {
    locs->InsertPoint(i, points[i].data());
    dirs->InsertTuple(i, vectors[i].data());
  }

  vtkNew<vtkPolyData> ps;
  ps->SetPoints(locs);
  ps->GetPointData()->SetVectors(dirs);

  vtkNew<vtkHedgeHog> hedgehog;
  hedgehog->SetInputData(ps);
  hedgehog->SetScaleFactor(scale_factor);

  // Mapper
  vtkNew<vtkPolyDataMapper> mapper;
  // mapper->SetInputConnection(input);
  mapper->SetInputConnection(hedgehog->GetOutputPort());

  // Actor in scene _actor->SetMapper(mapper)
  vtkNew<vtkActor> actor;

  // VTK Renderer
  // vtkNew<vtkRenderer> ren;

  // Actor in scene
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(line_width);

  //actor->GetProperty()->SetColor(color.data());

  return actor;
}

void VtkWrapper::setVertexScalars(std::vector<double> &scalars,
                                  double lower_bound, double upper_bound,
                                  vtkSmartPointer<vtkActor> actor) {
  auto mapper = actor->GetMapper();
  auto data = mapper->GetInput();
  vtkNew<vtkDoubleArray> darray;
  darray->Allocate(scalars.size());
  for (auto d : scalars) {
    darray->InsertNextValue(d);
  }
  darray->SetName("scalars");
  data->GetPointData()->SetScalars(darray);
  // data->GetPointData()->SetActiveScalars("scalars");

  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(lower_bound, upper_bound);

  refresh();
}

vtkSmartPointer<vtkActor>
VtkWrapper::processPoints(std::vector<Eigen::Vector3d> &points) {
  //std::vector<vtkFacetTuple<1>> polys;
	std::vector<VertexList<1>> polys;
  polys.resize(points.size());
  for (int i = 0; i < points.size(); ++i) {
    polys[i] = VertexList<1>(i);
  }
  auto data = processPolyData<1>(points, polys);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(data);
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  return actor;
}

// not completed
bool VtkWrapper::readJsonSettings() {
  std::ifstream fin(_settings_jsonfile);
  return false;

  // fin.close(); RAII
}

void VtkWrapper::testRenderFunction() {}

} // namespace viewtools