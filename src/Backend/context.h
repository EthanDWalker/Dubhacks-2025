#pragma once
#include "webgpu/webgpu_cpp.h"
#include "webgpu/webgpu_glfw.h"

struct BackendContext {
  wgpu::Instance instance;
  wgpu::Adapter adapter;
  wgpu::Device device;
  wgpu::Surface surface;
  wgpu::TextureFormat surface_format;

  void Init(GLFWwindow *window, int32_t width, int32_t height);
};
