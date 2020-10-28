#include "../includes/VulkanRenderer.h"
int VulkanRenderer::Init(GLFWwindow *newWindow)
{
    window = newWindow;
    try
    {
        CreateInstance();
        CreateSurface();
        GetPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
    }
    catch (const std::exception &e)
    {
        std::printf("ERROR %s \n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}

void VulkanRenderer::CreateInstance()
{
    // information of the application itself
    // most data here wont affect the program is for developer convinece
    if (enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation Layers requested, but not available!");
    }
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_1;
    applicationInfo.pApplicationName = "Vulkan Test Application";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "None";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // creation information for a vk instance witch is a vulkan instance.
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    // create a list to hold the instance extenstions
    auto extentions = std::vector<const char *>();
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    // add GLFW extenstion to list of extenstions
    for (size_t i = 0; i < glfwExtCount; ++i)
    {
        extentions.push_back(glfwExtensions[i]);
    }
    // check if INSTANCE extentions are supported
    if (!CheckInstanceExtentionSupport(&extentions))
    {
        throw std::runtime_error("VKInstance does not support required extentions!");
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extentions.size());
    createInfo.ppEnabledExtensionNames = extentions.data();
    // if validation layers are activated
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        std::cout << "Validation layers enabled!"
                  << "\n";
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    // Create instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a vulkan instance");
    }
}

void VulkanRenderer::CreateLogicalDevice()
{
    // get queuefamily  indicies for the choosen physical device
    auto indices = GetQueueFamilies(mainDevice.physicalDevice);
    // vector for queue creating information and set for family indicies
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily};
    for (int queueFamilyIndex : queueFamilyIndices)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex; // the index of the family to create a queue from
        queueCreateInfo.queueCount = 1;                      // number of queues to create.
        float priority = 1.0f;
        queueCreateInfo.pQueuePriorities = &priority; // vulkan needs to know how to handle multpible queues, so decide prio 1 is highest 0 lowest
        queueCreateInfos.push_back(queueCreateInfo);
    }
    // information to create logical device
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());  // number of queue create infos
    deviceInfo.pQueueCreateInfos = queueCreateInfos.data();                            // list of queue  create infos so device can create required queues
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // number of enabled logical device extenstions
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();                      // List of enabled logical device extentions
    // physical device features the logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceInfo.pEnabledFeatures = &deviceFeatures; // Physical device features logical device will use
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Logical Device!");
    }

    // queues are created at the same time as the dice...
    // so we want a refernce to queues
    // From given logical device, of given queue family of given index 0 since only one queue, place refernce in given
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presenetationQueue);
}

void VulkanRenderer::CreateSurface()
{
    // creating a surface create info struct, runs the create surface function
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a VK surface with glfw");
    }
}

void VulkanRenderer::CreateSwapChain(){
    SwapChainDetails details = GetSwapChainDetails(mainDevice.physicalDevice);
    // 1 choose best surface format
    auto surfaceFormat = ChooseBestSurfaceFormat(details.formats);
    // 2 choose best presentation mode
    auto presentationMode = ChooseBestPresentationMode(details.presentationModes);
    // 3 choose swap chain image resoultion
    auto extent = ChooseSwapExtent(details.surfaceCapabilites);

    uint32_t minImageCount = details.surfaceCapabilites.minImageCount +1;
    if (details.surfaceCapabilites.maxImageCount > 0 
    && details.surfaceCapabilites.maxImageCount < minImageCount) 
    {
        minImageCount = details.surfaceCapabilites.maxImageCount;
    }
    VkSwapchainCreateInfoKHR swapCreateInfo = {};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = surface;
    swapCreateInfo.imageFormat = surfaceFormat.format;
    swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapCreateInfo.presentMode = presentationMode;
    swapCreateInfo.imageExtent = extent;
    swapCreateInfo.minImageCount = minImageCount;
    swapCreateInfo.imageArrayLayers = 1;
    swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapCreateInfo.preTransform = details.surfaceCapabilites.currentTransform;
    swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // how to handle blending images , with external graphics eg other windows
    swapCreateInfo.clipped = VK_TRUE;                                           // whether to clip parts of image not visible by screen
    
    // get queuefamily indices
    auto indices = GetQueueFamilies(mainDevice.physicalDevice);
    // If graphics and presentation families are diffrent, then swapchain must let images be shared between families
    if(indices.graphicsFamily != indices.presentationFamily)
    {
        uint32_t queueFamilyIndices[] = {
            (uint32_t)indices.graphicsFamily,
            (uint32_t)indices.presentationFamily,
        };
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // image share handling
        swapCreateInfo.queueFamilyIndexCount = 2;                   // number if queues to share images between
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;    // array of queues to share between
    }
    else 
    {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0;
        swapCreateInfo.pQueueFamilyIndices = nullptr;
    }
    // If old been destroyed and this one replace it the link old one to quickly hand over responsibles
    swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    // create swapchain
    auto result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapCreateInfo , nullptr , &swapChain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a swapchain");
    }
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}
bool VulkanRenderer::CheckValidationLayerSupport()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }
    return true;
}

bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    if (extensionCount == 0)
    {
        return false;
    }
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
    // check for extension
    for (const auto &deviceExtension : deviceExtensions)
    {
        bool hasExtension = false;
        for (const auto &extension : extensions)
        {
            if (strcmp(deviceExtension, extension.extensionName) == 0)
            {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension)
        {
            return false;
        }
    }
    return true;
}
bool VulkanRenderer::CheckInstanceExtentionSupport(std::vector<const char *> *checkExtentions)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount == 0)
    {
        return false;
    }
    std::vector<VkExtensionProperties> extentions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extentions.data());
    // check if given is in list of available extentions
    for (const auto &e : *checkExtentions)
    {
        bool hasExtenstion = false;
        for (const auto &extention : extentions)
        {
            if ((strcmp(e, extention.extensionName)))
            {
                hasExtenstion = true;
                break;
            }
        }
        if (!hasExtenstion)
            return false;
    }
    return true;
}

void VulkanRenderer::Cleanup()
{
    vkDestroySwapchainKHR(mainDevice.logicalDevice , swapChain , nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

bool VulkanRenderer::CheckDeviceSuitable(VkPhysicalDevice device)
{
    // information about the device itself (id, name , type , vendor , etc)
    /**
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device ,&deviceFeatures);
    **/

    QueueFamilyIndices indicies = GetQueueFamilies(device);
    bool extensionSupported = CheckDeviceExtensionSupport(device);
    bool swapChainValid = false;
    if (extensionSupported)
    {
        SwapChainDetails swapChainDetails = GetSwapChainDetails(device);
        swapChainValid = !swapChainDetails.formats.empty() && !swapChainDetails.presentationModes.empty();
    }
    return indicies.IsValid() && extensionSupported && swapChainValid;
}

void VulkanRenderer::GetPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Cant find any GPU's that support vulkan instance.");
    }

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    for (const auto &device : deviceList)
    {
        if (CheckDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

QueueFamilyIndices VulkanRenderer::GetQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, VK_NULL_HANDLE);
    auto queueFamilyList = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
    int index = 0;
    for (const auto &queueFamily : queueFamilyList)
    {
        // fist check if queue family atleast have one 1 queue i nthat family could have no queue
        // Queue can be multible types defined thhrough bitfield. need to bitwise and with vk_queue_*_bit to check if has -
        // required type
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = index; // if queuefamily is valid get index
        }
        // CHECK IF QUEUEFAMILY SUPPORTS PRESENTATION
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentationSupport);
        // check if queue is presentation type (can be both graphics and presentation)
        if (queueFamily.queueCount > 0 && presentationSupport)
        {
            indices.presentationFamily = index;
        }
        // check if queuefamily indicies is in a valid state, stop searching if so.
        if (indices.IsValid())
        {
            break;
        }
        index++;
    }
    return indices;
}

SwapChainDetails VulkanRenderer::GetSwapChainDetails(VkPhysicalDevice device)
{
    SwapChainDetails details;
    // get surface capabillites for the given surface on the given physical device
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surfaceCapabilites);
    // formats
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // presentation modes
    uint32_t modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, nullptr);
    if (modeCount != 0)
    {
        details.presentationModes.resize(modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, details.presentationModes.data());
    }

    return details;
}

// best format is subjective, but ours will be:
// Format : VK_FORMAT_R8G8B8A8_UNORM or VK_FORMAT_B8G8R8A8_UNORM
// ColorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR VulkanRenderer::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
{
    // if only 1 format available and is undefine , then this means all formats are available (no restrictions)
    if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {VK_FORMAT_R8G8B8A8_UNORM , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }
    // if restricted search for optimal format
    for(const auto &f : formats)
    {
        if(f.format == (VK_FORMAT_R8G8B8A8_UNORM || VK_FORMAT_B8G8R8A8_UNORM) 
            && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return f;
        }
    }
    // else return the first
    return formats[0];
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabillites)
{
    // if current extent is at numberic limits, then extent can vary. otherwise it is the size of the window. 
    if(surfaceCapabillites.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabillites.currentExtent;
    }
 
    // if it varies set extents manually
    // get window size
    int width, height;
    glfwGetFramebufferSize(window , &width , &height);
    // create new extent using window size
    VkExtent2D newExtent = {};
    newExtent.width = static_cast<uint32_t>(width);
    newExtent.height = static_cast<uint32_t>(height);

    // saurface also defines max and min so make sure within boundaries by clamping value
    newExtent.width = std::max(surfaceCapabillites.minImageExtent.width, std::min(surfaceCapabillites.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabillites.minImageExtent.height, std::min(surfaceCapabillites.maxImageExtent.height, newExtent.height));
    return newExtent;  
}

VkPresentModeKHR VulkanRenderer::ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &modes)
{
    // look for mailbox presentation mode
    for(const auto &mode : modes)
    {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR) return mode;
    }
    // if not found return fifo as default , garanteed to be available
    return VK_PRESENT_MODE_FIFO_KHR;
}