#include "nve_camera.hpp"

// std
#include <cassert>
#include <limits>
#include <iostream>
#include <iomanip>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

namespace nve
{

  void NveCamera::setOrthographicProjection(
      float left, float right, float top, float bottom, float near, float far)
  {
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[1][0] = -2.f / (right - left);
    projectionMatrix[2][1] = -2.f / (bottom - top);
    projectionMatrix[0][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
    projectionMatrix[3][3] = 1;
  }
  // backups where axis are not  flipped
  //  void NveCamera::setOrthographicProjection(
  //      float left, float right, float top, float bottom, float near, float far)
  //  {
  //    projectionMatrix = glm::mat4{1.0f};
  //    projectionMatrix[0][0] = 2.f / (right - left);
  //    projectionMatrix[1][1] = 2.f / (bottom - top);
  //    projectionMatrix[2][2] = 1.f / (far - near);
  //    projectionMatrix[3][0] = -(right + left) / (right - left);
  //    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
  //    projectionMatrix[3][2] = -near / (far - near);
  //  }

  void NveCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
  {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[1][0] = -1.f / (aspect * tanHalfFovy);
    projectionMatrix[2][1] = -1.f / (tanHalfFovy);
    projectionMatrix[0][2] = far / (far - near);
    projectionMatrix[0][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
  }

  // backups where axis are not  flipped
  //    void NveCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
  //  {
  //    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
  //    const float tanHalfFovy = tan(fovy / 2.f);
  //    projectionMatrix = glm::mat4{0.0f};
  //    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
  //    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
  //    projectionMatrix[2][2] = far / (far - near);
  //    projectionMatrix[2][3] = 1.f;
  //    projectionMatrix[3][2] = -(far * near) / (far - near);
  //  }

  void NveCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
  {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
  }

  void NveCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
  {
    assert(target - position != glm::vec3{0.f} && "Camera cannot point at its own position");
    setViewDirection(position, target - position, up);
  }

  void NveCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
  {
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), -s2, (c1 * c2)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
  }

  void NveCamera::setViewZXY(glm::vec3 position, glm::vec3 rotation)
  {
    const float c1 = glm::cos(rotation.z);
    const float s1 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c3 = glm::cos(rotation.y);
    const float s3 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 - s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3), -(c2 * s3)};
    const glm::vec3 v{-(c2 * s1), (c1 * c2), s2};
    const glm::vec3 w{(c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2), (c2 * c3)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
  }

} // namespace nve
