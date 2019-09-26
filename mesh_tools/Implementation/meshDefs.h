#pragma once

#include <memory>

#include <Eigen/Dense>
#include "OpenVolumeMesh/Mesh/PolyhedralMesh.hh"
#include "OpenVolumeMesh/Core/OpenVolumeMeshHandle.hh"
#include "OpenVolumeMesh/FileManager/FileManager.hh"
#include "OpenVolumeMesh/Attribs/NormalAttrib.hh"

//definitions for polyhedral meshes
using OvmVeH = OpenVolumeMesh::VertexHandle;
using OvmFaH = OpenVolumeMesh::FaceHandle;
using OvmHaFaH = OpenVolumeMesh::HalfFaceHandle;
using OvmEgH = OpenVolumeMesh::EdgeHandle;
using OvmHaEgH = OpenVolumeMesh::HalfEdgeHandle;
using OvmCeH = OpenVolumeMesh::CellHandle;

using VMesh = OpenVolumeMesh::GeometricPolyhedralMeshV3d;
using VMeshPtr = std::shared_ptr<VMesh>;

using MeshPoint = OpenVolumeMesh::Geometry::Vec3d;
using OvmVec3d = OpenVolumeMesh::Geometry::Vec3d;

using _Matrix_3 = double[3][3];
using Matrix_3 = Eigen::Matrix<double, 3, 3>;
using Triangle = Eigen::Matrix<long long, 3, 1>;

class OvmHandleHasher
{
public:
	size_t operator()(const OpenVolumeMesh::OpenVolumeMeshHandle &k) const noexcept
	{
		return std::hash<int>{}(k.idx());
	}
};

class OvmHandleComparator
{
public:
	bool operator()(const OpenVolumeMesh::OpenVolumeMeshHandle &k1, const OpenVolumeMesh::OpenVolumeMeshHandle &k2) const noexcept
	{
		return k1.idx() == k2.idx();
	}
};