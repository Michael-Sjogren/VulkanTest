#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "includes/VulkanRenderer.h"
#include <iostream>
#include <memory>

GLFWwindow* window;
VulkanRenderer* vulkanRenderer;

void initWindow(std::string title = "VulkanTest", const int width = 800 , const int height = 600){
    if(!glfwInit()){
        std::cout << "failed to init glfw" << "\n";
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API , GLFW_NO_API );
    glfwWindowHint(GLFW_RESIZABLE , GLFW_FALSE);

    window = glfwCreateWindow(width, height , title.c_str() , nullptr , nullptr);
}

int main(void)
{
    initWindow();
    std::unique_ptr<VulkanRenderer> vulkanRenderer(new VulkanRenderer());
    if(vulkanRenderer->Init(window)){
        return EXIT_FAILURE;
    }
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }
    vulkanRenderer->Cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}