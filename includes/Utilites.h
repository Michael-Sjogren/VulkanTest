#pragma once

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices
{
    int presentationFamily = -1;
    int graphicsFamily = -1; // location of graphics queue family
    // check if queuefamily is valid
    bool IsValid()
    {
        return graphicsFamily >= 0 &&
               presentationFamily >= 0;
    }
};

struct SwapChainDetails 
{
    VkSurfaceCapabilitiesKHR surfaceCapabilites;        // surface propeties
    std::vector<VkSurfaceFormatKHR> formats;            // image formats supported by surface and color space
    std::vector<VkPresentModeKHR> presentationModes;    // how images should be presented to screen
};