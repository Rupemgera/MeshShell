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

void ActorControler::setVisibility(bool visibility) {
  _actor->SetVisibility(visibility);
}

void ActorControler::setRenderSyle(int nRenderStyle) {
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

void ActorControler::setColor(Color edge_color, Color face_color) {
  render_status.face_color = face_color.data();
  render_status.edge_color = edge_color.data();
}
void ActorControler::setPointSize(float size) {
  render_status.point_size = size;
  _actor->GetProperty()->SetPointSize(size);
}
vtkSmartPointer<vtkActor> ActorControler::get_actor() { return _actor; }

/************************* ActorControler  end  *************************/

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

int VtkWrapper::renderActor(vtkActor *actor) {
  // Add Actor to renderer
  _renderer->AddActor(actor);
  _renderer->ResetCamera();
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

vtkSmartPointer<vtkActor>
VtkWrapper::processHedgehog(const std::vector<Eigen::Vector3d> &points,
                            const std::vector<Eigen::Vector3d> &vectors, Color color,
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

  actor->GetProperty()->SetColor(color.data());

  return actor;
}

vtkSmartPointer<vtkActor>
VtkWrapper::processMesh(const std::vector<Eigen::Vector3d> &points,
                        const std::vector<Triangle> &faces) {
  ///* insert vertices */
  // vtkNew<vtkPoints> nodes;
  // size_t n_vertices = points.size();
  // nodes->GetData()->Allocate(n_vertices);
  // for (int i = 0; i < n_vertices; ++i) {
  //  nodes->InsertPoint(i, points[i].data());
  //}

  ///* insert faces */
  // vtkNew<vtkCellArray> triangles;
  // size_t n_faces = faces.size();
  ////每个单元有4个数据 1个表示点的数量，3个表示顶点的标号
  // triangles->GetData()->Allocate((1 + 3) * n_faces);
  // for (auto i = 0; i < n_faces; ++i) {
  //  triangles->InsertNextCell(3, faces[i].data());
  //}

  ///* form mesh data */

  // vtkNew<vtkPolyData> mesh_data;
  // mesh_data->SetPoints(nodes);
  // mesh_data->SetPolys(triangles);

  /// use processPolyData ///

  auto mesh_data = processPolyData<3>(points, faces);

  /***** mapper *****/

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(mesh_data);

  /********** actors *************/

  vtkNew<vtkActor> face_actor;
  face_actor->SetMapper(mapper);

  return face_actor;
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
  std::vector<vtkFacetTuple<1>> polys;
  polys.resize(points.size());
  for (int i = 0; i < points.size(); ++i) {
    polys[i] = {i};
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