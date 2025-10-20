#pragma once

#include "Backend/buffer.h"
#include <webgpu/webgpu_cpp.h>

struct BindGroup {
  wgpu::BindGroup obj;
  wgpu::BindGroupLayout layout;

  void Destroy();
};

struct BindGroupBuilder {
  std::vector<wgpu::BindGroupLayoutEntry> layout_entries;
  std::vector<wgpu::BindGroupEntry> entries;

  void AddUniformBuffer(AllocatedBuffer &buffer);
  void AddStorageBuffer(AllocatedBuffer &buffer, bool read_only = false);

  BindGroup Build(BackendContext &context, wgpu::ShaderStage visibility);
};
