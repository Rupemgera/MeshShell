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

VtkWrapper::VtkWrapper(QVTKOpenGLWidget *Qwidget) {
  // Create the usual rendering stuff

  _renderer = vtkRenderer::New();

  _renderWindow = vtkGenericOpenGLRenderWindow::New();
  _renderWindow->AddRenderer(_renderer);

  _vtkWidget = Qwidget;
  _vtkWidget->setRenderWindow(_renderWindow);

  //_renderWindow = (vtkGenericOpenGLRenderWindow)_vtkWidget->renderWindow();

  /*
  interaction style
  */

  vtkNew<vtkInteractorStyleSwitch> style_switch;
  vtkNew<vtkInteractorStyleTrackballCamera> style_trackball_camera;
  _renderWindow->GetInteractor()->SetInteractorStyle(style_trackball_camera);

  /*
  Axes
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

vtkSmartPointer<vtkActor> VtkWrapper::processHedgehog(
    const std::vector<Point3d> &points, const std::vector<Point3d> &vectors,
    double scale_factor, double line_width, Color color) {
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

vtkSmartPointer<vtkActor> VtkWrapper::processMesh(
    const std::vector<Point3d> &points, const std::vector<Triangle> &faces) {
  /* insert vertices */
  vtkNew<vtkPoints> nodes;
  size_t n_vertices = points.size();
  nodes->GetData()->Allocate(n_vertices);
  for (int i = 0; i < n_vertices; ++i) {
    nodes->InsertPoint(i, points[i].data());
  }

  /* insert faces */
  vtkNew<vtkCellArray> triangles;
  size_t n_faces = faces.size();
  //每个单元有4个数据 1个表示点的数量，3个表示顶点的标号
  triangles->GetData()->Allocate((1 + 3) * n_faces);
  for (auto i = 0; i < n_faces; ++i) {
    triangles->InsertNextCell(3, faces[i].data());
  }

  /* form mesh data */

  vtkNew<vtkPolyData> mesh_data;
  mesh_data->SetPoints(nodes);
  mesh_data->SetPolys(triangles);

  /***** mapper *****/

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(mesh_data);

  /********** actors *************/

  vtkNew<vtkActor> face_actor;
  face_actor->SetMapper(mapper);

  return face_actor;
}

void VtkWrapper::toggleSurfaceRepresentation(int nRenderType) {
  auto actors = _renderer->GetActors();
  // figure out there are how many actors
  int num = actors->GetNumberOfItems();
  // reset the index
  actors->InitTraversal();
  for (int i = 0; i < num; i++) {
    auto actor = actors->GetNextActor();
    actor->GetProperty()->SetRepresentation(nRenderType);
  }

  //_renderer->Modified();
}

// not completed
bool VtkWrapper::readJsonSettings() {
  std::ifstream fin(_settings_jsonfile);
  return false;

  // fin.close(); RAII
}

void VtkWrapper::testRenderFunction() {}

}  // namespace viewtools