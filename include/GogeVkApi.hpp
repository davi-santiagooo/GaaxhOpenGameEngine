#ifndef __GOGE_VK_API_HPP
#define __GOGE_VK_API_HPP

/* Vulkan */
#define VULKAN_HPP_NO_CONSTRUCTORS
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

/* Extern */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/* Standard */
#include <map>

/* Internal */
#include "GogeVkApi.hpp"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class VulkanApp {
public:
    void run();
    void setPhysicalDevice();
    void pickPhysicalDevice();

private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    GLFWwindow* window;
    vk::raii::PhysicalDevice physicalDevice = nullptr;


    /* Main Vulkan Loop */
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    /* Functions */
    void createInstance();
    std::vector<const char*> getRequiredInstanceExtensions();
    bool isDeviceSuitable(vk::raii::PhysicalDevice const& device);
    GLFWwindow* getWindow();
};

#endif