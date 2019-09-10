#pragma once

#include <Eigen/Dense>
#include <initializer_list>
#include <iostream>
#include <vector>
#include <assert.h>

namespace viewtools {
template <typename T, size_t size> class tuple {
protected:
  T _data[size];

public:
  tuple(){};
  tuple(std::initializer_list<T> para) {
    size_t n = para.size();
    assert(n == size);
    size_t i = 0;
    for (auto u : para) {
      _data[i] = u;
      i++;
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

/** vtkFacetTuple
        store vertices's ids of a face that will be rendered.
*/
template <size_t cell_n>
using vtkFacetTuple = tuple<long long, cell_n>;

/*********** defines begin **************/

using Point3d = Triple<double>;
template <int n>
// VertexList<n> is a n-list storing the vertices of a facet
using VertexList = Eigen::Matrix<long long, n, 1>;

/*********** defines end **************/

class Color : public Triple<double> {
public:
  Color(double *data);
  Color(double r, double g, double b);
  Color(std::string description);

private:
};
} // namespace viewtools