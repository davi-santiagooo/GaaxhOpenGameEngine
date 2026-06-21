cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(target GaaxhOpenGE)
project(${target})

add_executable(${target})

find_package(Vulkan REQUIRED)
find_package(slang CONFIG HINTS "$ENV{VULKAN_SDK}/lib/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

find_package(glfw3 3.4 REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
target_sources(${target} PRIVATE ${SOURCES})

target_include_directories(${target} PUBLIC include)

target_link_libraries(
    ${target}
    PUBLIC
        Vulkan::Vulkan
        glfw
        glm::glm
)
