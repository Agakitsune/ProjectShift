cmake_minimum_required(VERSION 3.30)

project(project_shift)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

include(FetchContent)

cmake_policy(SET CMP0148 OLD)

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG master # Latest release
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b-docking # Latest release with docking
)

set(OPTION_BUILD_EXAMPLES OFF)

FetchContent_MakeAvailable(glfw3)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    FetchContent_MakeAvailable(imgui)
endif()

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES
    "./src/*.cpp"
    
    "./src/math/*.cpp"
    "./src/math/matrix/*.cpp"
    "./src/math/vector/*.cpp"

    "./src/graphics/vulkan/*.cpp"
    "./src/graphics/vulkan/khr/*.cpp"

    "./src/graphics/meshes/*.cpp"
    "./src/graphics/*.cpp"
    
    "./src/*.c"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB IMGUI_SOURCES "${imgui_SOURCE_DIR}/*cpp")

    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    
    ${SOURCES}

    ${IMGUI_SOURCES}
)

# add compile option if cmake is run in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE ALCHEMIST_DEBUG)
else()
    message(STATUS "Building in Release mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE ALCHEMIST_RELEASE)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ALCHEMIST_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_options(${PROJECT_NAME} PRIVATE
    -g
    # -fsanitize=address
    -fno-omit-frame-pointer
)

# target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

target_include_directories(${PROJECT_NAME} PRIVATE ${glfw3_SOURCE_DIR}/include)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR}/backends)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)
