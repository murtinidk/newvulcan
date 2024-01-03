#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace nve {

class NveWindow {
 public:
  NveWindow(int w, int h, std::string name);
  ~NveWindow();

  NveWindow(const NveWindow&) = delete;
  NveWindow& operator=(const NveWindow&) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

 private:
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
  void initWindow();

  GLFWwindow* window;
  int width;
  int height;
  bool framebufferResized = false;

  std::string windowName;
};

}  // namespace nve
