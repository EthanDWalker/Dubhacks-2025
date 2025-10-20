#include "levels.h"
#include "glm/ext/matrix_transform.hpp"

LevelData GetLevel1() {
  LevelData level_data;
  level_data.instances.push_back({
      .matrix = glm::mat4(1.0f),
      .velocity = glm::vec3(0.0f),
      .object_id = 0,
      .acceleration = glm::vec3(0.0f, -9.8f, 0.0f),
      .color = glm::vec3(1.0f, 0.0f, 0.0f),
  });
  level_data.instance_names.push_back("red block");

  level_data.instances.push_back({
      .matrix = glm::scale(
          glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f)),
          glm::vec3(10.0f, 0.1f, 10.0f)),
      .velocity = glm::vec3(0.0f),
      .object_id = 0,
      .acceleration = glm::vec3(0.0f, -1.0f, 0.0f),
      .color = glm::vec3(1.0f, 1.0f, 1.0f),
  });
  level_data.instance_names.push_back("white block");

  level_data.question = "Will the red block hit the white block?";
  level_data.answers[0] = "Yes, after a long time";
  level_data.answers[1] = "Yes, after a short time";
  level_data.answers[2] = "No, both have negative acceleration";
  level_data.answers[3] = "All of the above";
  level_data.correct_answer = 1;

  return level_data;
}

LevelData GetLevel2() {
  LevelData level_data;
  level_data.instances.push_back({
      .matrix = glm::mat4(1.0f),
      .velocity = glm::vec3(0.0f),
      .object_id = 0,
      .acceleration = glm::vec3(0.0f, -9.8f, 0.0f),
      .color = glm::vec3(0.0f, 0.0f, 1.0f),
  });
  level_data.instance_names.push_back("blue block");

  level_data.instances.push_back({
      .matrix = glm::scale(
          glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f)),
          glm::vec3(10.0f, 0.1f, 10.0f)),
      .velocity = glm::vec3(0.0f),
      .object_id = 0,
      .acceleration = glm::vec3(0.0f, -1.0f, 0.0f),
      .color = glm::vec3(1.0f, 1.0f, 1.0f),
  });
  level_data.instance_names.push_back("white block");

  level_data.question = "Will the blue block hit the white block?";
  level_data.answers[0] = "Yes, after a long time";
  level_data.answers[1] = "Yes, after a short time";
  level_data.answers[2] = "No, both have negative acceleration";
  level_data.answers[3] = "All of the above";
  level_data.correct_answer = 1;

  return level_data;
}

std::vector<LevelData> GetLevels() {
  std::vector<LevelData> levels;
  levels.push_back(GetLevel1());
  levels.push_back(GetLevel2());
  return levels;
}
