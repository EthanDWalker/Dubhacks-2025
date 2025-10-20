#include "context.h"
#include "webgpu/webgpu_cpp.h"
#include "webgpu/webgpu_glfw.h"
#include <cassert>
#include <dawn/webgpu_cpp_print.h>
#include <iostream>

void BackendContext::Init(GLFWwindow *window, int32_t width, int32_t height) {
  static const auto k_timed_wait_any = wgpu::InstanceFeatureName::TimedWaitAny;
  wgpu::InstanceDescriptor instance_desc;
  instance_desc.requiredFeatureCount = 1;
  instance_desc.requiredFeatures = &k_timed_wait_any;
  this->instance = wgpu::CreateInstance(&instance_desc);

  wgpu::Future adapter_future = this->instance.RequestAdapter(
      nullptr, wgpu::CallbackMode::WaitAnyOnly,
      [&](wgpu::RequestAdapterStatus status, wgpu::Adapter a,
          wgpu::StringView message) {
        if (status != wgpu::RequestAdapterStatus::Success) {
          std::cout << "RequestAdapter: " << message << "\n";
          exit(0);
        }
        this->adapter = std::move(a);
      });

  this->instance.WaitAny(adapter_future, UINT64_MAX);

  wgpu::DeviceDescriptor device_desc{};
  device_desc.SetUncapturedErrorCallback([](const wgpu::Device &,
                                            wgpu::ErrorType errorType,
                                            wgpu::StringView message) {
    std::cout << "Error: " << errorType << " - message: " << message << "\n";
  });

  wgpu::Future device_future = this->adapter.RequestDevice(
      &device_desc, wgpu::CallbackMode::WaitAnyOnly,
      [&](wgpu::RequestDeviceStatus status, wgpu::Device d,
          wgpu::StringView message) {
        if (status != wgpu::RequestDeviceStatus::Success) {
          std::cout << "RequestDevice: " << message << "\n";
          exit(0);
        }
        this->device = std::move(d);
      });

  this->instance.WaitAny(device_future, UINT64_MAX);

  this->surface = wgpu::glfw::CreateSurfaceForWindow(this->instance, window);

  wgpu::SurfaceCapabilities capabilities;
  this->surface.GetCapabilities(this->adapter, &capabilities);
  this->surface_format = capabilities.formats[0];

  wgpu::SurfaceConfiguration surface_config{};
  surface_config.device = this->device;
  surface_config.format = this->surface_format;
  surface_config.width = width;
  surface_config.height = height;
  surface_config.presentMode = wgpu::PresentMode::Fifo;
  this->surface.Configure(&surface_config);
}
