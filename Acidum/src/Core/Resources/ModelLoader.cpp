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

void ModelLoader::processNode(const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4& parentMatrix, std::vector<MeshData>& allMeshes) {
    glm::mat4 localMatrix = glm::mat4(1.0f);

    if (node.matrix.size() == 16)
        localMatrix = glm::make_mat4(node.matrix.data());
    else {
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);

        if (node.translation.size() == 3)
            translation = glm::translate(glm::mat4(1.0f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));

        if (node.rotation.size() == 4) {
            glm::quat q(
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2])
            );

            rotation = glm::mat4_cast(q);
        }

        if (node.scale.size() == 3)
            scale = glm::scale(glm::mat4(1.0f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));

        localMatrix = translation * rotation * scale;
    }

    glm::mat4 globalMatrix = parentMatrix * localMatrix;
    
    if (node.mesh >= 0) {
        const tinygltf::Mesh& mesh = model.meshes[static_cast<size_t>(node.mesh)];
        for (const auto& primitive : mesh.primitives) {
            MeshData meshData {};
            meshData.name = node.name;

            if (primitive.material >= 0) {
                const tinygltf::Material& mat = model.materials[static_cast<size_t>(primitive.material)];

                if (mat.alphaMode == "BLEND")
                    meshData.isTransparent = true;
                meshData.roughness = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
                meshData.metallic = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);

                std::vector<double> factor = mat.pbrMetallicRoughness.baseColorFactor;
                if (factor.size() == 4)
                    meshData.baseColorFactor = glm::vec4(factor[0], factor[1], factor[2], factor[3]);

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

                int pbrTexIndex = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
                if (pbrTexIndex >= 0) {
                    int source = model.textures[static_cast<size_t>(pbrTexIndex)].source;
                    if (source >= 0) {
                        const tinygltf::Image& image = model.images[static_cast<size_t>(source)];

                        if (!image.uri.empty())
                            meshData.metallicRoughnessTextureName = image.uri; 
                        else if (image.bufferView >= 0) {
                            const tinygltf::BufferView& bv = model.bufferViews[static_cast<size_t>(image.bufferView)];
                            const tinygltf::Buffer& buffer = model.buffers[static_cast<size_t>(bv.buffer)];

                            meshData.embeddedMetallicRoughnessImage.assign(
                                buffer.data.begin() + static_cast<ptrdiff_t>(bv.byteOffset), 
                                buffer.data.begin() + static_cast<ptrdiff_t>(bv.byteOffset) + static_cast<ptrdiff_t>(bv.byteLength)
                            );
                        }
                    }
                }

                int normTexIndex = mat.normalTexture.index;
                if (normTexIndex >= 0) {
                    int source = model.textures[static_cast<size_t>(normTexIndex)].source;
                    if (source >= 0) {
                        const tinygltf::Image& image = model.images[static_cast<size_t>(source)];

                        if (!image.uri.empty())
                            meshData.normalTextureName = image.uri; 
                        else if (image.bufferView >= 0) {
                            const tinygltf::BufferView& bv = model.bufferViews[static_cast<size_t>(image.bufferView)];
                            const tinygltf::Buffer& buffer = model.buffers[static_cast<size_t>(bv.buffer)];

                            meshData.embeddedNormalImage.assign(
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

            size_t tanStride = 0;
            const uint8_t* tanData = nullptr;
            if (primitive.attributes.contains("TANGENT")) {
                const auto& tanAcc = model.accessors[static_cast<size_t>(primitive.attributes.at("TANGENT"))];
                const auto& tanView = model.bufferViews[static_cast<size_t>(tanAcc.bufferView)];
                tanData = model.buffers[static_cast<size_t>(tanView.buffer)].data.data() + tanView.byteOffset + tanAcc.byteOffset;
                tanStride = static_cast<size_t>(tanAcc.ByteStride(tanView));
            }

            meshData.transform = globalMatrix;
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

                if (tanData) {
                    const auto* pTan = reinterpret_cast<const float*>(tanData + (i * tanStride));
                    vertex.tangent = glm::make_vec4(pTan);
                } else
                    vertex.tangent = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }

            allMeshes.push_back(meshData);
        }
    }

    for (int childIndex : node.children)
        processNode(model.nodes[static_cast<size_t>(childIndex)], model, globalMatrix, allMeshes);
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

    if (!warn.empty()) ACIDUM_WARN("TinyGLTF warning: {}", warn);
    ACIDUM_ASSERT(result, "Failed to load model {} via tinyGLTF: {}", path, error);

    std::vector<MeshData> allMeshes;

    int sceneIndex = model.defaultScene > -1 ? model.defaultScene : 0;
    const tinygltf::Scene& scene = model.scenes[static_cast<size_t>(sceneIndex)];

    for (int nodeIndex : scene.nodes)
        processNode(model.nodes[static_cast<size_t>(nodeIndex)], model, glm::mat4(1.0f), allMeshes);

    return allMeshes;
}

} // namespace Acidum