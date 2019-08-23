#pragma once

#include <iostream>
#include <vector>

#include "vtk_heads.h"

namespace viewtools {

template <typename T>
class Triple {
 public:
  Triple() {}
  Triple(T u, T v, T w) {
    _data[0] = u;
    _data[1] = v;
    _data[2] = w;
  }
  Triple(T *data) {
    for (int i = 0; i < 3; ++i) _data[i] = data[i];
  }
  const T *data() const { return _data; }

  T *data() { return _data; }

 protected:
  T _data[3];
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

  /*
          set surface property of mesh
          0 : VTK_POINTS
          1 : VTK_WIREFRAME
          2 : VTK_SURFACE
          */
  void toggleSurfaceRepresentation(int nRenderType);

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
}  // namespace viewtools
