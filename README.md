# Project Shift

A little project made with Vulkan, Imgui and GLFW3

## Compilation

You will need:
* CMake (version 3.30 at least)
* [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)

### Linux

At the root of the repo, run
```
cmake -B build -DCMAKE_BUILD_TYPE=<BUILD-TYPE>
```

*BUILD-TYPE* can be **Debug** or **Release**

This will build the project with Imgui or not.


### Windows

#### Visual Studio

Import the repo as a project, if you have an error with VS not finding Vulkan::Vulkan, you may have to check your Vulkan installation

#### MinGW

It's the same as Linux
