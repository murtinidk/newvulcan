#include "nve_window.hpp"

// std
#include <stdexcept>

namespace nve {

NveWindow::NveWindow(int w, int h, std::string name) : width(w), height(h), windowName(name) { initWindow(); }

NveWindow::~NveWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void NveWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void NveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void NveWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
  auto app = reinterpret_cast<NveWindow*>(glfwGetWindowUserPointer(window));
  app->framebufferResized = true;
  app->width = width;
  app->height = height;
}

}  // namespace nve