#pragma once

#include <unordered_set>

#include "mesh_wrapper.h"
#include "view_wrapper.h"

class VtkMesh
{
public:
	/*构造函数*/

	VtkMesh(QVTKOpenGLWidget *widget);

	//VtkMesh(std::string filename, QVTKOpenGLWidget * widget);

	/*
	functions
	*/

	/*
	properties
	*/

private:
	/*
	数据区
	*/
	std::shared_ptr<meshtools::MeshWrapper> mesh;

	std::shared_ptr<viewtools::VtkWrapper> viewer;
};