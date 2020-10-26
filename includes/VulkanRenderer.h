#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include "Utilites.h"
#pragma once
class VulkanRenderer
{
private:
    /* data */
    GLFWwindow* window;
    // vulkan components
    VkInstance instance;
    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;

    void CreateInstance();
    void CreateLogicalDevice();
    // support functions
    // checker functions
    bool CheckInstanceExtentionSupport(std::vector<const char*> * extentions );
    bool CheckDeviceSuitable(VkPhysicalDevice device);
    // getter functions
    void GetPhysicalDevice();
    QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);

public:
    int Init(GLFWwindow* newWindow);
    void Cleanup();

    VulkanRenderer(/* args */){

    }

    ~VulkanRenderer() {

    }
};