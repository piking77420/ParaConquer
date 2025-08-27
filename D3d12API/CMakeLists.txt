cmake_minimum_required(VERSION 3.26)

set(BUILD_SHARED_LIBS OFF)
project(D3d12API VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/ZI)       ## Edit and Continue
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /INCREMENTAL")
    add_compile_options(/MP)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE SOURCE_FILES Src/Source/*.c Src/Source/*.cpp Src/Externals/Source/*.c Src/Externals/Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Src/Include/*.h Src/Include/*.hpp Src/Inline/*.inl Src/Include/Source/.*h Src/Include/Source/.*hpp Src/Externals/Include/*.h Src/Externals/Include/*.hpp)

include_directories(Src/Include)
include_directories(${CMAKE_SOURCE_DIR}/Core/Src/Include)

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${D3D12_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${D3D12_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC Src/Externals/Include)

if (PROFILING)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Profiler/Src/Include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/tracy/public)
target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_IMPORTS)

else()
target_link_libraries(${PROJECT_NAME} PUBLIC Profiler)

endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/Src/Include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/Inline )

target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Core/src/Externals/Include/)

target_include_directories(${PROJECT_NAME} PUBLIC Src/Include)
target_include_directories(${PROJECT_NAME} PUBLIC Src/Inline)
