#include "texture.h"
#include "glm/ext/vector_uint3_sized.hpp"
#include "webgpu/webgpu_cpp.h"

AllocatedTexture CreateTexture(BackendContext &context,
                               wgpu::TextureFormat format,
                               wgpu::TextureDimension dimensions,
                               wgpu::TextureViewDimension view_dimensions,
                               wgpu::TextureAspect aspect, glm::u32vec3 size,
                               wgpu::TextureUsage usage) {
  AllocatedTexture texture;
  wgpu::TextureDescriptor desc{};
  desc.format = format;
  desc.size = {size.x, size.y, size.z};
  desc.dimension = dimensions;
  desc.usage = usage;
  desc.viewFormatCount = 1;
  desc.viewFormats = &format;
  texture.obj = context.device.CreateTexture(&desc);

  wgpu::TextureViewDescriptor view_desc{};
  view_desc.format = format;
  view_desc.aspect = aspect;
  view_desc.usage = usage;
  view_desc.dimension = view_dimensions;
  texture.view = texture.obj.CreateView(&view_desc);
  texture.format = format;

  return texture;
}
