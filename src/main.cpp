// TODO: implement mouse mechanics
//* recources: https://www.glfw.org/docs/3.3/input_guide.html
//*            http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/

#include "app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
  try
  {
    nve::NveApp app{};
    app.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}