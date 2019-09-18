#pragma once

#include <Eigen/Dense>
#include <string>
#include <tuple>
#include <vector>

class ViewManager {
public:
  ViewManager(){};
  virtual ~ViewManager(){};

  /*************** renderer related ************/

  /**
   *@brief refresh renderer
   */
  virtual int refresh() = 0;

  virtual void resetCamera() = 0;

  /********** data related **********/

  /** transform data to vtk render data
          @param cell_n	the number of vertices of the cell
  */

  /**
   *@param name name is the key for seaching
   */
  virtual bool
  drawTetMesh(std::string name, const std::vector<Eigen::Vector3d> &points,
              const std::vector<Eigen::Matrix<long long, 3, 1>> &faces) = 0;

  /**
   *@brief draw vector field
   *@param scale_factor the scale rate of vector length
   */
  virtual bool drawVector(std::string name,
                          const std::vector<Eigen::Vector3d> &points,
                          const std::vector<Eigen::Vector3d> &vectors,
                          double scale_factor = 0.8,
                          double line_width = 1.0) = 0;

  /**
   *@brief set Vertex Scalar for every vertex
   */
  virtual void setVertexScalars(std::string name, std::vector<double> &scalars,
                                double lower_bound, double upper_bound) = 0;
  /**
   *@brief draw poly lines
   */
  virtual bool
  drawLines(std::string name,
            const std::vector<std::vector<Eigen::Vector3d>> &points,
            bool is_loop = false) = 0;

  virtual bool drawSegments(
      std::string name, const std::vector<Eigen::Vector3d> &points,
      const std::vector<Eigen::Matrix<long long, 2, 1>> &vertices_pairs) = 0;

  virtual bool drawPoints(std::string name,
                          const std::vector<Eigen::Vector3d> &points,
                          double point_size) = 0;

  /**
   *@param render_style 1: only edges 2: only faces 3:edges and faces
   */
  virtual void setRenderStyle(std::string name, int render_style) = 0;

  /**
   *@param opacity from 0.0 to 1.0
   */
  virtual void setOpacity(std::string name, double opacity) = 0;

  virtual void setVisibility(std::string name, bool flag) = 0;

  virtual void setColor(std::string name, double *color) = 0;

  virtual void setSize(std::string name, double size) = 0;

  /**
   *@brief whether name is in actor table
   */
  virtual bool exit(std::string name) = 0;

  /********** process data  end  **********/

  // test interface
  virtual void testRenderFunction() = 0;
};