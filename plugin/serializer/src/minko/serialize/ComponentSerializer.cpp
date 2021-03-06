/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/serialize/ComponentSerializer.hpp"
#include "msgpack.hpp"
#include "minko/Types.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/Effect.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Box.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/file/Dependency.hpp"

using namespace minko;
using namespace minko::serialize;


std::string
ComponentSerializer::serializeTransform(NodePtr                 node,
                                        AbstractComponentPtr    component,
                                        DependencyPtr           dependencies)
{
    int8_t                                      type        = serialize::TRANSFORM;
    auto                                        transform   = std::static_pointer_cast<component::Transform>(component);
    std::stringstream                           buffer;
    std::tuple<uint, std::string>               serialized  = serialize::TypeSerializer::serializeMatrix4x4(transform->matrix());
    msgpack::type::tuple<uint, std::string>     src(std::get<0>(serialized), std::get<1>(serialized));

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializePerspectiveCamera(NodePtr                 node,
                                                AbstractComponentPtr    component,
                                                DependencyPtr           dependencies)
{
    int8_t              type = serialize::PROJECTION_CAMERA;
    auto                perspectiveCamera = std::static_pointer_cast<component::PerspectiveCamera>(component);
    std::stringstream   buffer;
    std::vector<float>  data;

    data.push_back(perspectiveCamera->aspectRatio());
    data.push_back(perspectiveCamera->fieldOfView());
    data.push_back(perspectiveCamera->zNear());
    data.push_back(perspectiveCamera->zFar());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}


std::string
ComponentSerializer::serializeAmbientLight(NodePtr              node,
                                           AbstractComponentPtr component,
                                           DependencyPtr        dependencies)
{
    int8_t              type        = serialize::AMBIENT_LIGHT;
    auto                ambient     = std::static_pointer_cast<component::AmbientLight>(component);
    std::stringstream   buffer;
    std::vector<float>  data;

    data.push_back(ambient->ambient());
    data.push_back(ambient->color()->x());
    data.push_back(ambient->color()->y());
    data.push_back(ambient->color()->z());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeDirectionalLight(NodePtr              node,
                                               AbstractComponentPtr component,
                                               DependencyPtr        dependencies)
{
    int8_t                                          type        = serialize::DIRECTIONAL_LIGHT;
    auto    directional = std::static_pointer_cast<component::DirectionalLight>(component);
    std::stringstream                               buffer;
    std::vector<float>                              data;

    data.push_back(directional->diffuse());
    data.push_back(directional->specular());
    data.push_back(directional->color()->x());
    data.push_back(directional->color()->y());
    data.push_back(directional->color()->z());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}


std::string
ComponentSerializer::serializePointLight(NodePtr                node,
                                         AbstractComponentPtr   component,
                                         DependencyPtr          dependencies)
{
    int8_t              type    = serialize::POINT_LIGHT;
    auto                point   = std::static_pointer_cast<component::PointLight>(component);
    std::stringstream   buffer;
    std::vector<float>  data;

    data.push_back(point->diffuse());
    data.push_back(point->specular());
    data.push_back(point->attenuationCoefficients()->x());
    data.push_back(point->attenuationCoefficients()->y());
    data.push_back(point->attenuationCoefficients()->z());
    data.push_back(point->color()->x());
    data.push_back(point->color()->y());
    data.push_back(point->color()->z());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeSpotLight(NodePtr                 node,
                                        AbstractComponentPtr    component,
                                        DependencyPtr           dependencies)
{
    int8_t              type    = serialize::SPOT_LIGHT;
    auto                spot    = std::static_pointer_cast<component::SpotLight>(component);
    std::stringstream   buffer;
    std::vector<float>  data;

    data.push_back(spot->diffuse());
    data.push_back(spot->specular());
    data.push_back(spot->attenuationCoefficients()->x());
    data.push_back(spot->attenuationCoefficients()->y());
    data.push_back(spot->attenuationCoefficients()->z());
    data.push_back(acos(spot->cosInnerConeAngle()));
    data.push_back(acos(spot->cosOuterConeAngle()));
    data.push_back(spot->color()->x());
    data.push_back(spot->color()->y());
    data.push_back(spot->color()->z());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeSurface(NodePtr               node,
                                      AbstractComponentPtr  component,
                                      DependencyPtr         dependencies)
{
    int8_t              type    = serialize::SURFACE;
    auto                surface = std::static_pointer_cast<component::Surface>(component);
    std::stringstream   buffer;

    uint materialId = dependencies->registerDependency(surface->material());
    uint geometryId = dependencies->registerDependency(surface->geometry());
    uint effectId   = dependencies->registerDependency(surface->effect());

    msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string> src(
        geometryId,
        materialId,
        effectId,
        getSurfaceExtension(node, surface));

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::getSurfaceExtension(NodePtr node, SurfacePtr surface)
{
    std::vector<SimpleProperty> properties;

    std::string technique = surface->technique();

    if (surface->technique() != "default")
        properties.push_back(serializeSimpleProperty(std::string("technique"), technique));

    if (!surface->visible())
        properties.push_back(serializeSimpleProperty(std::string("visible"), surface->visible()));

    std::stringstream buffer;

    msgpack::pack(buffer, SimplePropertyVector(properties));

    return buffer.str();
}

std::string
ComponentSerializer::serializeRenderer(NodePtr              node,
                                       AbstractComponentPtr component,
                                       DependencyPtr        dependencies)
{
    int8_t              type        = serialize::RENDERER;
    auto                renderer    = std::static_pointer_cast<component::Renderer>(component);
    std::stringstream   buffer;

    msgpack::type::tuple<unsigned int> src(renderer->backgroundColor());

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}


std::string
ComponentSerializer::serializeBoundingBox(NodePtr               node,
                                          AbstractComponentPtr  component,
                                          DependencyPtr         dependencies)
{
    auto                boundingBox = std::static_pointer_cast<component::BoundingBox>(component);
    math::Box::Ptr      box         = boundingBox->box();
    math::Vector3::Ptr  topRight    = box->topRight();
    math::Vector3::Ptr  bottomLeft  = box->bottomLeft();
    int8_t              type        = serialize::BOUNDINGBOX;
    std::stringstream   buffer;
    std::vector<float>  data;

    float centerX = (topRight->x() + bottomLeft->x()) / 2.0f;
    float centerY = (topRight->y() + bottomLeft->y()) / 2.0f;
    float centerZ = (topRight->z() + bottomLeft->z()) / 2.0f;

    data.push_back(centerX);
    data.push_back(centerY);
    data.push_back(centerZ);
    data.push_back(box->width());
    data.push_back(box->height());
    data.push_back(box->depth());

    std::string src = serialize::TypeSerializer::serializeVector<float>(data);

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}
