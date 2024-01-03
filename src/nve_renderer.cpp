#include "nve_renderer.hpp"

// std
#include <array>
#include <iostream>
#include <stdexcept>

namespace nve
{

  NveRenderer::NveRenderer(NveWindow &nveWindow, NveDevice &nveDevice)
      : nveWindow(nveWindow), nveDevice(nveDevice)
  {
    recreateSwapChain();
    createCommandBuffers();
  }

  NveRenderer::~NveRenderer()
  {
    freeCommandBuffers();
  }

  void
  NveRenderer::recreateSwapChain()
  {
    auto extent = nveWindow.getExtent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = nveWindow.getExtent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(nveDevice.device());

    if (nveSwapChain == nullptr)
    {
      nveSwapChain = std::make_unique<NveSwapChain>(nveDevice, extent);
    }
    else
    {
      std::shared_ptr<NveSwapChain> oldSwapChain = std::move(nveSwapChain);
      nveSwapChain = std::make_unique<NveSwapChain>(nveDevice, extent, oldSwapChain);

      if (!oldSwapChain->compareSwapFormats(*nveSwapChain.get()))
      {
        throw std::runtime_error("Swap chain image(or depth) format has changed");
      }
    }
  }

  void
  NveRenderer::createCommandBuffers()
  {
    commandBuffers.resize(NveSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = nveDevice.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = nveDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(nveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void NveRenderer::freeCommandBuffers()
  {
    vkFreeCommandBuffers(nveDevice.device(), nveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
  }

  VkCommandBuffer NveRenderer::beginFrame()
  {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

    auto result = nveSwapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      recreateSwapChain();
      return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
  }

  void NveRenderer::endFrame()
  {
    assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to record command buffer!");
    }

    auto result = nveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || nveWindow.wasWindowResized())
    {
      nveWindow.resetWindowResizedFlag();
      recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
      throw std::runtime_error("failed to present swap chain image!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % NveSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void NveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
  {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = nveSwapChain->getRenderPass();
    renderPassInfo.framebuffer = nveSwapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = nveSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(nveSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(nveSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, nveSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void NveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
  {
    assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
  }

} // namespace nve