#include "app.hpp"

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

  NveApp::NveApp()
  {
    loadGameObjects();
  }

  NveApp::~NveApp()
  {
  }

  void
  NveApp::run()
  {
    SimpleRenderSystem simpleRenderSystem{nveDevice, nveRenderer.getSwapChainRenderPass()};

    while (!nveWindow.shouldClose())
    {
      glfwPollEvents();

      if (auto commandBuffer = nveRenderer.beginFrame())
      {
        nveRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
        nveRenderer.endSwapChainRenderPass(commandBuffer);
        nveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(nveDevice.device());
  }

  void
  NveApp::loadGameObjects()
  {
    std::vector<NveModel::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };
    auto nveModel = std::make_shared<NveModel>(nveDevice, vertices);

    auto triange = NveGameObject::createGameObject();
    triange.model = nveModel;
    triange.color = {0.1f, 0.8f, 0.1f};
    triange.transform2d.translation.x = 0.2f;
    triange.transform2d.scale = {2.f, .5f};
    triange.transform2d.rotation = 0.25f * glm::two_pi<float>();

    gameObjects.push_back(std::move(triange));
  }

} // namespace nve