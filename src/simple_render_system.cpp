#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <iostream>
#include <stdexcept>

namespace nve
{

  struct SimplePushConstantData
  {
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color;
  };

  SimpleRenderSystem::SimpleRenderSystem(NveDevice &device, VkRenderPass renderPass)
      : nveDevice{device}
  {
    createPipelineLayout();
    createPipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem()
  {
    vkDestroyPipelineLayout(nveDevice.device(), pipelineLayout, nullptr);
  }

  void
  SimpleRenderSystem::createPipelineLayout()
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(nveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void
  SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
  {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout is created");

    PipelineConfigInfo pipelineConfig{};
    NvePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    nvePipeline = std::make_unique<NvePipeline>(
        nveDevice, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);
  }

  void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<NveGameObject> &gameObjects, const NveCamera &camera)
  {
    nvePipeline->bind(commandBuffer);

    auto viewProjection = camera.getProjection() * camera.getView();

    for (auto &obj : gameObjects)
    {
      SimplePushConstantData push{};
      push.color = obj.color;
      push.transform = viewProjection * obj.transform.mat4();

      vkCmdPushConstants(
          commandBuffer,
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(SimplePushConstantData),
          &push);
      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
    }
  }

} // namespace nve