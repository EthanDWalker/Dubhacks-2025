#pragma once

#include "types.h"

struct LevelData {
  std::vector<std::string> instance_names;
  std::vector<Instance> instances;
  std::string question;
  std::string answers[4];
  uint correct_answer;
};

std::vector<LevelData> GetLevels();
