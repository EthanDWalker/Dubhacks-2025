#include "buffer.h"
#include "context.h"
#include "webgpu/webgpu_cpp.h"

void wgpuPollEvents([[maybe_unused]] wgpu::Device device,
                    [[maybe_unused]] bool yield_browser) {
#if defined(WEBGPU_BACKEND_DAWN)
  device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
  device.poll(false);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
  if (yield_browser) {
    emscripten_sleep(100);
  }
#endif
}

AllocatedBuffer CreateBuffer(BackendContext &context,
                             wgpu::BufferUsage usage, size_t size) {
  AllocatedBuffer buffer;
  wgpu::BufferDescriptor desc{};
  desc.size = size;
  desc.usage = usage;
  desc.mappedAtCreation = false;

  buffer.obj = context.device.CreateBuffer(&desc);
  buffer.size = size;

  return buffer;
}
