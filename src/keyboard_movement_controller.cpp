#include "keyboard_movement_controller.hpp"

#include <limits>

namespace nve
{

  MouseLookController::MouseLookController(GLFWwindow *window)
  {
    cursor_ = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
    {
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    // Reset mouse position for next frame
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width / 2, height / 2);
  }

  MouseLookController::~MouseLookController()
  {
    glfwDestroyCursor(cursor_);
  }

  void MouseLookController::UpdateMouse(GLFWwindow *window, NveGameObject &gameObject)
  {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Reset mouse position for next frame
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width / 2, height / 2);

    glm::vec3 rotate{0};

    rotate.z += .001f * float(width / 2 - xpos);
    rotate.y -= .001f * float(height / 2 - ypos);

    gameObject.transform.rotation += rotate;

    // limit pitch values to +/- 90 degrees
    // gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());
    gameObject.transform.rotation.x = glm::mod(gameObject.transform.rotation.x, glm::two_pi<float>());
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
    gameObject.transform.rotation.z = glm::mod(gameObject.transform.rotation.z, glm::two_pi<float>());
  }

  void KeyboardMovementController::moveInPlaneXZ(GLFWwindow *window, float dT, NveGameObject &gameObject)
  {
    glm::vec3 rotate{0};
    // clang-format off
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.z -= 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.z += 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.y += 1.f;
    // clang-format on

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
      gameObject.transform.rotation += lookSpeed * dT * glm::normalize(rotate);
    }

    // limit pitch values to +/- 90 degrees
    // gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());
    // gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
    gameObject.transform.rotation.x = glm::mod(gameObject.transform.rotation.x, glm::two_pi<float>());
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
    gameObject.transform.rotation.z = glm::mod(gameObject.transform.rotation.z, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.z;
    const glm::vec3 forwardDir{glm::sin(yaw), glm::cos(yaw), 0.f};
    const glm::vec3 rightDir{-forwardDir.y, forwardDir.x, 0.f};
    const glm::vec3 upDir{0.f, 0.f, 1.f};

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
