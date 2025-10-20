#pragma once

#include "Backend/binding.h"
#include "context.h"
#include <webgpu/webgpu_cpp.h>

struct GraphicsPipeline {
  wgpu::RenderPipeline obj;
  wgpu::PipelineLayout layout;
};

struct ComputePipeline {
  wgpu::ComputePipeline obj;
  wgpu::PipelineLayout layout;
};

struct GraphicsPipelineBuilder {
  wgpu::ShaderModule shader_module;
  wgpu::DepthStencilState depth_state{};

  std::vector<wgpu::BindGroupLayout> bind_layouts;

  void SetShader(BackendContext &context, std::string file_path);

  void SetDepthStencil(wgpu::CompareFunction compare, bool write,
                       wgpu::TextureFormat format);

  void AddBindGroup(BindGroup &bind_group) {
    bind_layouts.push_back(bind_group.layout);
  }

  GraphicsPipeline Build(BackendContext &context);
};

struct ComputePipelineBuilder {
  wgpu::ShaderModule shader_module;

  std::vector<wgpu::BindGroupLayout> bind_layouts;

  void SetShader(BackendContext &context, std::string file_path);

  void AddBindGroup(BindGroup &bind_group) {
    bind_layouts.push_back(bind_group.layout);
  }

  ComputePipeline Build(BackendContext &context);
};
