#pragma once
#include "view_wrapper.h"

namespace viewtools {
template <int cell_n>
inline vtkSmartPointer<vtkPolyData>
VtkWrapper::processPolyData(const std::vector<Eigen::Vector3d> &points,
                            const std::vector<vtkFacetTuple<cell_n>> &polys) {
  /* insert vertices */
  vtkNew<vtkPoints> nodes;
  size_t n_vertices = points.size();
  nodes->GetData()->Allocate(n_vertices);
  for (int i = 0; i < n_vertices; ++i) {
    nodes->InsertPoint(i, points[i].data());
  }

  /* insert polys */
  vtkNew<vtkCellArray> cells;
  size_t n_faces = polys.size();
  //每个单元有1+cell_n个数据 1个存储顶点个数cell_n，cell_n个存储面片顶点的标号
  cells->GetData()->Allocate((1 + cell_n) * n_faces);
  for (auto i = 0; i < n_faces; ++i) {
    cells->InsertNextCell(cell_n, polys[i].data());
  }

  /* form poly data */

  vtkNew<vtkPolyData> data;
  data->SetPoints(nodes);
  if (cell_n == 1)
    data->SetVerts(cells);
  else if (cell_n == 2)
		data->SetLines(cells);
  else
    data->SetPolys(cells);
  return data;
}
} // namespace viewtools
