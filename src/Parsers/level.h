#pragma once

#include "types.h"

struct LevelInfo {
  std::vector<Instance> instances;
};

const std::string LEVEL_DIR = "../assets/levels/";

LevelInfo ParseLevel(std::string file_path);
