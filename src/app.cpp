///* right handed z-up with x beeing forward
#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "nve_camera.hpp"
#include "simple_render_system.hpp"

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
    loadGameObjects();
  }

  NveApp::~NveApp()
  {
  }

  void
  NveApp::run()
  {
    SimpleRenderSystem simpleRenderSystem{nveDevice, nveRenderer.getSwapChainRenderPass()};
    NveCamera camera{};
    camera.setViewYXZ(glm::vec3{0.f}, glm::vec3{0.f, 0.f, 0.f});
    // camera.setViewDirection(glm::vec3{0.f}, glm::vec3{1.f, 0.f, 0.f});
    // camera.setViewTarget(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{2.5f, 0.f, 0.f});

    auto viewerObject = NveGameObject::createGameObject();
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

      std::cout << "rotations: x: " << viewerObject.transform.rotation.x << " y: " << viewerObject.transform.rotation.y << " z: " << viewerObject.transform.rotation.z << '\n';

      cameraController.moveInPlaneXZ(nveWindow.getGLFWwindow(), frameTime, viewerObject);
      mouseController.UpdateMouse(nveWindow.getGLFWwindow(), viewerObject);
      camera.setViewYXZdownX(viewerObject.transform.translation, viewerObject.transform.rotation);

      float aspect = nveRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

      if (auto commandBuffer = nveRenderer.beginFrame())
      {
        nveRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
        nveRenderer.endSwapChainRenderPass(commandBuffer);
        nveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(nveDevice.device());
  }

  // temporary helper function, creates a 1x1x1 cube centered at offset
  // clang-format off
  std::unique_ptr<NveModel> createCubeModel(NveDevice &device, glm::vec3 offset)
  {
    std::vector<NveModel::Vertex> vertices{

        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto &v : vertices)
    {
      v.position += offset;
    }
    return std::make_unique<NveModel>(device, vertices);
  }
  // clang-format on

  void
  NveApp::loadGameObjects()
  {
    std::shared_ptr<NveModel> nveModel = createCubeModel(nveDevice, {0.f, 0.f, 0.f});

    auto cube = NveGameObject::createGameObject();
    cube.model = nveModel;
    cube.transform.translation = {2.5f, 0.f, 0.f};
    cube.transform.scale = {0.5f, 0.5f, 0.5f};
    gameObjects.push_back(std::move(cube));
  }

} // namespace nve