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

find_package(Vulkan REQUIRED COMPONENTS glslang SPIRV-Tools)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

# to do find a betetr way
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::glslang)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::SPIRV-Tools)

#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/glslangd.lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/SPIRV-Toolsd.lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/SPIRV-Tools-optd.lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/glslang-default-resource-limitsd.lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/MachineIndependentd.lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE C:/VulkanSDK/1.4.304.0/Lib/GenericCodeGend.lib)

cmake_path(GET Vulkan_LIBRARY PARENT_PATH  VULKAN_LIBRARIES_PATH )

set(SPRIV_TOOL_OPT debug ${VULKAN_LIBRARIES_PATH}/SPIRV-Tools-optd.lib optimized ${VULKAN_LIBRARIES_PATH}/SPIRV-Tools-opt.lib)
set(glslang_DEFAULT_RESOURCE_LIMIT debug ${VULKAN_LIBRARIES_PATH}/glslang-default-resource-limitsd.lib optimized ${VULKAN_LIBRARIES_PATH}/glslang-default-resource-limits.lib)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SPRIV_TOOL_OPT})
target_link_libraries(${PROJECT_NAME} PRIVATE ${glslang_DEFAULT_RESOURCE_LIMIT})

#Copy Core DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:Core> 
    $<TARGET_FILE_DIR:Editor>)

if (PROFILING)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:Profiler> 
    $<TARGET_FILE_DIR:Editor>)

endif()


set(EDITOR_RESOURCE_FOLDER_NAME EditorResource)
file(RELATIVE_PATH REL_PATH_FROM_TARGET "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_SOURCE_DIR}")

target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH="${REL_PATH_FROM_TARGET}${EDITOR_RESOURCE_FOLDER_NAME}/"
)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)
#target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)

