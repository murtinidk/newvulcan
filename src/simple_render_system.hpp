#pragma once

#include "nve_camera.hpp"
#include "nve_device.hpp"
#include "nve_game_object.hpp"
#include "nve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace nve
{

  class SimpleRenderSystem
  {
  public:
    SimpleRenderSystem(NveDevice &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<NveGameObject> &gameObjects, const NveCamera &camera);

  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    NveDevice &nveDevice;

    std::unique_ptr<NvePipeline> nvePipeline;
    VkPipelineLayout pipelineLayout;
  };

} // namespace nve