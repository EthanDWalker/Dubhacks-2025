#pragma once

#include "Backend/context.h"
#include "webgpu/webgpu_cpp.h"

struct AllocatedBuffer {
  wgpu::Buffer obj;
  size_t size;

  void Destroy() {
    obj.Destroy();
  }
};

AllocatedBuffer CreateBuffer(BackendContext &context, wgpu::BufferUsage usage, size_t size);

void wgpuPollEvents([[maybe_unused]] wgpu::Device device, [[maybe_unused]] bool yieldToWebBrowser);
