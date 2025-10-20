#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

struct alignas(16) Vertex {
  glm::vec3 position;
  float uv_x;
  glm::vec3 normal;
  float uv_y;
};

struct alignas(16) Object {
  uint32_t first_vertex;
};

struct alignas(16) AABB {
  glm::vec3 min;
  glm::vec3 max;
};

struct alignas(16) Instance {
  glm::mat4 matrix;
  glm::vec3 velocity;
  uint object_id;
  glm::vec3 acceleration;
  uint _p0;
  glm::vec3 color;
  uint _p1;
};
