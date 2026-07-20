cmake_minimum_required(VERSION 3.26)

project(VulkanAPI VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /INCREMENTAL")
    add_compile_options(/MP)
endif()

# Set C++ version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Collect source and header files
file(GLOB_RECURSE SOURCE_FILES Source/*.c Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Include/*.h Include/*.hpp Inline/*.inl Include/Source/.*h Include/Source/.*hpp 
Include/*.h Include/*.hpp)


find_package(Vulkan REQUIRED)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE CoreDependencies)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC Include Inline)
target_include_directories(${PROJECT_NAME} PRIVATE ${RUNTIME_SOURCE_DIR}/Core/Include)

#Dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)   
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS}/vma)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)


