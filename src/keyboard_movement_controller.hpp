#pragma once

#include "nve_game_object.hpp"
#include "nve_window.hpp"

namespace nve
{
  class MouseLookController
  {
  public:
    MouseLookController(GLFWwindow *window);

    ~MouseLookController();

    void UpdateMouse(GLFWwindow *window, NveGameObject &gameObject);

    // void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    struct KeyMappings
    {
      int freeMouse = GLFW_KEY_TAB;
    };

    KeyMappings keys{};

  private:
    GLFWcursor *cursor_;
    bool freeMouse = false;
  };

  class KeyboardMovementController
  {
  public:
    struct KeyMappings
    {
      int moveLeft = GLFW_KEY_A;
      int moveRight = GLFW_KEY_D;
      int moveForward = GLFW_KEY_W;
      int moveBackward = GLFW_KEY_S;
      int moveUp = GLFW_KEY_E;
      int moveDown = GLFW_KEY_Q;
      int lookLeft = GLFW_KEY_LEFT;
      int lookRight = GLFW_KEY_RIGHT;
      int lookUp = GLFW_KEY_UP;
      int lookDown = GLFW_KEY_DOWN;
    };

    void moveInPlaneXZ(GLFWwindow *window, float dT, NveGameObject &gameObject);

    KeyMappings keys{};
    float moveSpeed{3.f};
    float lookSpeed{1.5f};
  };
} // namespace nve
