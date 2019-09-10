#pragma once

#include "view_templates.hpp"
#include "vtk_heads.h"
#include <map>

namespace viewtools {

class ActorControler {
protected:
  vtkSmartPointer<vtkActor> _actor;

public:
  ActorControler(std::string name, vtkSmartPointer<vtkActor> actor);
  virtual ~ActorControler();

  void setVisibility(bool visibility);

  virtual void setRenderSyle(int nRenderStyle);

  void setOpacity(double opacity);

  virtual void setColor(Color color);

  virtual void setSize(double size);

  vtkSmartPointer<vtkActor> get_actor();

  std::string name;
};

class MeshActorControler : public ActorControler {
public:
  struct {
    // whether edges are rendered
    bool edge_on = false;

    Color edge_color = Color(0.0, 0.545, 0.0);

    // whether faces are rendered
    bool face_on = false;

    Color face_color = Color(1.0, 1.0, 1.0);
  } render_status;

  MeshActorControler(std::string name, vtkSmartPointer<vtkActor> actor);

  void setRenderSyle(int nRenderStyle);
};

class PointsActorControler : public ActorControler {
public:
  struct {
    ////// properties of points //////////////

    float point_size = 1.0;

    Color point_color = Color(1.0, 0, 0);
  } render_status;

  PointsActorControler(std::string name, vtkSmartPointer<vtkActor> actor);

  ////////////// function about points render //////////////////

  /**
          set size of points
  */
  void setSize(double size);
};

/**
        maintain all active actors.

*/
class ActorTable {
protected:
  std::map<std::string, ActorControler *> _table;

public:
  void insert(ActorControler *u);

  /**
          ActorControl * will be deleted
  */
  void remove(const std::string key);

  /**
          ActorControl * will be deleted
  */
  void remove(std::map<std::string, ActorControler *>::iterator &id);

  std::map<std::string, ActorControler *>::iterator find(std::string key);

  std::map<std::string, ActorControler *>::iterator begin();

  std::map<std::string, ActorControler *>::iterator end();
};

class VtkWrapper {
public:
  VtkWrapper(QVTKOpenGLWidget *widget);
  ~VtkWrapper();

  /*vtk render functions*/

  int renderActor(vtkActor *actor);

  /* delete actor */

  int removeActor(vtkActor *actor);

  /* refresh */

  int refresh();

  /* reset camera */

  void resetCamera();

  /********** process data begin **********/

  /** transform data to vtk render data
          @param cell_n	the number of vertices of the cell
  */
  template <int cell_n>
  vtkSmartPointer<vtkPolyData>
  processPolyData(const std::vector<Eigen::Vector3d> &points,
                  const std::vector<VertexList<cell_n>> &polys);

  /* vector field */
  vtkSmartPointer<vtkActor>
  processHedgehog(const std::vector<Eigen::Vector3d> &points,
                  const std::vector<Eigen::Vector3d> &vectors,
                  double scale_factor = 0.8, double line_width = 1.0);

  /* mesh */

  template <int cell_n>
  vtkSmartPointer<vtkActor>
  processMesh(const std::vector<Eigen::Vector3d> &points,
              const std::vector<VertexList<cell_n>> &faces);

  /* Vertex Scalar */
  void setVertexScalars(std::vector<double> &scalars, double lower_bound,
                        double upper_bound, vtkSmartPointer<vtkActor> actor);

  vtkSmartPointer<vtkActor> processPoints(std::vector<Eigen::Vector3d> &points);

  /********** process data  end  **********/

  //
  bool readJsonSettings();

  // test interface
  void testRenderFunction();

private:
  vtkSmartPointer<vtkRenderer> _renderer;
  // renderWindow
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _renderWindow;
  vtkSmartPointer<vtkOrientationMarkerWidget> _markerWidget;
  QVTKOpenGLWidget *_vtkWidget;

  std::string _settings_jsonfile;
};
} // namespace viewtools

#include "view_templates_impl.hpp"