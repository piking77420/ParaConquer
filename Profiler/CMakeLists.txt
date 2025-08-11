cmake_minimum_required(VERSION 3.26)


project(Profiler VERSION 0.1.0 LANGUAGES C CXX)
set(PROFILER_BUILD_DYNAMIC_IMAGE OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)

endif()

file(GLOB_RECURSE SOURCE_FILES Src/Source/*.c Src/Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Src/Include/*.h Src/Include/*.hpp Src/Inline/*.inl)

if (PROFILING)

set(TRACY_PUBLIC_DIR "${CMAKE_SOURCE_DIR}/tracy/public")
set(TRACY_CLIENT_SRC "${TRACY_PUBLIC_DIR}/TracyClient.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${TRACY_CLIENT_SRC} ${HEADER_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE TRACY_EXPORTS)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROFILER_EXPORTS)
target_include_directories(${PROJECT_NAME} PUBLIC Src/Include)
target_include_directories(${PROJECT_NAME} PUBLIC Src/Inline)
target_include_directories(${PROJECT_NAME} PUBLIC ${TRACY_PUBLIC_DIR})

else()
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE Src/Include)
target_include_directories(${PROJECT_NAME} INTERFACE Src/Inline)
endif()
