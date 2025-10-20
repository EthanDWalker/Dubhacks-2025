#include "engine.h"
#include "Backend/binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include "Backend/pipeline.h"
#include "Backend/texture.h"
#include "Parsers/model.h"
#include "UI/context.h"
#include "input.h"
#include "levels.h"
#include "timer.h"
#include "webgpu/webgpu_cpp.h"

#include <GLFW/glfw3.h>

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1600

void Engine::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Engine", nullptr, nullptr);

  InputContext::InitCallbacks(window);

  backend_context.Init(window, WINDOW_WIDTH, WINDOW_HEIGHT);

  camera.Create(backend_context);

  scene.Init(backend_context);

  wgpuPollEvents(backend_context.device, true);

  current_level = GetLevels()[0];

  std::vector<MeshData> mesh_data = ParseModel("Box.gltf");

  for (auto &mesh : mesh_data) {
    uint32_t object_id =
        scene.AddObject(backend_context, mesh.vertices, mesh.indices);
    for (auto &instance : current_level.instances) {
      scene.AddInstance(backend_context, &instance);
    }
  }

  scene.UpdateSceneInfo(backend_context);

  wgpuPollEvents(backend_context.device, true);

  depth_texture = CreateTexture(
      backend_context, wgpu::TextureFormat::Depth24Plus,
      wgpu::TextureDimension::e2D, wgpu::TextureViewDimension::e2D,
      wgpu::TextureAspect::DepthOnly,
      glm::u32vec3(WINDOW_WIDTH, WINDOW_HEIGHT, 1),
      wgpu::TextureUsage::RenderAttachment);

  InitUi(window, backend_context, depth_texture);

  {
    delta_time_buffer = CreateBuffer(
        backend_context,
        wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, sizeof(float));
    BindGroupBuilder bind_group_builder{};
    bind_group_builder.AddUniformBuffer(delta_time_buffer);
    stats_bind_group =
        bind_group_builder.Build(backend_context, wgpu::ShaderStage::Compute);
  }

  {
    GraphicsPipelineBuilder pipeline_builder{};
    pipeline_builder.SetShader(backend_context, "main_pass.wgsl");
    pipeline_builder.SetDepthStencil(wgpu::CompareFunction::Less, true,
                                     depth_texture.format);
    pipeline_builder.AddBindGroup(camera.bind_group);
    pipeline_builder.AddBindGroup(scene.object_bind_group_read);
    pipeline_builder.AddBindGroup(scene.instance_bind_group_read);

    pipeline = pipeline_builder.Build(backend_context);
  }

  {
    ComputePipelineBuilder pipeline_builder{};
    pipeline_builder.SetShader(backend_context, "physics.wgsl");
    pipeline_builder.AddBindGroup(scene.instance_bind_group);
    pipeline_builder.AddBindGroup(scene.object_bind_group);
    pipeline_builder.AddBindGroup(stats_bind_group);
    physics_pipeline = pipeline_builder.Build(backend_context);
  }
}

void Engine::Run() {
  const float ZERO_F = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    Timer timer{};
    InputContext::Update(window);

    camera.Update(backend_context, delta_time);

    wgpu::SurfaceTexture surface_texture;
    backend_context.surface.GetCurrentTexture(&surface_texture);

    wgpu::CommandEncoder encoder =
        backend_context.device.CreateCommandEncoder();

    {
      wgpu::ComputePassDescriptor compute_desc{};
      wgpu::ComputePassEncoder pass = encoder.BeginComputePass(&compute_desc);
      pass.SetPipeline(physics_pipeline.obj);
      pass.SetBindGroup(0, scene.instance_bind_group.obj);
      pass.SetBindGroup(1, scene.object_bind_group.obj);
      pass.SetBindGroup(2, stats_bind_group.obj);
      pass.DispatchWorkgroups(scene.instance_index / 32 + 1);
      pass.End();
    }

    {
      wgpu::RenderPassDepthStencilAttachment depth_att{};
      depth_att.view = depth_texture.view;
      depth_att.depthClearValue = 1.0f;
      depth_att.depthStoreOp = wgpu::StoreOp::Store;
      depth_att.depthLoadOp = wgpu::LoadOp::Clear;
      depth_att.stencilClearValue = 0;
      depth_att.stencilReadOnly = true;

      wgpu::RenderPassColorAttachment attachment{};
      attachment.view = surface_texture.texture.CreateView();
      attachment.loadOp = wgpu::LoadOp::Clear;
      attachment.storeOp = wgpu::StoreOp::Store;

      wgpu::RenderPassDescriptor renderpass{};
      renderpass.colorAttachmentCount = 1;
      renderpass.colorAttachments = &attachment;
      renderpass.depthStencilAttachment = &depth_att;

      wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
      pass.SetPipeline(pipeline.obj);
      pass.SetBindGroup(0, camera.bind_group.obj);
      pass.SetBindGroup(1, scene.object_bind_group_read.obj);
      pass.SetBindGroup(2, scene.instance_bind_group_read.obj);
      pass.SetIndexBuffer(scene.index_buffer.obj, wgpu::IndexFormat::Uint32);
      pass.DrawIndexed(scene.indice_index, scene.instance_index, 0, 0, 0);
      new_level_requested = DrawUi(pass, current_level, selected_answer,
                                   current_level_index, paused);
      pass.End();
    }

    wgpu::CommandBuffer commands = encoder.Finish();
    backend_context.device.GetQueue().Submit(1, &commands);

    backend_context.surface.Present();
    backend_context.instance.ProcessEvents();

    wgpuPollEvents(backend_context.device, true);

    if (InputContext::GetInputPressed(Input::ESCAPE)) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (new_level_requested) {
      paused = true;
      wgpuPollEvents(backend_context.device, true);

      scene.Reset(backend_context);

      current_level = GetLevels()[current_level_index];

      wgpuPollEvents(backend_context.device, true);

      for (auto &instance : current_level.instances) {
        std::vector<MeshData> mesh_data = ParseModel("Box.gltf");

        for (auto &mesh : mesh_data) {
          uint32_t object_id =
              scene.AddObject(backend_context, mesh.vertices, mesh.indices);
          for (auto &instance : current_level.instances) {
            scene.AddInstance(backend_context, &instance);
          }
        }
      };

      scene.UpdateSceneInfo(backend_context);

      wgpuPollEvents(backend_context.device, true);
    }

    delta_time = timer.Elapsed();
    if (!paused) {
      backend_context.device.GetQueue().WriteBuffer(delta_time_buffer.obj, 0,
                                                    &delta_time, sizeof(float));
    } else {
      backend_context.device.GetQueue().WriteBuffer(delta_time_buffer.obj, 0,
                                                    &ZERO_F, sizeof(float));
    }
  }
}

void Engine::Destroy() {
  scene.Destroy();
  camera.Destroy();
  depth_texture.Destroy();
  delta_time_buffer.Destroy();
  glfwTerminate();
  glfwDestroyWindow(window);
}
