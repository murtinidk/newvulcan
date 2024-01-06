#pragma once

#include "nve_device.hpp"
#include "nve_descriptors.hpp"
#include "nve_game_object.hpp"
#include "nve_window.hpp"
#include "nve_renderer.hpp"

// std
#include <memory>
#include <vector>

namespace nve
{

  class NveApp
  {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGTH = 600;

    NveApp();
    ~NveApp();

    NveApp(const NveApp &) = delete;
    NveApp &operator=(const NveApp &) = delete;

    void run();

  private:
    void loadGameObjects();

    NveWindow nveWindow{WIDTH, HEIGTH, "newvulkan"};
    NveDevice nveDevice{nveWindow};
    NveRenderer nveRenderer{nveWindow, nveDevice};

    // note: order of declarations matters
    std::unique_ptr<NveDescriptorPool> globalPool{};
    NveGameObject::Map gameObjects;
  };

} // namespace nve