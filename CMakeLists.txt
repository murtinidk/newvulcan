cmake_minimum_required(VERSION 3.10)

# Project name
project(newvulkan)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_compile_options("-Wall" "-Wextra" "-g")

set(SOURCE_FILES src/main.cpp src/nve_window.cpp src/nve_window.hpp src/app.cpp src/app.hpp src/nve_pipeline.cpp src/nve_pipeline.hpp src/nve_device.cpp src/nve_device.hpp src/nve_swap_chain.cpp src/nve_swap_chain.hpp src/nve_model.cpp src/nve_model.hpp src/nve_game_object.cpp src/nve_game_object.hpp src/nve_renderer.cpp src/nve_renderer.hpp src/simple_render_system.cpp src/simple_render_system.hpp)

# Find GLFW
add_subdirectory(libs/GLFW)
# Find GLM
add_subdirectory(libs/GLM)

# Find Vulkan
find_package(Vulkan REQUIRED)

# newvulkan executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC libs/GLWF/include)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC libs/GLM)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

ADD_CUSTOM_TARGET(
    compile_shaders
    COMMAND cmd /c ${PROJECT_SOURCE_DIR}/compile.bat
)
ADD_DEPENDENCIES(${PROJECT_NAME} compile_shaders)