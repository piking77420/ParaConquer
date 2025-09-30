cmake_minimum_required(VERSION 3.26)


project(Profiler VERSION 0.1.0 LANGUAGES C CXX)

set(PROFILER_BUILD_DYNAMIC_IMAGE OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)

endif()

file(GLOB_RECURSE SOURCE_FILES Source/*.c Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Include/*.h Include/*.hpp Inline/*.inl)

if (PROFILING)

set(TRACY_PUBLIC_DIR "${THIRD_PARTY_DIR}/tracy/public")
set(TRACY_CLIENT_SRC "${TRACY_PUBLIC_DIR}/TracyClient.cpp")

add_library(${PROJECT_NAME} SHARED
    ${SOURCE_FILES}
    ${TRACY_CLIENT_SRC}
    ${HEADER_FILES}
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE TRACY_EXPORTS PROFILER_EXPORTS)
target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_ENABLE)

# Public include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        Include        
        Inline         
        ${TRACY_PUBLIC_DIR}
)

else()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE Include)
target_include_directories(${PROJECT_NAME} INTERFACE Inline)

endif()
