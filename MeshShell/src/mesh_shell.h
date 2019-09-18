#include "mesh_wrapper.h"
#include "view_manager_fwd.h"

/*********** defines begin **************/

using MeshWrapper = meshtools::MeshWrapper;

/*********** defines end **************/

class TetMeshData {
public:
  TetMeshData();

  void getFaceData(std::vector<int> &face_ids,
                   std::vector<meshtools::FaceList<3>> &ext);

  std::vector<Eigen::Vector3d> points;
  std::vector<meshtools::FaceList<3>> faces;
  std::vector<meshtools::FaceList<3>> boundary_faces;
};

class MeshShell {
public:
  MeshShell(ViewManager *viewer);
  ~MeshShell();

  void drawMesh(int nRenderStyle = 3);

  void readMesh(std::string filename);

  void updateMeshRenderStyle(int nRenderStyle);

  /**
   *geometry = 1 : normal mesh
   *geometry = 2 : shrinked mesh
   */
  void updateFaceOpacity(double opacity, int geometryStyle);

  void drawShrink(int nRenderStyle = 3);

  /*void setVertexScalars(std::vector<double> &scalars, double lower_bound,
                        double upper_bound);*/

  /*void renderScalars(vtkSmartPointer<vtkActor> actor, bool flag);*/

  /************************* actor related begin *************************/

  /**
   *@brief set visibility status of actors
   */
  void setVisibility(std::string name, bool visi);

  void setVertexScalars(std::string name, std::vector<double> &scalars,
                        double lower_bound, double upper_bound);

  /************************* actor related end *************************/

  /************************* stress begin *************************/

  /**
   *input two groups of data, separated by ','
   *first group contains 1 integer, that is the id of the cell
   *second group contains 6 decimal, that the 6 tensor stress component,ordered
   *by XX YY ZZ XY YZ ZX
   */
  bool readStressField(std::string filename);

  void drawStressField(bool major = true, bool middle = false,
                       bool minor = false);

  void stressSingularity(double tolerance, double point_size = 10.0);

  void singularitySizeChange(int point_size);

  void divideCells(double tolerance);

  void extractSingularLines();

  /************************* stress  end  *************************/

  /***************************** test ***********************************/

  void test();


  bool mesh_loaded = false;

  bool shrinked = false;

  std::string mesh_name;

  MeshWrapper *mesh_wrapper;

  TetMeshData mesh_data;

protected:
  ViewManager *_viewer;
};