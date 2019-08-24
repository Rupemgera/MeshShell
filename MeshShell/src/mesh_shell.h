#include "mesh_wrapper.h"
#include "view_tools/view_wrapper.h"

#include <map>

/*********** defines begin **************/
using MeshWrapper = meshtools::MeshWrapper;
using VtkWrapper = viewtools::VtkWrapper;
using ActorControler = viewtools::ActorControler;
using ActorMap = std::map<std::string, vtkSmartPointer<vtkActor>>;
/*********** defines end **************/

class MeshShell {
public:
  MeshShell(VtkWrapper *viewer);
  ~MeshShell();

  void drawMesh(int nRenderStyle = 3);

  void readMesh(std::string filename);

	void updateMeshRenderStyle(int nRenderStyle);

  bool mesh_loaded = false;

	std::string mesh_name;

	MeshWrapper *ovm_mesh;

protected:

  VtkWrapper *_viewer;

	ActorControler *_main_actor;
  
  ActorMap map_actors;
};