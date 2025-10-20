#include "model.h"
#include "fastgltf/types.hpp"
#include "types.h"
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/util.hpp>
#include <filesystem>
#include <fmt/base.h>
#include <iterator>
#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

MaterialData ParseMaterialData(fastgltf::Material &material,
                               std::span<size_t> textures,
                               std::span<std::string> images) {
  MaterialData new_material;
  new_material.albedo =
      material.pbrData.baseColorTexture.has_value()
          ? images[textures[material.pbrData.baseColorTexture->textureIndex]]
          : "";
  new_material.ambient_occlusion =
      material.occlusionTexture.has_value()
          ? images[textures[material.occlusionTexture->textureIndex]]
          : "";
  new_material.normal =
      material.normalTexture.has_value()
          ? images[textures[material.normalTexture->textureIndex]]
          : "";
  new_material.emissive =
      material.emissiveTexture.has_value()
          ? images[textures[material.emissiveTexture->textureIndex]]
          : "";
  new_material.metal_roughness =
      material.pbrData.metallicRoughnessTexture.has_value()
          ? images[textures[material.pbrData.metallicRoughnessTexture
                                ->textureIndex]]
          : "";
  return new_material;
}

std::vector<MeshData> ParseModel(std::string path) {
  std::filesystem::path file_path = MODEL_DIR + path;

  fastgltf::Parser parser;

  auto data = fastgltf::GltfDataBuffer::FromPath(file_path);

  if (data.error() != fastgltf::Error::None) {
    fmt::println("{}", static_cast<uint64_t>(data.error()));
    fmt::println("[ERROR] gltf model {} failed to load", path);
    abort();
  }

  fastgltf::Options load_options =
      fastgltf::Options::LoadExternalBuffers |
      fastgltf::Options::DontRequireValidAssetMember;

  auto asset =
      parser.loadGltf(data.get(), file_path.parent_path(), load_options);

  if (asset.error() != fastgltf::Error::None) {
    fmt::println("[ERROR] gltf model {} failed to parse asset", path);
    abort();
  }

  std::vector<std::string> images;
  images.reserve(asset->images.size());

  for (fastgltf::Image &image : asset->images) {
    std::visit(fastgltf::visitor{
                   [](auto &arg) {},
                   [&](fastgltf::sources::URI &image_file_path) {
                     assert(image_file_path.fileByteOffset == 0);
                     assert(image_file_path.uri.isLocalPath());

                     const std::string image_path(
                         image_file_path.uri.path().begin(),
                         image_file_path.uri.path().end());

                     images.push_back(file_path.parent_path().string() + "/" +
                                      image_path);
                   },
               },
               image.data);
  }

  std::vector<size_t> textures;
  textures.reserve(asset->textures.size());
  for (fastgltf::Texture texture : asset->textures) {
    textures.push_back(texture.imageIndex.value());
  }

  std::vector<MaterialData> material_datas;
  material_datas.reserve(asset->materials.size());

  for (fastgltf::Material &material : asset->materials) {
    material_datas.push_back(ParseMaterialData(material, textures, images));
  }

  std::vector<MeshData> mesh_data;
  std::vector<size_t> unique_check_sums;

  for (fastgltf::Mesh &mesh : asset->meshes) {
    mesh_data.reserve(mesh.primitives.size() + mesh_data.size());
    unique_check_sums.reserve(mesh.primitives.size() +
                              unique_check_sums.size());

    for (auto &&p : mesh.primitives) {
      size_t check_sum = 0;
      std::vector<uint32_t> indices;
      std::vector<Vertex> vertices;

      auto &index_accessor = asset->accessors[p.indicesAccessor.value()];
      indices.reserve(index_accessor.count);

      fastgltf::iterateAccessor<std::uint32_t>(asset.get(), index_accessor,
                                               [&](std::uint32_t index) {
                                                 indices.push_back(index);
                                                 check_sum += index;
                                               });

      auto &position_accessor =
          asset->accessors[p.findAttribute("POSITION")->accessorIndex];

      vertices.resize(vertices.size() + position_accessor.count);

      fastgltf::iterateAccessorWithIndex<glm::vec3>(
          asset.get(), position_accessor,
          [&](glm::vec3 position, size_t index) {
            Vertex vertex;
            vertex.position = position;
            vertex.normal = {1, 0, 0};
            vertex.uv_x = 0;
            vertex.uv_y = 0;
            vertices[index] = vertex;
          });

      auto normals = p.findAttribute("NORMAL");
      if (normals != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec3>(
            asset.get(), asset->accessors[normals->accessorIndex],
            [&](glm::vec3 normal, std::size_t index) {
              check_sum += normal.x + normal.y + normal.z;
              vertices[index].normal = normal;
            });
      }

      auto uv = p.findAttribute("TEXCOORD_0");
      if (uv != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec2>(
            asset.get(), asset->accessors[uv->accessorIndex],
            [&](glm::vec2 uv, std::size_t index) {
              vertices[index].uv_x = uv.x;
              vertices[index].uv_y = uv.y;
            });
      }

      MaterialData material_data{};
      if (p.materialIndex.has_value()) {
        material_data = material_datas[p.materialIndex.value()];
        check_sum += p.materialIndex.value();
      }

      assert(unique_check_sums.size() == mesh_data.size());

      auto it = std::find(unique_check_sums.begin(), unique_check_sums.end(),
                          check_sum);

      glm::vec3 centroid = glm::vec3(0.0);

      for (const Vertex &vertex : vertices) {
        centroid += vertex.position;
      }

      centroid /= static_cast<float>(vertices.size());

      glm::mat4 new_instance = glm::translate(glm::mat4(1.0f), centroid);

      if (it != unique_check_sums.end()) {
        size_t mesh_index = std::distance(unique_check_sums.begin(), it);

        mesh_data[mesh_index].instances.push_back(new_instance);
      } else {
        unique_check_sums.push_back(check_sum);

        for (auto &vertex : vertices) {
          vertex.position -= centroid;
        }

        std::vector<glm::mat4> instances = {new_instance};

        mesh_data.push_back({
            .material_data = material_data,
            .vertices = vertices,
            .indices = indices,
            .instances = instances,
        });
      }
    }
  }

  return mesh_data;
}
