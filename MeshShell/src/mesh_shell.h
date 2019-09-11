#include "mesh_wrapper.h"
#include "view_tools/view_wrapper.h"

/*********** defines begin **************/

using MeshWrapper = meshtools::MeshWrapper;
using VtkWrapper = viewtools::VtkWrapper;
using ActorControler = viewtools::ActorControler;
using MeshActorControler = viewtools::MeshActorControler;
using PointsActorControler = viewtools::PointsActorControler;

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
  MeshShell(VtkWrapper *viewer);
  ~MeshShell();

  void drawMesh(int nRenderStyle = 3);

  void readMesh(std::string filename);

  void updateMeshRenderStyle(int nRenderStyle);

  /**
          geometry = 1 : normal mesh
          geometry = 2 : shrinked mesh
  */
  void updateFaceOpacity(double opacity, int geometryStyle);

  void drawShrink(int nRenderStyle = 3);

  void setVertexScalars(std::vector<double> &scalars, double lower_bound,
                        double upper_bound);

  void renderScalars(vtkSmartPointer<vtkActor> actor, bool flag);

  /************************* actor related begin *************************/

  /**
          @return		return true if actor exits, false if not
  */
  bool setVisibility(std::string actor_name, bool visi);

  /************************* actor related end *************************/

  /************************* stress begin *************************/

  /**
input two groups of data, separated by ','
first group contains 1 integer, that is the id of the cell
second group contains 6 decimal, that the 6 tensor stress component,
ordered by XX YY ZZ XY YZ ZX
*/
  void readStressField(std::string filename);

  void drawStressField(bool major = true, bool middle = false,
                       bool minor = false);

  void stressSingularity(double tolerance, double point_size = 10.0);

  void singularitySizeChange(int point_size);

  /************************* stress  end  *************************/

  bool mesh_loaded = false;

  bool shrinked = false;

  std::string mesh_name;

  MeshWrapper *ovm_mesh;

	TetMeshData mesh_data;

protected:
  VtkWrapper *_viewer;

  ActorControler *_main_actor = nullptr;

  ActorControler *_shrink_actor = nullptr;

  /**
          store all active actors
  */
  viewtools::ActorTable _actors_table;
};