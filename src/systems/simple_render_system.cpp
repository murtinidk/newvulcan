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
    glm::mat4 modelProjection{1.f};
  };

  SimpleRenderSystem::SimpleRenderSystem(NveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
      : nveDevice{device}
  {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem()
  {
    vkDestroyPipelineLayout(nveDevice.device(), pipelineLayout, nullptr);
  }

  void
  SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

  void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo)
  {
    nvePipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

    for (auto &kv : frameInfo.gameObjects)
    {
      auto &obj = kv.second;
      if (obj.model == nullptr)
        continue;
      SimplePushConstantData push{};
      push.modelProjection = obj.transform.mat4();

      vkCmdPushConstants(
          frameInfo.commandBuffer,
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(SimplePushConstantData),
          &push);
      obj.model->bind(frameInfo.commandBuffer);
      obj.model->draw(frameInfo.commandBuffer);
    }
  }

} // namespace nve