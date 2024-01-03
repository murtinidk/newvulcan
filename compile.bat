cd /d "%~dp0"
del shaders\simple_shader.vert.spv
del shaders\simple_shader.frag.spv
C:\VulkanSDK\Bin\glslc.exe shaders\simple_shader.vert -o shaders\simple_shader.vert.spv
C:\VulkanSDK\Bin\glslc.exe shaders\simple_shader.frag -o shaders\simple_shader.frag.spv