cmake_minimum_required(VERSION 3.26)

project(Core VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  )

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
file(GLOB_RECURSE HEADER_FILES Include/*.h Include/*.hpp Inline/*.inl)

# Create the Core shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC CoreDependencies)
target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_EXPORT)


# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC Include)
target_include_directories(${PROJECT_NAME} PUBLIC Inline)
# ThirdParty Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_DIR})

# Begin Dependencies

## D3D12 API
if (WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC D3d12API)
endif()

## VULKAN API
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanAPI)

# Imgui
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_BUILD_DLL)
target_link_libraries(${PROJECT_NAME} PUBLIC Imgui)

## MotionCore
target_link_libraries(${PROJECT_NAME} PUBLIC MotionCore)

## Json
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)

## Magic Enum
target_link_libraries(${PROJECT_NAME} PUBLIC magic_enum)

## Profiler
target_link_libraries(${PROJECT_NAME} PUBLIC Profiler)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
    target_link_options(${PROJECT_NAME} PUBLIC -stdlib=libc++)
endif()


# End Dependencies

 



