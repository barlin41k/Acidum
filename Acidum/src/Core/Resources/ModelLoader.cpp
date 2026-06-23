#include "Acidum/Core/Resources/ModelLoader.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wall"
    #pragma GCC diagnostic ignored "-Wextra"
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wshadow"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#if defined(_MSC_VER)
    #pragma warning(push, 0)
#endif

#include <tiny_gltf.h>

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#include <glm/gtc/type_ptr.hpp>

#include "Acidum/Core/Base/Logger.hpp"

namespace Acidum {

bool DummyImageLoader(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*) {
    return true;
}
    
std::vector<MeshData> ModelLoader::load(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error, warn;

    loader.SetImageLoader(DummyImageLoader, nullptr);
    
    bool result = false;
    if (path.ends_with(".glb"))
        result = loader.LoadBinaryFromFile(&model, &error, &warn, path);
    else
        result = loader.LoadASCIIFromFile(&model, &error, &warn, path);

    if (!warn.empty()) ENGINE_WARN("TinyGLTF warning: {}", warn);
    ENGINE_VERIFY(result, "Failed to load model {} via tinyGLTF: {}", path, error);

    std::vector<MeshData> allMeshes;
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            MeshData meshData {};

            if (primitive.material >= 0) {
                const tinygltf::Material& mat = model.materials[static_cast<size_t>(primitive.material)];

                if (mat.alphaMode == "BLEND")
                    meshData.isTransparent = true;

                int texIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
                if (texIndex >= 0) {
                    int source = model.textures[static_cast<size_t>(texIndex)].source;
                    if (source >= 0) {
                        const tinygltf::Image& image = model.images[static_cast<size_t>(source)];

                        if (!image.uri.empty())
                            meshData.textureName = image.uri;
                        else if (image.bufferView >= 0) {
                            const tinygltf::BufferView& bv = model.bufferViews[static_cast<size_t>(image.bufferView)];
                            const tinygltf::Buffer& buffer = model.buffers[static_cast<size_t>(bv.buffer)];

                            meshData.embeddedImage.assign(
                            buffer.data.begin() + static_cast<ptrdiff_t>(bv.byteOffset), 
                            buffer.data.begin() + static_cast<ptrdiff_t>(bv.byteOffset) + static_cast<ptrdiff_t>(bv.byteLength)
                            );
                        }
                    }
                }
            }

            const tinygltf::Accessor& indexAccessor = model.accessors[static_cast<size_t>(primitive.indices)];
            const tinygltf::BufferView& indexBufferView = model.bufferViews[static_cast<size_t>(indexAccessor.bufferView)];
            const tinygltf::Buffer& indexBuffer = model.buffers[static_cast<size_t>(indexBufferView.buffer)];
            const uint8_t* indexData = indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset;

            for (size_t i = 0; i < indexAccessor.count; i++) {
                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    meshData.indices.push_back(reinterpret_cast<const uint8_t*>(indexData)[i]);
                else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                    meshData.indices.push_back(reinterpret_cast<const uint16_t*>(indexData)[i]);
                else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                    meshData.indices.push_back(reinterpret_cast<const uint32_t*>(indexData)[i]);
            }

            auto posIt = primitive.attributes.find("POSITION");
            ENGINE_VERIFY(posIt != primitive.attributes.end(), "Model {} has no positions!", path);

            const tinygltf::Accessor& posAccessor = model.accessors[static_cast<size_t>(posIt->second)];
            const tinygltf::BufferView& posBufferView = model.bufferViews[static_cast<size_t>(posAccessor.bufferView)];
            const tinygltf::Buffer& posBuffer = model.buffers[static_cast<size_t>(posBufferView.buffer)];
            const uint8_t* posData = posBuffer.data.data() + posBufferView.byteOffset + posAccessor.byteOffset;
            const size_t posStride = static_cast<size_t>(posAccessor.ByteStride(posBufferView));

            size_t normStride = 0;
            const uint8_t* normData = nullptr;
            if (primitive.attributes.contains("NORMAL")) {
                const auto& normAcc = model.accessors[static_cast<size_t>(primitive.attributes.at("NORMAL"))];
                const auto& normView = model.bufferViews[static_cast<size_t>(normAcc.bufferView)];
                normData = model.buffers[static_cast<size_t>(normView.buffer)].data.data() + normView.byteOffset + normAcc.byteOffset;
                normStride = static_cast<size_t>(normAcc.ByteStride(normView));
            }

            size_t texStride = 0;
            const uint8_t* texData = nullptr;
            if (primitive.attributes.contains("TEXCOORD_0")) {
                const auto& texAcc = model.accessors[static_cast<size_t>(primitive.attributes.at("TEXCOORD_0"))];
                const auto& texView = model.bufferViews[static_cast<size_t>(texAcc.bufferView)];
                texData = model.buffers[static_cast<size_t>(texView.buffer)].data.data() + texView.byteOffset + texAcc.byteOffset;
                texStride = static_cast<size_t>(texAcc.ByteStride(texView));
            }

            
            meshData.vertices.resize(posAccessor.count);
            for (size_t i = 0; i < posAccessor.count; i++) {
                Vertex& vertex = meshData.vertices[i];

                const auto* pPos = reinterpret_cast<const float*>(posData + (i * posStride));
                vertex.pos = glm::make_vec3(pPos);

                vertex.color = glm::vec3(1.0f);

                if (normData) {
                    const auto* pNorm = reinterpret_cast<const float*>(normData + (i * normStride));
                    vertex.normal = glm::make_vec3(pNorm);
                } else
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);

                if (texData) {
                    const auto* pTex = reinterpret_cast<const float*>(texData + (i * texStride));
                    vertex.texCoord = glm::make_vec2(pTex);
                } else
                    vertex.texCoord = glm::vec2(0.0f);
            }

            allMeshes.push_back(meshData);
        }
    }

    return allMeshes;
}

} // namespace Acidum