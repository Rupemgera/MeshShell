#include "vtk_wrapper.h"
#include <fstream>

namespace viewtools {

/************************* ActorControler begin *************************/

ActorControler::ActorControler(std::string name, ActorPtr actor)
    : name_(name), _actor(actor) {}

ActorControler::~ActorControler() {}

void ActorControler::setVisibility(bool visibility) {
  visibility_flag = visibility;
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

std::tuple<int, int, int> ActorControler::getIntColor() {
  double *c = _actor->GetProperty()->GetColor();
  int r = static_cast<int>(c[0] * 255.0);
  int g = static_cast<int>(c[1] * 255.0);
  int b = static_cast<int>(c[2] * 255.0);
  return std::tuple<int, int, int>(r, g, b);
}

void ActorControler::setColor(double r, double g, double b) {
  _actor->GetProperty()->SetColor(r, g, b);
}

void ActorControler::setSize(double size) {
  _actor->GetProperty()->SetLineWidth(size);
}

double ActorControler::getSize() {
  return _actor->GetProperty()->GetLineWidth();
}

bool ActorControler::getVisibility() { return visibility_flag; }

ActorControlerType ActorControler::getClassType() {
  return ActorControlerType::BASE;
}

std::tuple<bool, double, double> ActorControler::getStatus() {
  double opacity = _actor->GetProperty()->GetOpacity();
  double size = _actor->GetProperty()->GetLineWidth();
  return std::tuple<bool, double, double>(visibility_flag, opacity, size);
}

ActorPtr ActorControler::get_actor() { return _actor; }

void ActorControler::setOpacity(double opacity) {
  _actor->GetProperty()->SetOpacity(opacity);
}

/************************* ActorControler  end  *************************/

/************************* MeshActorControler  begein *************************/

MeshActorControler::MeshActorControler(std::string name, ActorPtr actor)
    : ActorControler(name, actor) {
  _actor->GetProperty()->SetColor(render_status.face_color.data());
  _actor->GetProperty()->SetEdgeColor(render_status.edge_color.data());
}

void MeshActorControler::setRenderSyle(int nRenderStyle) {
  if (nRenderStyle & 2) {
    _actor->VisibilityOn();
    // surface type : points(0), wireframe(1) or surface(2)
    _actor->GetProperty()->SetRepresentation(2);
    render_status.face_on = true;
    _actor->GetProperty()->SetColor(render_status.face_color.data());
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

ActorControlerType MeshActorControler::getClassType() {
  return ActorControlerType::MESH;
}

/************************* MeshActorControler   end  *************************/

/************************* MeshActorControler  begein *************************/

PointsActorControler::PointsActorControler(std::string name, ActorPtr actor)
    : ActorControler(name, actor) {
  _actor->GetProperty()->SetColor(render_status.point_color.data());
}

void PointsActorControler::setSize(double size) {
  render_status.point_size = size;
  _actor->GetProperty()->SetPointSize(size);
}

double PointsActorControler::getSize() {
  return _actor->GetProperty()->GetPointSize();
}

std::tuple<bool, double, double> PointsActorControler::getStatus() {
  double opacity = _actor->GetProperty()->GetOpacity();
  double size = _actor->GetProperty()->GetPointSize();
  return std::tuple<bool, double, double>(visibility_flag, opacity, size);
}

/************************* MeshActorControler   end  *************************/
/************************* SegmentActorControler  *************************/

void SegmentActorControler::setSize(double size) {
  _actor->GetProperty()->SetLineWidth(size);
}

ActorControlerType SegmentActorControler::getClassType() {
  return ActorControlerType::SEGMENT;
}

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

  addAxes();
}

/**
 *brief delete all ActorControler in _table_
 */
VtkWrapper::~VtkWrapper() {
  for (auto &u : _table_) {
    if (u.second != nullptr) {
      delete u.second;
    }
  }
}

std::map<std::string, ActorControler *> &VtkWrapper::getTable() {
  return _table_;
}

int VtkWrapper::addActor(vtkActor *actor) {

  /// insert into _table_

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

void VtkWrapper::resetCamera() { _renderer->ResetCamera(); }

void VtkWrapper::addAxes() {

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

  _markerWidget = vtkOrientationMarkerWidget::New();

  _markerWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  _markerWidget->SetOrientationMarker(axes);
  _markerWidget->SetInteractor(_renderWindow->GetInteractor());
  _markerWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  _markerWidget->SetEnabled(1);
  _markerWidget->InteractiveOn();
}

void VtkWrapper::insert(std::string name, ActorControler *actor_controler) {
  auto target = _table_.find(name);

  // if name is in table, replace old data
  if (target != _table_.end()) {
    removeActor(target->second->get_actor());
    delete target->second;
    target->second = actor_controler;
  } else {
    // else insert new data
    _table_.insert(std::make_pair(name, actor_controler));
  }

  // render actor
  addActor(actor_controler->get_actor());
}

void VtkWrapper::remove(std::string name, ActorControler *actor_controler) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    delete target->second;
    _table_.erase(target);
  }
}

bool VtkWrapper::drawVector(std::string name,
                            const std::vector<Eigen::Vector3d> &points,
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

  // Actor in scene
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(line_width);

  auto ac = new VectorActorControler(name, actor);

  insert(name, ac);

  // actor->GetProperty()->SetColor(color.data());

  return true;
}

bool VtkWrapper::drawVectorWithScalars(std::string name,
                            const std::vector<Eigen::Vector3d> &points,
                            const std::vector<Eigen::Vector3d> &vectors,
                            std::vector<double> &scalars, double scale_factor,
                            double line_width) {
  size_t n = points.size();
  vtkNew<vtkPoints> locs;
  locs->Allocate(n);
  vtkNew<vtkDoubleArray> dirs;
  dirs->SetNumberOfComponents(3);
  dirs->SetNumberOfTuples(n);
  vtkNew<vtkDoubleArray> scals;
  scals->Allocate(n);

  double x[3] = {0, 0, 0};
  double v[3] = {1, 0, 0};
  double low_bound = 1e20, high_bound = -1e20;
  for (int i = 0; i < n; ++i) {
    locs->InsertPoint(i, points[i].data());
    dirs->InsertTuple(i, vectors[i].data());
    scals->InsertNextValue(scalars[i]);
    if (scalars[i] < low_bound) {
      low_bound = scalars[i];
    } else if (scalars[i] > high_bound) {
      high_bound = scalars[i];
    }
  }

  std::cout<<"stress low : "<<low_bound<<"--> high : "<<high_bound<<std::endl;

  vtkNew<vtkPolyData> ps;
  ps->SetPoints(locs);
  ps->GetPointData()->SetVectors(dirs);
  ps->GetPointData()->SetScalars(scals);

  vtkNew<vtkHedgeHog> hedgehog;
  hedgehog->SetInputData(ps);
  hedgehog->SetScaleFactor(scale_factor);

  // Mapper
  vtkNew<vtkPolyDataMapper> mapper;
  // mapper->SetInputConnection(input);
  mapper->SetInputConnection(hedgehog->GetOutputPort());
  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(low_bound,high_bound);

  // Actor in scene _actor->SetMapper(mapper)
  vtkNew<vtkActor> actor;

  // Actor in scene
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(line_width);

  auto ac = new VectorActorControler(name, actor);

  insert(name, ac);

  // actor->GetProperty()->SetColor(color.data());

  return true;
}

void VtkWrapper::setVertexScalars(std::string name,
                                  std::vector<double> &scalars,
                                  double lower_bound, double upper_bound) {
  auto ac = _table_.find(name)->second;
  auto actor = ac->get_actor();
  auto mapper = actor->GetMapper();
  auto data = mapper->GetInput();
  vtkNew<vtkDoubleArray> darray;
  darray->Allocate(scalars.size());
  for (auto d : scalars) {
    darray->InsertNextValue(d);
  }
  darray->SetName(name.c_str());
  data->GetPointData()->SetScalars(darray);
  // data->GetPointData()->SetActiveScalars("scalars");

  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(lower_bound, upper_bound);

  refresh();
}

bool VtkWrapper::drawLines(
    std::string name, const std::vector<std::vector<Eigen::Vector3d>> &points,
    bool is_loop) {
  /* insert vertices */
  // number of all points
  size_t n_points = 0;
  // number of all segments
  size_t n_segs = 0;
  for (auto s : points) {
    n_points += s.size();
    n_segs += s.size() - 1;
  }
  vtkNew<vtkPoints> nodes;
  nodes->GetData()->Allocate(n_points);

  int i = 0;
  for (auto s : points) {
    for (auto p : s) {
      nodes->InsertPoint(i, p.data());
      i++;
    }
  }

  /* insert using vtkLine */
  /* insert polys */
  vtkNew<vtkCellArray> cells;

  vtkNew<vtkPolyData> data;
  //每个单元有1+2个数据 1个存储顶点个数cell_n，cell_n个存储面片顶点的标号
  // cells->GetData()->Allocate((1 + 2) * n_segs);
  int p_id = 0;
  int s_id; // if draw loop, we should add the start point at the end
  for (auto s : points) {
    vtkNew<vtkPolyLine> polyline;
    if (is_loop)
      polyline->GetPointIds()->SetNumberOfIds(s.size() + 1);
    else
      polyline->GetPointIds()->SetNumberOfIds(s.size());
    s_id = p_id;
    for (int i = 0; i < s.size(); ++i) {
      // pairs[0] = p_id;
      // pairs[1] = p_id + 1;
      // cells->InsertNextCell(2, pairs);
      polyline->GetPointIds()->SetId(i, p_id);
      p_id++;
    }
    if (is_loop) {
      /*pairs[0] = p_id;
      pairs[1] = p_id - s.size() + 1;
      cells->InsertNextCell(2, pairs);*/
      polyline->GetPointIds()->SetId(s.size(), s_id);
    }
    cells->InsertNextCell(polyline);
    // skip last point of each segment
    // p_id++;
  }
  /* form poly data */

  data->SetPoints(nodes);
  data->SetLines(cells);
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(data);
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(2);
  // actor->GetProperty()->SetRepresentationToWireframe();
  // actor->GetProperty()->SetEdgeColor(1.0, 0.4, 0.2);
  auto ac = new MeshActorControler(name, actor);
  // ac->setRenderSyle(1);
  double color[] = {1.0, 0.4, 0.2};
  ac->setColor(color);
  insert(name, ac);
  return true;
}

bool VtkWrapper::drawSegments(
    std::string name, const std::vector<Eigen::Vector3d> &points,
    const std::vector<Eigen::Matrix<long long, 2, 1>> &vertices_pairs,
    double line_width) {

  auto data = processPolyData<2>(points, vertices_pairs);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(data);
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  auto ac = new SegmentActorControler(name, actor);
  ac->setColor(0.2, 0.9, 0.0);
  ac->setSize(line_width);
  insert(name, ac);

  return true;
}

bool VtkWrapper::drawPoints(std::string name,
                            const std::vector<Eigen::Vector3d> &points,
                            double point_size) {
  // std::vector<vtkFacetTuple<1>> polys;
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

  auto ac = new PointsActorControler(name, actor);
  // ac->setColor();
  ac->setSize(point_size);
  insert(name, ac);

  return true;
}

bool VtkWrapper::drawTetMesh(
    std::string name, const std::vector<Eigen::Vector3d> &points,
    const std::vector<Eigen::Matrix<long long, 3, 1>> &faces) {
  drawMesh<3>(name, points, faces);
  return true;
}

void VtkWrapper::setOpacity(std::string name, double opacity) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    target->second->setOpacity(opacity);
  }
}

void VtkWrapper::setVisibility(std::string name, bool flag) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    target->second->setVisibility(flag);
  }
}

void VtkWrapper::setColor(std::string name, double *color) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    target->second->setColor(color);
  }
}

std::tuple<int, int, int> VtkWrapper::getColor(std::string name) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    return target->second->getIntColor();
  }
  return std::tuple<int, int, int>(0, 0, 0);
}

void VtkWrapper::setSize(std::string name, double size) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    target->second->setSize(size);
  }
}

double VtkWrapper::getSize(std::string name) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    return target->second->getSize();
  }
  return 0.0;
}

std::tuple<double, double> VtkWrapper::getStatus(std::string name) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    auto status = target->second->getStatus();
    return std::tuple<double, double>(std::get<1>(status), std::get<2>(status));
  }
  return std::tuple<double, double>(1.0, 0.0);
}

void VtkWrapper::setRenderStyle(std::string name, int render_style) {
  auto target = _table_.find(name);
  if (target != _table_.end()) {
    target->second->setRenderSyle(render_style);
  }
}

bool VtkWrapper::exist(std::string name) {
  auto target = _table_.find(name);
  return target != _table_.end();
}

void VtkWrapper::testRenderFunction() {}

ActorControlerType VectorActorControler::getClassType() {
  return ActorControlerType::VECTOR;
}

} // namespace viewtools