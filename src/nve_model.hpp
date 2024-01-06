#pragma once

#include "nve_device.hpp"
#include "nve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// st
#include <memory>
#include <vector>

namespace nve
{

  class NveModel
  {
  public:
    // * Dont forget to change Attribute Descriptions in nve_model.cpp if this changes
    struct Vertex
    {
      glm::vec3 position{};
      glm::vec3 color{};
      glm::vec3 normal{};
      glm::vec2 uv{};

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex &other) const
      {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
      }
    };

    struct Builder
    {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
      // Builder& addVertex(Vertex vertex);
      // Builder& addIndex(uint32_t index);
      // NveModel build(NveDevice& device);
    };

    NveModel(NveDevice &device, const NveModel::Builder &builder);
    ~NveModel();

    NveModel(NveModel const &) = delete;
    NveModel &operator=(NveModel const &) = delete;

    static std::unique_ptr<NveModel> createModelFromFile(NveDevice &device, const std::string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    NveDevice &nveDevice;

    std::unique_ptr<NveBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<NveBuffer> indexBuffer;
    uint32_t indexCount;
  };

} // namespace nve