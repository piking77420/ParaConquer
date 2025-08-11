cmake_minimum_required(VERSION 3.26)

project(Core VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
   add_compile_options(/ZI)       ## Edit and Continue
   set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /INCREMENTAL")

    add_compile_options(/MP)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIB_FOLDER_DEBUG ${CMAKE_SOURCE_DIR}/Src/Externals/Lib/Debug)
set(LIB_FOLDER_RELEASE ${CMAKE_SOURCE_DIR}/Src/Externals/Lib/Release)

# Collect source and header files
file(GLOB_RECURSE SOURCE_FILES Src/Source/*.c Src/Source/*.cpp Src/externals/Source/*.c Src/externals/Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Src/Include/*.h Src/Include/*.hpp Src/Inline/*.inl)


#link dir
link_directories(Src/Externals/lib)
include_directories(Src/Externals/include/Imgui)


# Create the Core shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_EXPORT)
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_API)


# Include directories for Core
target_include_directories(${PROJECT_NAME} PUBLIC Src/include)
target_include_directories(${PROJECT_NAME} PUBLIC Src/Inline)
target_include_directories(${PROJECT_NAME} PUBLIC SYSTEM Src/Externals/Include)


# Add directory where MotionCore.lib is located
#link_directories(${CMAKE_SOURCE_DIR}/lib)  

# Link MotionCore.lib to Core
target_link_libraries(${PROJECT_NAME} PRIVATE MotionCore)

# Link VulkanAPI.lib to Core
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanAPI)

target_link_directories(${PROJECT_NAME} PRIVATE
  "$<$<CONFIG:Debug>:${LIB_FOLDER_DEBUG}>"
  "$<$<CONFIG:Release>:${LIB_FOLDER_RELEASE}>"
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

# Link Profiler.lib/Interface to Core 
target_link_libraries(${PROJECT_NAME} PUBLIC Profiler)


if (PROFILING)
    target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_IMPORTS)
else()
target_link_libraries(${PROJECT_NAME} PUBLIC Profiler)

endif()


# share the header
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/inline )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/VulkanAPI/src/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/VulkanAPI/src/inline)


