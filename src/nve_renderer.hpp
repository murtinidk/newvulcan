#pragma once

#include "nve_device.hpp"
#include "nve_swap_chain.hpp"
#include "nve_window.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace nve
{

  class NveRenderer
  {
  public:
    NveRenderer(NveWindow &nveWindow, NveDevice &nveDevice);
    ~NveRenderer();

    NveRenderer(const NveRenderer &) = delete;
    NveRenderer &operator=(const NveRenderer &) = delete;

    VkRenderPass getSwapChainRenderPass() const { return nveSwapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const
    {
      assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
      return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const
    {
      assert(isFrameStarted && "Cannot get frame index when frame not in progress");
      return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    NveWindow &nveWindow;
    NveDevice &nveDevice;
    std::unique_ptr<NveSwapChain> nveSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
  };

} // namespace nve