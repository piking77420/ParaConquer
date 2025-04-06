cmake_minimum_required(VERSION 3.26)

project(Editor VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES src/source/*.c src/source/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/include/*.h src/include/*.hpp src/inline/*.inl 
src/externals/include/*.h src/externals/include/*.hpp src/externals/include/*.inl)

include_directories(src/include)
include_directories(src/externals/include)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

# to do find a betetr way
target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/glslang.lib)

target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/SPIRV-Tools.lib)


target_link_libraries(${PROJECT_NAME} PUBLIC Core)
#target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)

