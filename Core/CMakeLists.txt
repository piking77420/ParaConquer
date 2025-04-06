cmake_minimum_required(VERSION 3.26)

project(Core VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect source and header files
file(GLOB_RECURSE SOURCE_FILES src/source/*.c src/source/*.cpp src/externals/source/*.c src/externals/source/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/include/*.h src/include/*.hpp src/inline/*.inl)


#link dir
link_directories(src/externals/lib)
include_directories(src/externals/include/Imgui)


# Create the Core shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_EXPORT)


# Include directories for Core
target_include_directories(${PROJECT_NAME} PUBLIC src/include)
target_include_directories(${PROJECT_NAME} PUBLIC src/inline)
target_include_directories(${PROJECT_NAME} PUBLIC SYSTEM src/externals/include)


# Add directory where MotionCore.lib is located
#link_directories(${CMAKE_SOURCE_DIR}/lib)  

# Link MotionCore.lib to Core
target_link_libraries(${PROJECT_NAME} PRIVATE MotionCore)

# Link VulkanAPI.lib to Core
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanAPI)

# Link glfw3.lib to Core
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_FILE}glfw3.lib)



# share the header
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/MotionCore/MotionCore/src/inline )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/VulkanAPI/src/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/VulkanAPI/src/inline)


