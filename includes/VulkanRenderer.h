#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstring>
#include <set>
#include "Utilites.h"
#pragma once
class VulkanRenderer
{
private:
    /* data */
    GLFWwindow *window;
    // vulkan components
    // - Main
    VkInstance instance;
    struct
    {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;
    VkQueue presenetationQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    // - Utillity
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    // - create functions
    void CreateInstance();
    void CreateLogicalDevice();
    void CreateSurface();
    void CreateSwapChain();
    // - support functions
    // -- checker functions
    bool CheckInstanceExtentionSupport(std::vector<const char *> *extentions);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    bool CheckDeviceSuitable(VkPhysicalDevice device);
    // -- choose functions
    VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &modes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabillites);
    // - getter functions
    void GetPhysicalDevice();
    QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    SwapChainDetails GetSwapChainDetails(VkPhysicalDevice device);
    // - validation
    bool CheckValidationLayerSupport();
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

public:
    int Init(GLFWwindow *newWindow);
    void Cleanup();

    VulkanRenderer(/* args */)
    {
    }

    ~VulkanRenderer()
    {
    }
};