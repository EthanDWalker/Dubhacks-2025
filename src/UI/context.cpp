#include "context.h"
#include "fmt/base.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "webgpu/webgpu.h"
#include "webgpu/webgpu_cpp.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void InitUi(GLFWwindow *window, BackendContext &backend_context,
            AllocatedTexture &depth_texture) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForOther(window, true);
  ImGui_ImplWGPU_InitInfo init_info{};
  init_info.DepthStencilFormat = *(WGPUTextureFormat *)&depth_texture.format;
  init_info.Device = *(WGPUDevice *)&backend_context.device;
  init_info.RenderTargetFormat =
      *(WGPUTextureFormat *)&backend_context.surface_format;

  ImGui_ImplWGPU_Init(&init_info);
}

bool DrawUi(wgpu::RenderPassEncoder pass, LevelData &level_data,
            int &selected_answer, uint &current_level_index, bool &paused) {
  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  for (uint i = 0; i < level_data.instances.size(); i++) {
    ImGui::Text("%s:", level_data.instance_names[i].c_str());
    ImGui::Text("acceleration: %s",
                glm::to_string(level_data.instances[i].acceleration).c_str());
    ImGui::Text("velocity: %s",
                glm::to_string(level_data.instances[i].velocity).c_str());
    ImGui::Text(" ");
  }

  ImGui::Separator();
  ImGui::Text(" ");

  ImGui::Text("%s", level_data.question.c_str());
  for (uint i = 0; i < 4; i++) {
    if (ImGui::RadioButton(level_data.answers[i].c_str(),
                           selected_answer == i)) {
      selected_answer = i;
    }
  }

  ImGui::Text(" ");

  if (ImGui::Button("Play Simulation")) {
    paused = false;
  }

  ImGui::SameLine();
  if (ImGui::Button("Stop Simulation")) {
    paused = true;
  }

  bool new_level = false;
  if (paused == false) {
    if (selected_answer == level_data.correct_answer) {
      if (ImGui::Button("Next Level")) {
        new_level = true;
        current_level_index++;
      }
    } else {
      if (ImGui::Button("Reset Level")) {
        new_level = true;
      }
    }
  }

  ImGui::EndFrame();
  ImGui::Render();
  ImDrawData *draw_data = ImGui::GetDrawData();
  draw_data->FramebufferScale = ImVec2(1, 1);
  ImGui_ImplWGPU_RenderDrawData(draw_data, *(WGPURenderPassEncoder *)&pass);
  return new_level;
}

void DestroyUi() {
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplWGPU_Shutdown();
}
