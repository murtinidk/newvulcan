#pragma once

#include "nve_camera.hpp"
#include "nve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace nve
{

#define MAX_LIGHTS 10

  struct PointLight
  {
    glm::vec4 position{}; // ignore w
    glm::vec4 color;      // w is intensity
  };

  struct GlobalUbo
  {

    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::mat4 inverseView{1.f};
    alignas(16) glm::vec4 ambientColor{1.f, 1.f, 1.f, 0.02f};
    PointLight pointLights[MAX_LIGHTS];
    int numLights{0};
  };

  struct FrameInfo
  {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    NveCamera &camera;
    VkDescriptorSet globalDescriptorSet;
    NveGameObject::Map &gameObjects;
  };

} // namespace nve
