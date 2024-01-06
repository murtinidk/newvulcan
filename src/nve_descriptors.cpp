#include "nve_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace nve
{

  // *************** Descriptor Set Layout Builder *********************

  NveDescriptorSetLayout::Builder &NveDescriptorSetLayout::Builder::addBinding(
      uint32_t binding,
      VkDescriptorType descriptorType,
      VkShaderStageFlags stageFlags,
      uint32_t count)
  {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
  }

  std::unique_ptr<NveDescriptorSetLayout> NveDescriptorSetLayout::Builder::build() const
  {
    return std::make_unique<NveDescriptorSetLayout>(nveDevice, bindings);
  }

  // *************** Descriptor Set Layout *********************

  NveDescriptorSetLayout::NveDescriptorSetLayout(
      NveDevice &nveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
      : nveDevice{nveDevice}, bindings{bindings}
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : bindings)
    {
      setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
            nveDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  NveDescriptorSetLayout::~NveDescriptorSetLayout()
  {
    vkDestroyDescriptorSetLayout(nveDevice.device(), descriptorSetLayout, nullptr);
  }

  // *************** Descriptor Pool Builder *********************

  NveDescriptorPool::Builder &NveDescriptorPool::Builder::addPoolSize(
      VkDescriptorType descriptorType, uint32_t count)
  {
    poolSizes.push_back({descriptorType, count});
    return *this;
  }

  NveDescriptorPool::Builder &NveDescriptorPool::Builder::setPoolFlags(
      VkDescriptorPoolCreateFlags flags)
  {
    poolFlags = flags;
    return *this;
  }
  NveDescriptorPool::Builder &NveDescriptorPool::Builder::setMaxSets(uint32_t count)
  {
    maxSets = count;
    return *this;
  }

  std::unique_ptr<NveDescriptorPool> NveDescriptorPool::Builder::build() const
  {
    return std::make_unique<NveDescriptorPool>(nveDevice, maxSets, poolFlags, poolSizes);
  }

  // *************** Descriptor Pool *********************

  NveDescriptorPool::NveDescriptorPool(
      NveDevice &nveDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes)
      : nveDevice{nveDevice}
  {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(nveDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  NveDescriptorPool::~NveDescriptorPool()
  {
    vkDestroyDescriptorPool(nveDevice.device(), descriptorPool, nullptr);
  }

  bool NveDescriptorPool::allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(nveDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS)
    {
      return false;
    }
    return true;
  }

  void NveDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
  {
    vkFreeDescriptorSets(
        nveDevice.device(),
        descriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
  }

  void NveDescriptorPool::resetPool()
  {
    vkResetDescriptorPool(nveDevice.device(), descriptorPool, 0);
  }

  // *************** Descriptor Writer *********************

  NveDescriptorWriter::NveDescriptorWriter(NveDescriptorSetLayout &setLayout, NveDescriptorPool &pool)
      : setLayout{setLayout}, pool{pool} {}

  NveDescriptorWriter &NveDescriptorWriter::writeBuffer(
      uint32_t binding, VkDescriptorBufferInfo *bufferInfo)
  {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  NveDescriptorWriter &NveDescriptorWriter::writeImage(
      uint32_t binding, VkDescriptorImageInfo *imageInfo)
  {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  bool NveDescriptorWriter::build(VkDescriptorSet &set)
  {
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success)
    {
      return false;
    }
    overwrite(set);
    return true;
  }

  void NveDescriptorWriter::overwrite(VkDescriptorSet &set)
  {
    for (auto &write : writes)
    {
      write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.nveDevice.device(), writes.size(), writes.data(), 0, nullptr);
  }

} // namespace nve
