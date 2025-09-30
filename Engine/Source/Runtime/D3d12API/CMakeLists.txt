cmake_minimum_required(VERSION 3.26)

project(D3d12API VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/ZI)       ## Edit and Continue
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


add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE CoreDependencies)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC Include)
target_include_directories(${PROJECT_NAME} PUBLIC Inline)
include_directories(${RUNTIME_SOURCE_DIR}Core/Include) ## include Core Headers

target_link_libraries(${PROJECT_NAME} PUBLIC d3d12 dxgi) ## dont add d3dcompiler we use DXC
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

if (PROFILING)
target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_IMPORTS)
else()
endif()