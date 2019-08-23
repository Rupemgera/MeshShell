#include "mesh_wrapper.h"
#include "view_wrapper.h"

#include <map>

/*********** defines begin **************/
using MeshWrapper = meshtools::MeshWrapper;
using VtkWrapper = viewtools::VtkWrapper;
using ActorMap = std::map<std::string, vtkSmartPointer<vtkActor>>;
/*********** defines end **************/

class MeshShell : public MeshWrapper {
 public:
  MeshShell(VtkWrapper *viewer);
  void drawMesh();

 protected:
  VtkWrapper *_viewer;
  ActorMap map_actors;
};