cmake_minimum_required(VERSION 3.16)
set(${project_name} VulkanTest)
project(VulkanTest)
set(CMAKE_CXX_STANDARD 17)

set(
    SRC 
    main.cpp
    source/VulkanRenderer.cpp
)

set(
    HEADERS
    includes/VulkanRenderer.h
    includes/Utilites.h
)

add_executable( VulkanTest ${SRC} ${HEADERS} )

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)


target_link_libraries(VulkanTest glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)
