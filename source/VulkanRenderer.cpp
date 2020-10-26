#include "../includes/VulkanRenderer.h"
int VulkanRenderer::Init(GLFWwindow* newWindow){
    window = newWindow;
    try
    {
        CreateInstance();
        CreateLogicalDevice();
    }
    catch(const std::exception& e)
    {
        std::printf("ERROR %s \n", e.what());
        return EXIT_FAILURE;
    }
    
    return 0;
}

void VulkanRenderer::CreateInstance(){
    // information of the application itself
    // most data here wont affect the program is for developer convinece
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_1;
    applicationInfo.pApplicationName = "Vulkan Test Application";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    applicationInfo.pEngineName = "None";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1,0,0);

    // creation information for a vk instance witch is a vulkan instance.
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    
    // create a list to hold the instance extenstions
    auto extentions = std::vector<const char*>();
    uint32_t glfwExtCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    // add GLFW extenstion to list of extenstions
    for(size_t i = 0; i < glfwExtCount; ++i){
        extentions.push_back(glfwExtensions[i]);
    }
    // check if INSTANCE extentions are supported
    if (!CheckInstanceExtentionSupport(&extentions)){
        throw std::runtime_error("VKInstance does not support required extentions!");
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t> (extentions.size());
    createInfo.ppEnabledExtensionNames = extentions.data();
    // TODO setup validation
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    // Create instance
    VkResult result = vkCreateInstance(&createInfo,nullptr,&instance);
    if(result != VK_SUCCESS){
        throw std::runtime_error("Failed to create a vulkan instance");
    }
}

void VulkanRenderer::CreateLogicalDevice(){
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    // get queuefamily  indicies for the choosen physical device
    auto indicies = GetQueueFamilies(mainDevice.physicalDevice);
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indicies.graphicsFamily; // the index of the family to create a queue from
    queueCreateInfo.queueCount = 1; // number of queues to create.
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority; // vulkan needs to know how to handle multpible queues, so decide prio 1 is highest 0 lowest
    // information to create logical device
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1; // number of queue create infos
    deviceInfo.pQueueCreateInfos = &queueCreateInfo; // list of queue  create infos so device can create required queues
    deviceInfo.enabledExtensionCount = 0; // number of enabled logical device extenstions
    deviceInfo.ppEnabledExtensionNames = nullptr; // List of enabled logical device extentions
    // physical device features the logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceInfo.pEnabledFeatures = &deviceFeatures; // Physical device features logical device will use
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);

}

bool VulkanRenderer::CheckInstanceExtentionSupport(std::vector<const char*> * checkExtentions){
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr , &extCount , nullptr);
    std::vector<VkExtensionProperties> extentions(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr , &extCount , extentions.data());
    // check if given is in list of available extentions
    for(const auto& e : *checkExtentions){
        bool hasExtenstion = false;
        for(const auto &extention : extentions){
            if((strcmp(e,extention.extensionName))){
                hasExtenstion = true;
                break;
            }
        }
        if(!hasExtenstion) return false;
    }
    return true;
}

void VulkanRenderer::Cleanup(){
    vkDestroyInstance(instance,nullptr);
}
bool VulkanRenderer::CheckDeviceSuitable(VkPhysicalDevice device){
    // information about the device itself (id, name , type , vendor , etc)
    /**
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device ,&deviceFeatures);
    **/
    
    // chekc if queues are supported
    QueueFamilyIndices indicies = GetQueueFamilies(device);
    return indicies.IsValid();
}
void VulkanRenderer::GetPhysicalDevice(){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Cant find any GPU's that support vulkan instance.");
    }
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
    for(const auto &device : deviceList ){
        if(CheckDeviceSuitable(device)){
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

QueueFamilyIndices VulkanRenderer::GetQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device ,&queueFamilyCount , nullptr);
    auto queueFamilyList = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device , &queueFamilyCount , queueFamilyList.data());
    int index = 0;
    for (const auto &queueFamily : queueFamilyList){
        // fist check if queue family atleast have one 1 queue i nthat family could have no queue
        // Queue can be multible types defined thhrough bitfield. need to bitwise and with vk_queue_*_bit to check if has -
        // required type
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = index; // if queuefamily is valid get index
        }
        // check if queuefamily indicies is in a valid state, stop searching if so.
        if (indices.IsValid()){
            break;
        }
        index++;
    }
    return indices;
}
