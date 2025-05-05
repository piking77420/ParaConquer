cmake_minimum_required(VERSION 3.26)

project(VulkanAPI VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES src/source/*.c src/source/*.cpp src/externals/source/*.c src/externals/source/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/include/*.h src/include/*.hpp src/inline/*.inl src/include/source/.*h src/include/source/.*hpp)


include_directories(src/include)
include_directories(${CMAKE_SOURCE_DIR}/Core/src/include)


find_package(Vulkan REQUIRED)
add_library(${PROJECT_NAME}  ${SOURCE_FILES} ${HEADER_FILES})


target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)   
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC src/externals/include)



target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/inline )

target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Core/src/externals/include/)

target_include_directories(${PROJECT_NAME} PUBLIC src/include)
target_include_directories(${PROJECT_NAME} PUBLIC src/inline)
