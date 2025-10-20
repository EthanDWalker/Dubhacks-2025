#pragma once
#include "Backend/binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Frustum {
  glm::vec4 top;
  glm::vec4 bottom;
  glm::vec4 right;
  glm::vec4 left;
  glm::vec4 far;
  glm::vec4 near;
};

struct CameraBuffer {
  Frustum frustum;
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
  glm::mat4 inv_view;
  glm::mat4 inv_proj;
  glm::vec3 view_pos;
  float _p0;
  float near;
  float far;
  glm::vec2 _p1;
};

struct Camera {
  CameraBuffer buffer_data;
  AllocatedBuffer buffer;

  BindGroup bind_group;
  glm::vec3 velocity;
  glm::vec3 position;

  float pitch{0.0f};
  float yaw{0.0f};

  void Create(BackendContext &context);

  void Update(BackendContext &context, float delta_time);

  void Destroy() {
    buffer.Destroy();
  }
};
