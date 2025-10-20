#pragma once

#include "Backend/context.h"
#include "Backend/pipeline.h"
#include "Backend/texture.h"
#include "Manager/scene.h"
#include "camera.h"
#include "levels.h"

struct Engine {
  BackendContext backend_context;
  GLFWwindow *window;

  GraphicsPipeline pipeline;
  ComputePipeline physics_pipeline;

  BindGroup stats_bind_group;
  AllocatedBuffer delta_time_buffer;

  AllocatedTexture depth_texture;

  Camera camera;
  Scene scene;

  LevelData current_level;
  uint current_level_index;

  int selected_answer;

  float delta_time;
  bool paused = true;
  bool new_level_requested;

  void Init();

  void LoadLevel();

  void Run();

  void Destroy();
};
