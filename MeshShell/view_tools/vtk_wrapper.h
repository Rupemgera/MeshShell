#pragma once

#include "src/view_manager_fwd.h"
#include "view_defs.h"
#include "vtk_heads.h"
#include "vtk_templates.hpp"
#include <map>

namespace viewtools {

/******************* defines ***************************/

using ActorPtr = vtkSmartPointer<vtkActor>;

class ActorControler {
protected:
  ActorPtr _actor;

  bool visibility_flag = true;

public:
  ActorControler(std::string name, ActorPtr actor);
  virtual ~ActorControler();

  void setVisibility(bool visibility);

  bool getVisibility();

  virtual void setRenderSyle(int nRenderStyle);

  virtual void setOpacity(double opacity);

  virtual void setColor(Color color);

  virtual void setColor(double r, double g, double b);

  virtual void setSize(double size);

  // tuple 1.visibility 2.opacity 3.size
  virtual std::tuple<bool, double, double> getStatus();

  ActorPtr get_actor();

  std::string name_;
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

  MeshActorControler(std::string name, ActorPtr actor);
  /**
   * 1: edge 2:face 3:both
   */
  void setRenderSyle(int nRenderStyle);
};

class PointsActorControler : public ActorControler {
public:
  struct {
    ////// properties of points //////////////

    float point_size = 1.0;

    Color point_color = Color(0.4157, 0.3529, 0.8039);
  } render_status;

  PointsActorControler(std::string name, ActorPtr actor);

  ////////////// function about points render //////////////////

  /**
   * set size of points
   */
  void setSize(double size);

  std::tuple<bool, double, double> getStatus();
};

class SegmentActorControler : public ActorControler {
public:
  SegmentActorControler(std::string name, ActorPtr actor)
      : ActorControler(name, actor) {}

  void setSize(double size);
};

class VectorActorControler : public ActorControler {
public:
  VectorActorControler(std::string name, ActorPtr actor)
      : ActorControler(name, actor) {}
};

class VtkWrapper : public ViewManager {
protected:
  // store name-actor pair
  // maintain all active actors.
  std::map<std::string, ActorControler *> _table_;

  /** Axes
   * add a Orthogonal coordinate to the bottom left of the screen
   */
  void addAxes();

  /******************** source manage part **************************/

  /**
   *@brief insert into _table_
   */
  void insert(std::string name, ActorControler *actor_controler);

  void remove(std::string name, ActorControler *actor_controler);

  /*********** process data begin **************************/

  /** transform data to vtk render data
          @param cell_n	the number of vertices of the cell
  */
  template <int cell_n>
  vtkSmartPointer<vtkPolyData>
  processPolyData(const std::vector<Eigen::Vector3d> &points,
                  const std::vector<VertexList<cell_n>> &polys);

  /**
   *@param name name is the key for seaching
   */
  template <int cell_n>
  bool drawMesh(std::string name, const std::vector<Eigen::Vector3d> &points,
                const std::vector<VertexList<cell_n>> &faces);

  /********** process data  end  **********/

  /***************** render part *********************/

  int addActor(vtkActor *actor);

  int removeActor(vtkActor *actor);

public:
  VtkWrapper(QVTKOpenGLWidget *widget);

  ~VtkWrapper();

  std::map<std::string, ActorControler *> &getTable();

  int refresh();

  /* reset camera */
  void resetCamera();

  // test interface
  void testRenderFunction();

  /* vector field */
  bool drawVector(std::string name, const std::vector<Eigen::Vector3d> &points,
                  const std::vector<Eigen::Vector3d> &vectors,
                  double scale_factor = 0.8, double line_width = 1.0);

  /* Vertex Scalar */
  void setVertexScalars(std::string name, std::vector<double> &scalars,
                        double lower_bound, double upper_bound);

  /**
   *@brief draw segment lines
   */
  bool drawLines(std::string name,
                 const std::vector<std::vector<Eigen::Vector3d>> &points,
                 bool is_loop = false);

  bool drawSegments(
      std::string name, const std::vector<Eigen::Vector3d> &points,
      const std::vector<Eigen::Matrix<long long, 2, 1>> &vertices_pairs,
      double line_width = 2.0);

  bool drawPoints(std::string name, const std::vector<Eigen::Vector3d> &points,
                  double point_size);

  bool drawTetMesh(std::string name, const std::vector<Eigen::Vector3d> &points,
                   const std::vector<Eigen::Matrix<long long, 3, 1>> &faces);
  /**
   *@param opacity from 0.0 to 1.0
   */
  void setOpacity(std::string name, double opacity);

  void setVisibility(std::string name, bool flag);

  void setColor(std::string name, double *color);

  void setSize(std::string name, double size);

  /**
   *@param render_style 1: only edges 2: only faces 3:edges and faces
   */
  void setRenderStyle(std::string name, int render_style);

  bool exit(std::string name);

private:
  vtkSmartPointer<vtkRenderer> _renderer;
  // renderWindow
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _renderWindow;
  vtkSmartPointer<vtkOrientationMarkerWidget> _markerWidget;
  QVTKOpenGLWidget *_vtkWidget;
};
} // namespace viewtools

#include "vtk_wrapper_impl.hpp"