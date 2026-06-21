#include "GogeVkApi.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan_raii.hpp>


/* Validation Layers */
const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


/* Public  */
void VulkanApp::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

/*==========================================*/
/* pickPhysicalDevice                       */
/*==========================================*/
/* @desc selects physical device based on   */
/* performance and capabilities             */
/*==========================================*/
void VulkanApp::pickPhysicalDevice() {

    // Physical devices detection
    auto physicalDevices = instance.enumeratePhysicalDevices();
    if(physicalDevices.empty()) {
        throw std::runtime_error("Failed to find devices with Vulkan support");
    }

    // Physical device selection
    std::multimap<int, vk::raii::PhysicalDevice> candidates;
    for(const auto& pd : physicalDevices) {
        if(!isDeviceSuitable(pd)) continue;

        auto deviceProperties = pd.getProperties2();
        auto deviceFeatures = pd.getFeatures2();
        uint32_t deviceScore = 0;

        if(deviceProperties.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            deviceScore += 1000;
        }

        deviceScore += deviceProperties.properties.limits.maxImageDimension2D;

        candidates.insert(std::make_pair(deviceScore, pd));
    }
    if(!candidates.empty() && candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin() -> second;
    }
    else {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

void VulkanApp::setPhysicalDevice() {
    //TODO
}

/* Main Vulkan Loop */
void VulkanApp::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Test", nullptr, nullptr);
}
void VulkanApp::initVulkan() {
    createInstance();
    if(this->physicalDevice != nullptr) {
        pickPhysicalDevice();
    }
}
void VulkanApp::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
void VulkanApp::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}


/* Functions */
void VulkanApp::createInstance() {
    constexpr vk::ApplicationInfo appInfo { 
        .pApplicationName       = "Vulkan App",
        .applicationVersion     = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName            = "No Engine",
        .engineVersion          = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion             = vk::ApiVersion14
    };
    
    // Extensions
    auto requiredExtensions = getRequiredInstanceExtensions();

    auto extensionProperties = context.enumerateInstanceExtensionProperties();
    auto unsupportedPropertyIt =
        std::ranges::find_if(requiredExtensions, [&extensionProperties](auto const &requiredExtension) {
            return std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) {
                return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        });
    });
	if(unsupportedPropertyIt != requiredExtensions.end()) {
		throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
	}

    // Layers
    std::vector<char const*> requiredLayers;
    if(enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }
    
    auto layerProperties = context.enumerateInstanceLayerProperties();
    auto unsupportedLayerIt = 
    std::ranges::find_if(requiredLayers, [&layerProperties](auto const &requiredLayer) {
        return std::ranges::none_of(layerProperties, [requiredLayer](auto const &layerProperty) {
            return strcmp(layerProperty.layerName, requiredLayer) == 0;
        });
    });
    if(unsupportedLayerIt != requiredLayers.end()) {
        throw std::runtime_error("Required Vulkan layer not supported: " + std::string(*unsupportedLayerIt));
    }

    // Instance
    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    instance = vk::raii::Instance(context, createInfo);
}

std::vector<const char*> VulkanApp::getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

bool VulkanApp::isDeviceSuitable(vk::raii::PhysicalDevice const& device) {
    auto deviceProperties = device.getProperties2();
    auto deviceFeature = device.getFeatures2();

    bool supportsVulkan1_3 = deviceProperties.properties.apiVersion >= vk::ApiVersion13;

    auto queueFamilies = device.getQueueFamilyProperties();
    bool supportsGraphics =
        std::ranges::any_of(queueFamilies, [](auto const &qfp) {
            return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

    std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName};

    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions =
    std::ranges::all_of( requiredDeviceExtension, [&availableDeviceExtensions]( auto const & requiredDeviceExtension ) {
        return std::ranges::any_of( availableDeviceExtensions, [requiredDeviceExtension]( auto const & availableDeviceExtension ) {
            return strcmp( availableDeviceExtension.extensionName, requiredDeviceExtension ) == 0;
        } );
    } );

    auto features = physicalDevice.template getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    >();
    bool supportsRequiredFeatures =
        features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
        features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
        features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

GLFWwindow* VulkanApp::getWindow() {
    return window;
}