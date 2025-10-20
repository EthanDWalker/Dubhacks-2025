#pragma once

#include "Backend/context.h"
#include "Backend/texture.h"
#include "levels.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>

void InitUi(GLFWwindow *window, BackendContext &backend_context,
            AllocatedTexture &depth_texture);

bool DrawUi(wgpu::RenderPassEncoder pass, LevelData &level_data,
            int &selected_answer, uint &current_level_index, bool &paused);

void DestroyUi();
