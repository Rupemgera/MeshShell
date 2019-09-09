#include "mesh_wrapper.h"
#include "view_tools/view_wrapper.h"

#include <map>

/*********** defines begin **************/
using MeshWrapper = meshtools::MeshWrapper;
using VtkWrapper = viewtools::VtkWrapper;
using ActorControler = viewtools::ActorControler;
using ActorMap = std::map<std::string, ActorControler *>;
/*********** defines end **************/

class MeshShell {
public:
  MeshShell(VtkWrapper *viewer);
  ~MeshShell();

  void drawMesh(int nRenderStyle = 3);

  void readMesh(std::string filename);

  void updateMeshRenderStyle(int nRenderStyle);

  void drawShrink(int nRenderStyle = 3);

  void setVertexScalars(std::vector<double> &scalars, double lower_bound,
                        double upper_bound);

  void renderScalars(vtkSmartPointer<vtkActor> actor, bool flag);

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

  void stressSingularity(double tolerance);

  /************************* stress  end  *************************/

  bool mesh_loaded = false;

  bool shrinked = false;

  std::string mesh_name;

  MeshWrapper *ovm_mesh;

protected:

  VtkWrapper *_viewer;

  ActorControler *_main_actor = nullptr;

  ActorControler *_shrink_actor = nullptr;

  ActorMap map_actors;

	void insert_actor(ActorControler *a);
};