#pragma once

#include <initializer_list>
#include <iostream>
#include <vector>

#include "vtk_heads.h"

namespace viewtools {

template <typename T, int size> class tuple {
protected:
  T _data[size];

public:
  tuple(){};
  tuple(std::initializer_list<T> para) {
    int n = std::min(para.size(), size);
    for (int i = 0; i < n; ++i) {
      _data[i] = para[i];
    }
  }
  tuple(T *para) {
    for (int i = 0; i < size; ++i) {
      _data[i] = para[i];
    }
  }
  const T *data() const { return _data; }

  T *data() { return _data; }
};

template <typename T> class Triple : public tuple<T, 3> {
public:
  Triple() {}
  Triple(T u, T v, T w) {
    _data[0] = u;
    _data[1] = v;
    _data[2] = w;
  }
  Triple(T *data) {
    for (int i = 0; i < 3; ++i)
      _data[i] = data[i];
  }
};

/*********** defines begin **************/
using Point3d = Triple<double>;
using Triangle = Triple<long long>;
/*********** defines end **************/

class Color : public Triple<double> {
public:
  Color(double *data);
  Color(double r, double g, double b);
  Color(std::string description);

private:
};

class ActorControler {
private:
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
  } render_status;

  ActorControler(vtkSmartPointer<vtkActor> actor);

	void setVisibility(bool visibility);

  void setRenderSyle(int nRenderStyle);

  void setColor(Color face_color = Color(1.0, 1.0, 1.0),
                Color edge_color = Color(0.0, 0.0, 1.0));

  vtkSmartPointer<vtkActor> get_actor();
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

  /* vector field */
  vtkSmartPointer<vtkActor> processHedgehog(const std::vector<Point3d> &points,
                                            const std::vector<Point3d> &vectors,
                                            double scale_factor = 1.0,
                                            double line_width = 2.0,
                                            Color color = Color(1, 0, 0));

  /* mesh */

  vtkSmartPointer<vtkActor> processMesh(const std::vector<Point3d> &points,
                                        const std::vector<Triangle> &faces);

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
