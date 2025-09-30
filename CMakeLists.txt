cmake_minimum_required(VERSION 3.26)

#  Options 
option(PROFILING "Build profiling dependencies" OFF)
option(BUILD_TEST "Build UnitTest" OFF)

# Compile Definitions
add_compile_definitions(WITH_EDITOR=1)

# Variable
set(EDITOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Editor")
set(RUNTIME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Runtime")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/ThirdParty")
set(PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Plugins")

# Solution
project(ParaConquer VERSION 0.1.0 LANGUAGES C CXX)

## Assimp 
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE) ## Force Assimp to not build Unit test
add_subdirectory(${THIRD_PARTY_DIR}/assimp)

## Tracy
add_subdirectory(${THIRD_PARTY_DIR}/tracy)

## GLFW
set(GLFW_INSTALL ON CACHE BOOL "Enable GLFW install" FORCE)
add_subdirectory(${THIRD_PARTY_DIR}/glfw)

## SPRIV-REFLECT
set(SPIRV_REFLECT_STATIC_LIB TRUE)
add_subdirectory(${THIRD_PARTY_DIR}/SPIRV-Reflect)

## Json
add_subdirectory(${THIRD_PARTY_DIR}/json)

## Magic Enum
add_subdirectory(${THIRD_PARTY_DIR}/magic_enum)

add_subdirectory(${RUNTIME_SOURCE_DIR}/CoreDependencies)
add_subdirectory(${RUNTIME_SOURCE_DIR}/Core)

## MotionCore
add_subdirectory(${THIRD_PARTY_DIR}/MotionCore/MotionCore)
set(MATH_INCLUDE_PATH ${THIRD_PARTY_DIR}/MotionCore/MotionCore/Include  ${THIRD_PARTY_DIR}/MotionCore/MotionCore/Inline)

## Profiler 
add_subdirectory(${PLUGINS_DIR}/Profiler)

## Imgui 
add_subdirectory(${PLUGINS_DIR}/Imgui)

## D3D12 API
if (WIN32)
add_subdirectory(${RUNTIME_SOURCE_DIR}/D3d12API)
endif()

## VULKAN API
add_subdirectory(${RUNTIME_SOURCE_DIR}/VulkanAPI)

# Editor
add_subdirectory(${EDITOR_SOURCE_DIR})

if(BUILD_TEST)
add_subdirectory(${RUNTIME_SOURCE_DIR}/UnitTest)
endif()