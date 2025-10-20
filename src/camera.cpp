#include "camera.h"
#include "Backend/binding.h"
#include "Backend/buffer.h"
#include "Backend/context.h"
#include "input.h"
#include "webgpu/webgpu_cpp.h"
#include <fmt/base.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

void Camera::Create(BackendContext &context) {
  buffer = CreateBuffer(context,
                        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
                        sizeof(CameraBuffer));

  BindGroupBuilder bind_group_builder;
  bind_group_builder.AddUniformBuffer(buffer);
  bind_group = bind_group_builder.Build(
      context, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment |
                   wgpu::ShaderStage::Compute);
};

void Camera::Update(BackendContext &context, float delta_time) {
  const double sensitivity = 3.0f;
  if (InputContext::GetInputHeld(Input::MOUSE_LEFT)) {
    yaw -= InputContext::delta_mouse_position.x * sensitivity;
    pitch += InputContext::delta_mouse_position.y * sensitivity;
  }

  if (pitch < -89.0f)
    pitch = -89.0f;
  else if (pitch > 89.0f)
    pitch = 89.0f;

  const float speed = 50.0f * delta_time;

  if (InputContext::GetInputHeld(Input::W))
    velocity.z = -speed;
  else if (InputContext::GetInputHeld(Input::S))
    velocity.z = speed;
  else
    velocity.z = 0.0f;

  if (InputContext::GetInputHeld(Input::A))
    velocity.x = -speed;
  else if (InputContext::GetInputHeld(Input::D))
    velocity.x = speed;
  else
    velocity.x = 0.0f;

  if (InputContext::GetInputHeld(Input::Q))
    velocity.y = -speed;
  else if (InputContext::GetInputHeld(Input::E))
    velocity.y = speed;
  else
    velocity.y = 0.0f;

  {
    const float z_near = 0.01f;
    const float z_far = 10000.0f;
    const float aspect_ratio =
        InputContext::window_size.x / InputContext::window_size.y;
    const float fov_y = glm::radians(70.0f);

    glm::quat pitch_rotation = glm::angleAxis(pitch, glm::vec3{1.f, 0.f, 0.f});
    glm::quat yaw_rotation = glm::angleAxis(yaw, glm::vec3{0.f, -1.f, 0.f});
    glm::mat4 rotation_matrix =
        glm::toMat4(yaw_rotation) * glm::toMat4(pitch_rotation);

    glm::mat4 camera_translation = glm::translate(glm::mat4(1.f), position);

    glm::mat4 view_matrix = glm::inverse(camera_translation * rotation_matrix);

    buffer_data.view_matrix = view_matrix;

    glm::mat4 projection = glm::perspective(fov_y, aspect_ratio, z_near, z_far);

    buffer_data.projection_matrix = projection;

    position += glm::vec3(rotation_matrix * glm::vec4(velocity, 0.0f));

    buffer_data.view_pos = position;

    glm::mat4 clip = projection * view_matrix;

    Frustum frustum{};
    frustum.left = glm::normalize(clip[3] + clip[0]);
    frustum.right = glm::normalize(clip[3] - clip[0]);
    frustum.bottom = glm::normalize(clip[3] + clip[1]);
    frustum.top = glm::normalize(clip[3] - clip[1]);
    frustum.near = glm::normalize(clip[3] + clip[2]);
    frustum.far = glm::normalize(clip[3] - clip[2]);

    buffer_data.frustum = frustum;

    buffer_data.inv_view = glm::inverse(view_matrix);
    buffer_data.inv_proj = glm::inverse(projection);

    buffer_data.near = z_near;
    buffer_data.far = z_far;

    context.device.GetQueue().WriteBuffer(buffer.obj, 0, &buffer_data,
                                          sizeof(CameraBuffer));
  }
}
