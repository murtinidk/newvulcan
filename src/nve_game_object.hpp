#pragma once

#include "nve_model.hpp"

// std
#include <memory>

namespace nve
{

struct Transform2dComponent
{
  glm::vec2 translation{};
  glm::vec2 scale{1.f, 1.f};
  float rotation;
  
  glm::mat2 mat2() { 
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMat = glm::mat2{{c, s}, {-s, c}};

    glm::mat2 scaleMat = glm::mat2{{scale.x, 0.f}, { 0.f, scale.y}};
    return rotMat * scaleMat; 
  }
};


class NveGameObject
{
  public:
    using id_t = uint32_t;

    static NveGameObject createGameObject(){
      static id_t currentId = 0;
      return NveGameObject(currentId++);
    }

    NveGameObject(const NveGameObject&) = delete;
    NveGameObject& operator=(const NveGameObject&) = delete;
    NveGameObject(NveGameObject&&) = default;
    NveGameObject& operator=(NveGameObject&&) = default;

    id_t getId() const { return id; }

    std::shared_ptr<NveModel> model;
    glm::vec3 color{};
    Transform2dComponent transform2d{};

  private:
    NveGameObject(id_t objId) : id(objId) {}

    id_t id;
  
};

} // namespace nve