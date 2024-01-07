#include "point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>

namespace nve
{

  struct PointLightPushConstants
  {
    glm::vec4 position{};
    glm::vec4 color{};
    float radius{};
  };

  PointLightSystem::PointLightSystem(NveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
      : nveDevice{device}
  {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
  }

  PointLightSystem::~PointLightSystem()
  {
    vkDestroyPipelineLayout(nveDevice.device(), pipelineLayout, nullptr);
  }

  void
  PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

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
  PointLightSystem::createPipeline(VkRenderPass renderPass)
  {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout is created");

    PipelineConfigInfo pipelineConfig{};
    NvePipeline::defaultPipelineConfigInfo(pipelineConfig);
    NvePipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    nvePipeline = std::make_unique<NvePipeline>(
        nveDevice, "../shaders/point_light.vert.spv", "../shaders/point_light.frag.spv", pipelineConfig);
  }

  void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo)
  {

    auto rotateLight = glm::rotate(
        glm::mat4{1.f},
        frameInfo.frameTime * glm::two_pi<float>() / 5.f,
        glm::vec3{0.f, 0.f, 1.f});

    int lightIndex = 0;
    for (auto &kv : frameInfo.gameObjects)
    {
      auto &gameObj = kv.second;
      if (gameObj.isHidden)
        continue;
      if (gameObj.pointLight == nullptr)
        continue;

      assert(lightIndex < MAX_LIGHTS && "Too many point lights!");

      // update light position
      gameObj.transform.translation = glm::vec3(rotateLight * glm::vec4(gameObj.transform.translation, 1.f));

      // copy light to ubo
      ubo.pointLights[lightIndex].position = glm::vec4(gameObj.transform.translation, 1.f);
      ubo.pointLights[lightIndex].color = glm::vec4(gameObj.color, gameObj.pointLight->lightIntensity);

      lightIndex++;
    }
    ubo.numLights = lightIndex;
  }

  void PointLightSystem::render(FrameInfo &frameInfo)
  {

    // sort lights by distance to camera
    std::map<float, NveGameObject::id_t> sorted;

    for (auto &kv : frameInfo.gameObjects)
    {
      auto &gameObj = kv.second;
      if (gameObj.pointLight == nullptr)
        continue;
      if (gameObj.isHidden)
        continue;

      // calculate distance to camera
      auto offset = frameInfo.camera.getPosition() - gameObj.transform.translation;
      float disSquared = glm::dot(offset, offset);
      sorted[disSquared] = gameObj.getId();
    }

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

    // itterate through sorted lights in reverse order
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
      auto &gameObj = frameInfo.gameObjects.at(it->second);

      PointLightPushConstants push{};
      push.position = glm::vec4(gameObj.transform.translation, 1.f);
      push.color = glm::vec4(gameObj.color, gameObj.pointLight->lightIntensity);
      push.radius = gameObj.transform.scale;

      vkCmdPushConstants(
          frameInfo.commandBuffer,
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(PointLightPushConstants),
          &push);

      vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
  }

} // namespace nve