#include "Acidum/Core/Resources/ModelLoader.hpp"

#include <cstddef>
#include <tiny_obj_loader.h>

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {
    
MeshData ModelLoader::load(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    ENGINE_VERIFY(ret, "Failed to load model: ", err);

    MeshData meshData {};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex {};

            vertex.pos = {
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0],
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1],
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (index.texcoord_index >= 0)
                vertex.texCoord = {
                    attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0],
                    1.0f - attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1] // TODO: Move flip UV to IGraphicsAPI settings
                };

            if (index.normal_index >= 0)
                vertex.normal = {
                    attrib.normals[3 * static_cast<size_t>(index.normal_index) + 0],
                    attrib.normals[3 * static_cast<size_t>(index.normal_index) + 1],
                    attrib.normals[3 * static_cast<size_t>(index.normal_index) + 2]
                };
            else
                vertex.normal = {0.0f, 1.0f, 0.0f};

            meshData.vertices.push_back(vertex);
            meshData.indices.push_back(static_cast<uint32_t>(meshData.indices.size()));
        }
    }

    return meshData;
}

} // namespace Acidum