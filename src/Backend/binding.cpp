#include "binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include "webgpu/webgpu_cpp.h"

void BindGroupBuilder::AddUniformBuffer(AllocatedBuffer &buffer) {
  wgpu::BindGroupLayoutEntry layout_entry{};
  layout_entry.binding = layout_entries.size();
  layout_entry.buffer.type = wgpu::BufferBindingType::Uniform;
  layout_entry.buffer.minBindingSize = buffer.size;

  layout_entries.push_back(layout_entry);

  wgpu::BindGroupEntry entry{};
  entry.size = buffer.size;
  entry.buffer = buffer.obj;
  entry.offset = 0;
  entry.binding = entries.size();

  entries.push_back(entry);
}

void BindGroupBuilder::AddStorageBuffer(AllocatedBuffer &buffer,
                                        bool read_only) {
  wgpu::BindGroupLayoutEntry layout_entry{};
  layout_entry.binding = layout_entries.size();
  layout_entry.buffer.type = read_only
                                 ? wgpu::BufferBindingType::ReadOnlyStorage
                                 : wgpu::BufferBindingType::Storage;
  layout_entry.buffer.minBindingSize = buffer.size;

  layout_entries.push_back(layout_entry);

  wgpu::BindGroupEntry entry{};
  entry.size = buffer.size;
  entry.buffer = buffer.obj;
  entry.offset = 0;
  entry.binding = entries.size();

  entries.push_back(entry);
}

BindGroup BindGroupBuilder::Build(BackendContext &context,
                                  wgpu::ShaderStage visibility) {
  BindGroup bind_group;
  for (auto &entry : layout_entries) {
    entry.visibility = visibility;
  }

  wgpu::BindGroupLayoutDescriptor layout_desc{};
  layout_desc.entries = layout_entries.data();
  layout_desc.entryCount = layout_entries.size();
  bind_group.layout = context.device.CreateBindGroupLayout(&layout_desc);

  wgpu::BindGroupDescriptor desc{};
  desc.layout = bind_group.layout;
  desc.entries = entries.data();
  desc.entryCount = entries.size();
  bind_group.obj = context.device.CreateBindGroup(&desc);
  return bind_group;
}

void BindGroup::Destroy() { obj.Destroy(); }
