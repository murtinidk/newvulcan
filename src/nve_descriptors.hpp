#pragma once

#include "nve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace nve
{

  class NveDescriptorSetLayout
  {
  public:
    class Builder
    {
    public:
      Builder(NveDevice &nveDevice) : nveDevice{nveDevice} {}

      Builder &addBinding(
          uint32_t binding,
          VkDescriptorType descriptorType,
          VkShaderStageFlags stageFlags,
          uint32_t count = 1);
      std::unique_ptr<NveDescriptorSetLayout> build() const;

    private:
      NveDevice &nveDevice;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    NveDescriptorSetLayout(
        NveDevice &nveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~NveDescriptorSetLayout();
    NveDescriptorSetLayout(const NveDescriptorSetLayout &) = delete;
    NveDescriptorSetLayout &operator=(const NveDescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

  private:
    NveDevice &nveDevice;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class NveDescriptorWriter;
  };

  class NveDescriptorPool
  {
  public:
    class Builder
    {
    public:
      Builder(NveDevice &nveDevice) : nveDevice{nveDevice} {}

      Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
      Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
      Builder &setMaxSets(uint32_t count);
      std::unique_ptr<NveDescriptorPool> build() const;

    private:
      NveDevice &nveDevice;
      std::vector<VkDescriptorPoolSize> poolSizes{};
      uint32_t maxSets = 1000;
      VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    NveDescriptorPool(
        NveDevice &nveDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~NveDescriptorPool();
    NveDescriptorPool(const NveDescriptorPool &) = delete;
    NveDescriptorPool &operator=(const NveDescriptorPool &) = delete;

    bool allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

  private:
    NveDevice &nveDevice;
    VkDescriptorPool descriptorPool;

    friend class NveDescriptorWriter;
  };

  class NveDescriptorWriter
  {
  public:
    NveDescriptorWriter(NveDescriptorSetLayout &setLayout, NveDescriptorPool &pool);

    NveDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    NveDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

  private:
    NveDescriptorSetLayout &setLayout;
    NveDescriptorPool &pool;
    std::vector<VkWriteDescriptorSet> writes;
  };

} // namespace nve