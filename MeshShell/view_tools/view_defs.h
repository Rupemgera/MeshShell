#pragma once

#include <Eigen/Dense>

/***************** defines ********************/

using Point3d = Eigen::Vector3d;
template <int n>
// VertexList<n> is a n-list storing the vertices of a facet
using VertexList = Eigen::Matrix<long long, n, 1>;

/***************** classes ********************/

class Color {
public:
  Color(double *data) {
    for (size_t i = 0; i < 3; i++) {
      _data[i] = data[i];
    }
  }
  Color(double r, double g, double b) {
    _data[0] = r;
    _data[1] = g;
    _data[2] = b;
  }
  Color(std::string description) {
    if (description == "red") {
      _data[0] = 1.0;
      _data[1] = 0.0;
      _data[2] = 0.0;
    } else if (description == "green") {
      _data[0] = 0.0;
      _data[1] = 1.0;
      _data[2] = 0.0;
    } else if (description == "blue") {
      _data[0] = 0.0;
      _data[1] = 0.0;
      _data[2] = 1.0;
    } else {
      _data[0] = 1.0;
      _data[1] = 1.0;
      _data[2] = 1.0;
    }
  }
  const double *data() const { return _data; }
  double *data() { return _data; }

private:
  double _data[3];
};