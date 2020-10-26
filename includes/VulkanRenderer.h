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
    // validation
    bool CheckValidationLayerSupport();
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

public:
    int Init(GLFWwindow* newWindow);
    void Cleanup();

    VulkanRenderer(/* args */){

    }

    ~VulkanRenderer() {

    }
};