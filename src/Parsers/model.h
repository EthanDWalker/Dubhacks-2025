#pragma once

#include "types.h"
#include <glm/vec3.hpp>
#include <string>
#include <vector>

const std::string MODEL_DIR = "../assets/models/";

struct MaterialData {
  std::string albedo;
  std::string metal_roughness;
  std::string emissive;
  std::string normal;
  std::string ambient_occlusion;
};

struct MeshData {
  MaterialData material_data;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<glm::mat4> instances;
};

std::vector<MeshData> ParseModel(std::string path);
