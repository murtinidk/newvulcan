#pragma once

#include "nve_camera.hpp"
#include "nve_device.hpp"
#include "nve_game_object.hpp"
#include "nve_pipeline.hpp"
#include "nve_frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace nve
{

  class SimpleRenderSystem
  {
  public:
    SimpleRenderSystem(NveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    NveDevice &nveDevice;

    std::unique_ptr<NvePipeline> nvePipeline;
    VkPipelineLayout pipelineLayout;
  };

} // namespace nve