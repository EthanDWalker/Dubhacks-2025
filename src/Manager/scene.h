#pragma once

#include "Backend/binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "types.h"
#include <cstdint>

const uint32_t MAX_INSTANCES = 200;
const uint32_t MAX_OBJECTS = 100;
const uint32_t MAX_INDICES = 200'000;
const uint32_t MAX_VERTICES = 100'000;

struct SceneInfo {
  uint32_t object_count;
  uint32_t instance_count;
  uint32_t index_count;
};

struct Scene {
  AllocatedBuffer instance_buffer;

  AllocatedBuffer object_buffer;
  AllocatedBuffer aabb_buffer;

  AllocatedBuffer index_buffer;
  AllocatedBuffer vertex_buffer;

  AllocatedBuffer scene_info_buffer;

  BindGroup object_bind_group;
  BindGroup instance_bind_group;

  BindGroup object_bind_group_read;
  BindGroup instance_bind_group_read;

  uint32_t object_index;
  uint32_t instance_index;
  uint32_t indice_index;
  uint32_t vertex_index;

  void Init(BackendContext &context);

  uint32_t AddObject(BackendContext &context, std::vector<Vertex> vertices,
                     std::vector<uint32_t> indices);

  uint32_t AddInstance(BackendContext &context, Instance *instance);

  void UpdateSceneInfo(BackendContext &context);

  void Reset(BackendContext &context);

  void Destroy();
};
