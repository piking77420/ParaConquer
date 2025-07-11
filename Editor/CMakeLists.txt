cmake_minimum_required(VERSION 3.26)

project(Editor VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/ZI)     ## Edit and Continue
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /INCREMENTAL")
    add_compile_options(/MP)

    ## set editor to be default project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
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


find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::shaderc_combined)




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
    PRIVATE EDITOR_RESOURCE_PATH="${REL_PATH_FROM_TARGET}${EDITOR_RESOURCE_FOLDER_NAME}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)

#target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)

