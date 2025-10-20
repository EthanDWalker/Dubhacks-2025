#include "pipeline.h"
#include "context.h"
#include "webgpu/webgpu_cpp.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>

const char *SHADER_DIR = "../assets/shaders/";

void ComputePipelineBuilder::SetShader(BackendContext &context,
                                       std::string file_path) {

  std::ifstream file(SHADER_DIR + file_path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    assert(false && "failed to load shader");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size + 1, '\0');

  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();

  wgpu::ShaderSourceWGSL source{{.code = buffer.data()}};
  wgpu::ShaderModuleDescriptor descriptor{};
  descriptor.nextInChain = &source;
  shader_module = context.device.CreateShaderModule(&descriptor);
}

void GraphicsPipelineBuilder::SetShader(BackendContext &context,
                                        std::string file_path) {
  std::ifstream file(SHADER_DIR + file_path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    assert(false && "failed to load shader");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size + 1, '\0');

  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();

  wgpu::ShaderSourceWGSL source{{.code = buffer.data()}};
  wgpu::ShaderModuleDescriptor descriptor{};
  descriptor.nextInChain = &source;
  shader_module = context.device.CreateShaderModule(&descriptor);
}

void GraphicsPipelineBuilder::SetDepthStencil(wgpu::CompareFunction compare,
                                              bool write,
                                              wgpu::TextureFormat format) {
  depth_state.format = format;
  depth_state.depthCompare = compare;
  depth_state.depthWriteEnabled = write;
  depth_state.stencilReadMask = 0;
  depth_state.stencilWriteMask = 0;
}

ComputePipeline ComputePipelineBuilder::Build(BackendContext &context) {
  ComputePipeline pipeline;

  wgpu::PipelineLayoutDescriptor layout_desc{};
  layout_desc.bindGroupLayouts = bind_layouts.data();
  layout_desc.bindGroupLayoutCount = bind_layouts.size();
  pipeline.layout = context.device.CreatePipelineLayout(&layout_desc);

  wgpu::ComputeState compute_state{};
  compute_state.module = shader_module;

  wgpu::ComputePipelineDescriptor desc{};
  desc.layout = pipeline.layout;
  desc.compute = compute_state;

  pipeline.obj = context.device.CreateComputePipeline(&desc);
  return pipeline;
}

GraphicsPipeline GraphicsPipelineBuilder::Build(BackendContext &context) {
  GraphicsPipeline pipeline;

  wgpu::PipelineLayoutDescriptor layout_desc{};
  layout_desc.bindGroupLayoutCount = bind_layouts.size();
  layout_desc.bindGroupLayouts = bind_layouts.data();
  pipeline.layout = context.device.CreatePipelineLayout(&layout_desc);

  wgpu::ColorTargetState color_target{};
  color_target.format = context.surface_format;

  wgpu::FragmentState fragment{};
  fragment.module = shader_module;
  fragment.targetCount = 1;
  fragment.targets = &color_target;

  wgpu::RenderPipelineDescriptor descriptor{};
  descriptor.vertex = {.module = shader_module};
  descriptor.fragment = &fragment;
  descriptor.layout = pipeline.layout;
  descriptor.depthStencil = &depth_state;

  pipeline.obj = context.device.CreateRenderPipeline(&descriptor);
  return pipeline;
}
