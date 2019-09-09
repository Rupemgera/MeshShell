#pragma once
#include "view_wrapper.h"

namespace viewtools {
template <int cell_n>
inline vtkSmartPointer<vtkPolyData> VtkWrapper::processPolyData(
    const std::vector<Point3d> &points,
    const std::vector<tuple<long long, cell_n>> &polys) {
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
  //每个单元有4个数据 1个存储顶点个数，3个存储面片顶点的标号
  cells->GetData()->Allocate((1 + cell_n) * n_faces);
  for (auto i = 0; i < n_faces; ++i) {
    cells->InsertNextCell(cell_n, polys[i].data());
  }

  /* form mesh data */

  vtkNew<vtkPolyData> data;
  data->SetPoints(nodes);
  data->SetPolys(cells);
  return data;
}
} // namespace viewtools
