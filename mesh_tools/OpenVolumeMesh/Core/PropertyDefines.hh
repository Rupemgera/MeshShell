/*===========================================================================*\
 *                                                                           *
 *                            OpenVolumeMesh                                 *
 *        Copyright (C) 2011 by Computer Graphics Group, RWTH Aachen         *
 *                        www.openvolumemesh.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenVolumeMesh.                                     *
 *                                                                           *
 *  OpenVolumeMesh is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenVolumeMesh is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenVolumeMesh.  If not,                              *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

#pragma once

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "Entities.hh"
#include "PropertyHandles.hh"
#include "PropertyPtr.hh"

namespace OpenVolumeMesh {

template <class T>
class OpenVolumeMeshPropertyT;

class ResourceManager;

template <class T>
const std::string typeName();

template <> const std::string typeName<int>();
template <> const std::string typeName<unsigned int>();
template <> const std::string typeName<short>();
template <> const std::string typeName<long>();
template <> const std::string typeName<unsigned long>();
template <> const std::string typeName<char>();
template <> const std::string typeName<unsigned char>();
template <> const std::string typeName<bool>();
template <> const std::string typeName<float>();
template <> const std::string typeName<double>();
template <> const std::string typeName<std::string>();
template <>  const std::string typeName<std::map<HalfEdgeHandle, int> >();
template <>  const std::string typeName<std::vector<double> >();
template <>  const std::string typeName<std::vector<VertexHandle> >();
template <>  const std::string typeName<std::vector<HalfFaceHandle> >();
template <>  const std::string typeName<std::vector<std::vector<HalfFaceHandle> > >();

template<typename Entity>
const std::string entityTypeName();

template <> const std::string entityTypeName<Entity::Vertex>();
template <> const std::string entityTypeName<Entity::HalfEdge>();
template <> const std::string entityTypeName<Entity::Edge>();
template <> const std::string entityTypeName<Entity::Face>();
template <> const std::string entityTypeName<Entity::HalfFace>();
template <> const std::string entityTypeName<Entity::Cell>();
template <> const std::string entityTypeName<Entity::Mesh>();

template<typename T, typename Entity>
class PropertyTT : public PropertyPtr<OpenVolumeMeshPropertyT<T>, Entity> {
public:
    template<typename MeshT>
    PropertyTT(MeshT *mesh, const std::string& _name, const T &_def = T())
        : PropertyTT(std::move(mesh->template request_property<T, Entity>(_name, _def)))
    {}
    using PropertyHandleT = OpenVolumeMesh::PropHandleT<Entity>;
    PropertyTT(const std::string& _name, const std::string& _internal_type_name, ResourceManager& _resMan, PropertyHandleT _handle, const T &_def = T());
    ~PropertyTT() override = default;
    BaseProperty* clone(ResourceManager &_resMan, OpenVolumeMeshHandle _handle) const override;
    const std::string entityType() const override { return entityTypeName<Entity>(); }
    const std::string typeNameWrapper() const override { return typeName<T>(); }
private:
    PropertyTT(OpenVolumeMeshPropertyT<T> *_prop, ResourceManager& _resMan, PropertyHandleT _handle);
};

template<typename T> using VertexPropertyT   = PropertyTT<T, Entity::Vertex>;
template<typename T> using EdgePropertyT     = PropertyTT<T, Entity::Edge>;
template<typename T> using HalfEdgePropertyT = PropertyTT<T, Entity::HalfEdge>;
template<typename T> using FacePropertyT     = PropertyTT<T, Entity::Face>;
template<typename T> using HalfFacePropertyT = PropertyTT<T, Entity::HalfFace>;
template<typename T> using CellPropertyT     = PropertyTT<T, Entity::Cell>;
template<typename T> using MeshPropertyT     = PropertyTT<T, Entity::Mesh>;


} // Namespace OpenVolumeMesh

#if defined(INCLUDE_TEMPLATES) && !defined(PROPERTYDEFINEST_CC)
#include "PropertyDefinesT_impl.hh"
#endif
