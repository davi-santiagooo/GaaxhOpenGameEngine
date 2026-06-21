/* Standard */
#include <iostream>

/* Constants */
#define LINUX
//#define WINDOWS
//#define MACOS

// Uncomment for release
//#define NDEBUG

/* Internal */
#include "GogeVkApi.hpp"

int main(int argc, char* argv[]) {
    try {
    VulkanApp app;
    app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
