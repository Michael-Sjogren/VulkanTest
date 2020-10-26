cmake_minimum_required(VERSION 3.16)
set(${project_name} VulkanTest)
project(VulkanTest)
set(CMAKE_CXX_STANDARD 14)
set(HAVE_LIBC TRUE)

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

if (glfw3_FOUND)
    message(STATUS "glfw3 found")
    target_link_libraries(${PROJECT_NAME} glfw)
endif()

if (Vulkan_FOUND)
    message(STATUS "Vulkan found")
    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})
endif()
