#include "keyboard_movement_controller.hpp"

#include <limits>

namespace nve
{

  void KeyboardMovementController::moveInPlaneXZ(GLFWwindow *window, float dT, NveGameObject &gameObject)
  {
    glm::vec3 rotate{0};
    // clang-format off
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;
    // clang-format on

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
      gameObject.transform.rotation += lookSpeed * dT * glm::normalize(rotate);
    }

    // limit pitch values to +/- 90 degrees
    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{glm::sin(yaw), 0.f, glm::cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{.0f};
    // clang-format off
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;
    // clang-format on

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
    {
      gameObject.transform.translation += moveSpeed * dT * glm::normalize(moveDir);
    }
  }

} // namespace nve
