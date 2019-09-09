#pragma once

#include "view_templates.hpp"
#include "vtk_heads.h"
#include <Eigen/Dense>
#include <map>

namespace viewtools {

class ActorControler {
protected:
  vtkSmartPointer<vtkActor> _actor;

public:
  struct {
    // whether edges are rendered
    bool edge_on = false;

    Color edge_color = Color(0.0, 0.0, 1.0);

    // whether faces are rendered
    bool face_on = false;

    Color face_color = Color(1.0, 1.0, 1.0);

    // whether to render stress field
    bool field_on = false;

    ////// properties of points //////////////

    float point_size = 1.0;
  } render_status;

  ActorControler(std::string name, vtkSmartPointer<vtkActor> actor);

  void setVisibility(bool visibility);

  void setRenderSyle(int nRenderStyle);

	void setOpacity(double opacity);

  void setColor(Color edge_color = Color(0.0, 0.0, 1.0),
                Color face_color = Color(1.0, 1.0, 1.0));

  ////////////// function about points render //////////////////

  void setPointSize(float size);

  vtkSmartPointer<vtkActor> get_actor();

  std::string name;
};

/**
	maintain all active actors.

*/
class ActorTable{
protected:
	std::map<std::string, ActorControler *> _table;

public:
	void insert(ActorControler *u);

	std::map<std::string, ActorControler *>::iterator find(std::string key);

	std::map<std::string, ActorControler *>::iterator begin();

	std::map<std::string, ActorControler *>::iterator end();

};

class VtkWrapper {
public:
  VtkWrapper(QVTKOpenGLWidget *widget);

  /*vtk render functions*/

  int renderActor(vtkActor *actor);

  /* delete actor */

  int removeActor(vtkActor *actor);

  /* refresh */

  int refresh();

  /********** process data begin **********/

  /** transform data to vtk render data
          @param cell_n	the number of vertices of the cell
  */
  template <int cell_n>
  vtkSmartPointer<vtkPolyData>
  processPolyData(const std::vector<Eigen::Vector3d> &points,
                  const std::vector<vtkFacetTuple<cell_n>> &polys);

  /* vector field */
  vtkSmartPointer<vtkActor> processHedgehog(const std::vector<Eigen::Vector3d> &points,
                                            const std::vector<Eigen::Vector3d> &vectors,
                                            Color color = Color(1, 0, 0),
                                            double scale_factor = 0.8,
                                            double line_width = 1.0);

  /* mesh */

  vtkSmartPointer<vtkActor> processMesh(const std::vector<Eigen::Vector3d> &points,
                                        const std::vector<Triangle> &faces);

  /* Vertex Scalar */
  void setVertexScalars(std::vector<double> &scalars, double lower_bound,
                        double upper_bound, vtkSmartPointer<vtkActor> actor);

  vtkSmartPointer<vtkActor> processPoints(std::vector<Eigen::Vector3d> &points);

  /********** process data  end  **********/

  /*vtkSmartPointer<vtkActor>
  processShrinkMesh(const std::vector<Point3d> &points, );*/

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