#include "GogeVkApi.hpp"


/* Validation Layers */
const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


/* Main Vulkan Loop */
void VulkanApp::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Test", nullptr, nullptr);
}
void VulkanApp::initVulkan() {
    createInstance();
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

GLFWwindow* VulkanApp::getWindow() {
    return window;
}