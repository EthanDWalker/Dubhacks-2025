#include "scene.h"
#include "Backend/binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include "types.h"
#include "webgpu/webgpu_cpp.h"
#include <span>

AABB GetAabbBounds(std::span<Vertex> vertices) {
  AABB aabb{};
  aabb.min = glm::vec3(std::numeric_limits<float>::max());
  aabb.max = glm::vec3(std::numeric_limits<float>::min());
  for (const auto &vertex : vertices) {
    glm::vec3 position = vertex.position;
    aabb.min = glm::min(aabb.min, position);
    aabb.max = glm::max(aabb.max, position);
  }
  return aabb;
}

void Scene::Init(BackendContext &context) {
  instance_buffer = CreateBuffer(
      context, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
      sizeof(Instance) * MAX_INSTANCES);
  object_buffer = CreateBuffer(
      context, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
      sizeof(Object) * MAX_OBJECTS);

  scene_info_buffer = CreateBuffer(
      context, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
      sizeof(SceneInfo));

  index_buffer =
      CreateBuffer(context,
                   wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst |
                       wgpu::BufferUsage::Index,
                   sizeof(uint32_t) * MAX_INDICES);
  vertex_buffer = CreateBuffer(
      context, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
      sizeof(Vertex) * MAX_VERTICES);

  aabb_buffer = CreateBuffer(
      context, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
      sizeof(AABB) * MAX_OBJECTS);

  {
    const bool read_only = false;
    BindGroupBuilder bind_group_builder{};
    bind_group_builder.AddStorageBuffer(object_buffer, read_only);
    bind_group_builder.AddStorageBuffer(aabb_buffer, read_only);
    bind_group_builder.AddStorageBuffer(vertex_buffer, read_only);
    bind_group_builder.AddStorageBuffer(index_buffer, read_only);
    bind_group_builder.AddUniformBuffer(scene_info_buffer);
    object_bind_group =
        bind_group_builder.Build(context, wgpu::ShaderStage::Compute);
  }

  {
    const bool read_only = false;
    BindGroupBuilder bind_group_builder{};
    bind_group_builder.AddStorageBuffer(instance_buffer, read_only);
    instance_bind_group =
        bind_group_builder.Build(context, wgpu::ShaderStage::Compute);
  }
  {
    const bool read_only = true;
    BindGroupBuilder bind_group_builder{};
    bind_group_builder.AddStorageBuffer(object_buffer, read_only);
    bind_group_builder.AddStorageBuffer(aabb_buffer, read_only);
    bind_group_builder.AddStorageBuffer(vertex_buffer, read_only);
    bind_group_builder.AddStorageBuffer(index_buffer, read_only);
    bind_group_builder.AddUniformBuffer(scene_info_buffer);
    object_bind_group_read = bind_group_builder.Build(
        context, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment |
                     wgpu::ShaderStage::Compute);
  }

  {
    const bool read_only = true;
    BindGroupBuilder bind_group_builder{};
    bind_group_builder.AddStorageBuffer(instance_buffer, read_only);
    instance_bind_group_read = bind_group_builder.Build(
        context, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment |
                     wgpu::ShaderStage::Compute);
  }
}

uint32_t Scene::AddObject(BackendContext &context, std::vector<Vertex> vertices,
                          std::vector<uint32_t> indices) {
  Object new_object{};
  new_object.first_vertex = vertex_index;

  context.device.GetQueue().WriteBuffer(object_buffer.obj,
                                        sizeof(Object) * object_index,
                                        &new_object, sizeof(Object));

  AABB aabb = GetAabbBounds(vertices);
  context.device.GetQueue().WriteBuffer(
      aabb_buffer.obj, sizeof(AABB) * object_index, &aabb, sizeof(AABB));

  context.device.GetQueue().WriteBuffer(
      index_buffer.obj, sizeof(uint32_t) * indice_index, indices.data(),
      sizeof(uint32_t) * indices.size());

  context.device.GetQueue().WriteBuffer(
      vertex_buffer.obj, sizeof(Vertex) * vertex_index, vertices.data(),
      sizeof(Vertex) * vertices.size());

  vertex_index += vertices.size();
  indice_index += indices.size();
  return object_index++;
}

uint32_t Scene::AddInstance(BackendContext &context, Instance *instance) {
  context.device.GetQueue().WriteBuffer(instance_buffer.obj,
                                        sizeof(Instance) * instance_index,
                                        instance, sizeof(Instance));
  return instance_index++;
}

void Scene::UpdateSceneInfo(BackendContext &context) {
  SceneInfo info{};
  info.index_count = indice_index;
  info.object_count = object_index;
  info.instance_count = instance_index;

  context.device.GetQueue().WriteBuffer(scene_info_buffer.obj, 0, &info,
                                        sizeof(SceneInfo));
}

void Scene::Reset(BackendContext &context) {
  indice_index = 0;
  object_index = 0;
  instance_index = 0;
  vertex_index = 0;

  UpdateSceneInfo(context);
}

void Scene::Destroy() {
  instance_buffer.Destroy();
  aabb_buffer.Destroy();
  object_buffer.Destroy();
  index_buffer.Destroy();
  vertex_buffer.Destroy();
}
