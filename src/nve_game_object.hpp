#pragma once

#include "nve_model.hpp"

// glm
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace nve
{

  struct TransformComponent
  {
    glm::vec3 translation{};
    float scale{1.f};
    glm::vec3 rotation{}; // Tait-Bryan Angles(Euler Angles)(Y, X, Z)

    // Matrix corrsponds to translate * Roty * Rotx * Rotz * Scale
    // Rotation convention used Tait-Bryan Angles with axis order Y(1), X(2), Z(3)
    //* this function gets called once per frame per object, make sure it is optimized
    glm::mat4 mat4();
  };

  struct PointLightComponent
  {
    float lightIntensity{1.f};
  };

  class NveGameObject
  {
  public:
    using id_t = uint32_t;
    using Map = std::unordered_map<id_t, NveGameObject>;

    static NveGameObject createGameObject()
    {
      static id_t currentId = 0;
      return NveGameObject(currentId++);
    }

    static NveGameObject makePointLight(float intesity = 1.f, float radius = 0.2f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));

    NveGameObject(const NveGameObject &) = delete;
    NveGameObject &operator=(const NveGameObject &) = delete;
    NveGameObject(NveGameObject &&) = default;
    NveGameObject &operator=(NveGameObject &&) = default;

    id_t getId() const { return id; }

    glm::vec3 color{};
    TransformComponent transform{};
    bool isHidden{false};

    // optional components
    std::shared_ptr<NveModel> model{};
    std::unique_ptr<PointLightComponent> pointLight = nullptr;

  private:
    NveGameObject(id_t objId) : id(objId) {}

    id_t id;
  };

} // namespace nve