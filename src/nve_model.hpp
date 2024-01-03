#pragma once

#include "nve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

namespace nve
{

  class NveModel
  {
  public:
    // * Dont forget to change Attribute Descriptions in nve_model.cpp if this changes
    struct Vertex
    {
      glm::vec3 position;
      glm::vec3 color;

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    NveModel(NveDevice &device, const std::vector<Vertex> &vertices);
    ~NveModel();

    NveModel(NveModel const &) = delete;
    NveModel &operator=(NveModel const &) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    NveDevice &nveDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
  };

} // namespace nve