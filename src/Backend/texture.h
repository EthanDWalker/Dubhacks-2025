#pragma once

#include "context.h"
#include "glm/ext/vector_uint3_sized.hpp"
#include "webgpu/webgpu_cpp.h"

struct AllocatedTexture {
  wgpu::Texture obj;
  wgpu::TextureView view;
  wgpu::TextureFormat format;

  void Destroy() { obj.Destroy(); }
};

AllocatedTexture CreateTexture(BackendContext &context,
                               wgpu::TextureFormat format,
                               wgpu::TextureDimension dimensions,
                               wgpu::TextureViewDimension view_dimensions,
                               wgpu::TextureAspect aspect, glm::u32vec3 size,
                               wgpu::TextureUsage usage);
