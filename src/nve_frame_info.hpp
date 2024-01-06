#pragma once

#include "nve_camera.hpp"
#include "nve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace nve
{

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
