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

/* Internal */
#include "GogeVkApi.hpp"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class VulkanApp {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    GLFWwindow* window;


    /* Main Vulkan Loop */
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    /* Functions */
    void createInstance();
    std::vector<const char*> getRequiredInstanceExtensions();
    GLFWwindow* getWindow();
};

#endif