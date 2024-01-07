///* right handed z-up with x beeing forward
#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "nve_camera.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "nve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <iostream>
#include <stdexcept>

#define MAX_FRAME_TIME 0.1f

namespace nve
{

  NveApp::NveApp()
  {
    globalPool = NveDescriptorPool::Builder(nveDevice)
                     .setMaxSets(NveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();
    loadGameObjects();
  }

  NveApp::~NveApp()
  {
  }

  void
  NveApp::run()
  {
    std::vector<std::unique_ptr<NveBuffer>> uboBuffers(NveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; size_t(i) < uboBuffers.size(); i++)
    {
      uboBuffers[i] = std::make_unique<NveBuffer>(
          nveDevice,
          sizeof(GlobalUbo),
          1,
          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      uboBuffers[i]->map();
    }

    auto globalSetLayout = NveDescriptorSetLayout::Builder(nveDevice)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                               .build();

    std::vector<VkDescriptorSet> globalDescriptorSet(NveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; size_t(i) < globalDescriptorSet.size(); i++)
    {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      NveDescriptorWriter(*globalSetLayout, *globalPool)
          .writeBuffer(0, &bufferInfo)
          .build(globalDescriptorSet[i]);
    }

    SimpleRenderSystem simpleRenderSystem{
        nveDevice,
        nveRenderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout()};
    PointLightSystem pointLightSystem{
        nveDevice,
        nveRenderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout()};
    NveCamera camera{};
    // camera.setViewYXZ(glm::vec3{0.f}, glm::vec3{0.f, 0.f, 0.f});
    // camera.setViewDirection(glm::vec3{0.f}, glm::vec3{1.f, 0.f, 0.f});
    // camera.setViewTarget(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{2.5f, 0.f, 0.f});

    auto viewerObject = NveGameObject::createGameObject();
    viewerObject.transform.translation.x = -2.5f;
    KeyboardMovementController cameraController{};
    MouseLookController mouseController{nveWindow.getGLFWwindow()};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!nveWindow.shouldClose())
    {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      currentTime = newTime;

      frameTime = glm::min(frameTime, MAX_FRAME_TIME);

      // std::cout << "rotations: x: " << viewerObject.transform.rotation.x << " y: " << viewerObject.transform.rotation.y << " z: " << viewerObject.transform.rotation.z << '\n';

      cameraController.moveInPlaneXZ(nveWindow.getGLFWwindow(), frameTime, viewerObject);
      mouseController.UpdateMouse(nveWindow.getGLFWwindow(), viewerObject);
      // camera.setViewZXY(glm::vec3{2.5f, 2.5f, 0.f}, glm::vec3{0.f, 1.f, -glm::half_pi<float>()});
      // camera.setViewZXY(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 0.f});
      camera.setViewZXY(viewerObject.transform.translation, viewerObject.transform.rotation);
      // camera.PrintMat4();

      float aspect = nveRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
      // camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, 0.1f, 10.f);

      if (auto commandBuffer = nveRenderer.beginFrame())
      {
        int frameIndex = nveRenderer.getFrameIndex();
        FrameInfo frameInfo{
            frameIndex,
            frameTime,
            commandBuffer,
            camera,
            globalDescriptorSet[frameIndex],
            gameObjects};

        // update
        GlobalUbo ubo{};
        ubo.projection = camera.getProjection();
        ubo.view = camera.getView();
        ubo.inverseView = camera.getInverseiew();
        pointLightSystem.update(frameInfo, ubo);
        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();

        // render
        nveRenderer.beginSwapChainRenderPass(commandBuffer);

        // order here matters
        simpleRenderSystem.renderGameObjects(frameInfo);
        pointLightSystem.render(frameInfo);

        nveRenderer.endSwapChainRenderPass(commandBuffer);
        nveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(nveDevice.device());
  }

  void
  NveApp::loadGameObjects()
  {
    std::shared_ptr<NveModel> nveModel = NveModel::createModelFromFile(nveDevice, "../models/flat_vase.obj");
    auto flat_vase = NveGameObject::createGameObject();
    flat_vase.model = nveModel;
    flat_vase.transform.translation = {0.f, 1.f, -0.5f};
    flat_vase.transform.scale = 3.f;
    flat_vase.transform.rotation = {-glm::half_pi<float>(), 0.f, 0.f};
    gameObjects.emplace(flat_vase.getId(), std::move(flat_vase));

    nveModel = NveModel::createModelFromFile(nveDevice, "../models/smooth_vase.obj");
    auto smooth_vase = NveGameObject::createGameObject();
    smooth_vase.model = nveModel;
    smooth_vase.transform.translation = {0.f, -1.f, -0.5f};
    smooth_vase.transform.scale = 3.f;
    smooth_vase.transform.rotation = {-glm::half_pi<float>(), 0.f, 0.f};
    gameObjects.emplace(smooth_vase.getId(), std::move(smooth_vase));

    nveModel = NveModel::createModelFromFile(nveDevice, "../models/quad.obj");
    auto floor = NveGameObject::createGameObject();
    floor.model = nveModel;
    floor.transform.translation = {0.f, 0.f, -0.5f};
    floor.transform.scale = 3.f;
    floor.transform.rotation = {-glm::half_pi<float>(), 0.f, 0.f};
    gameObjects.emplace(floor.getId(), std::move(floor));

    std::vector<glm::vec3> lightColors{
        {1.f, .1f, .1f},
        {.1f, .1f, 1.f},
        {.1f, 1.f, .1f},
        {1.f, 1.f, .1f},
        {.1f, 1.f, 1.f},
        {1.f, 1.f, 1.f} //
    };

    for (int i = 0; size_t(i) < lightColors.size(); i++)
    {
      auto pointLight = NveGameObject::makePointLight(0.2f);
      pointLight.color = lightColors[i];
      auto rotateLight = glm::rotate(
          glm::mat4{1.f},
          (i * glm::two_pi<float>()) / lightColors.size(),
          glm::vec3{0.f, 0.f, 1.f});
      pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4{0.f, 2.f, 1.f, 1.f});
      gameObjects.emplace(pointLight.getId(), std::move(pointLight));
    }
  }

} // namespace nve