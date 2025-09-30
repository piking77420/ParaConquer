cmake_minimum_required(VERSION 3.26)

project(Editor VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/ZI)     ## Edit and Continue
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /INCREMENTAL")
    add_compile_options(/MP)

    ## set editor to be default project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# Set C++ version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add sources
file(GLOB_RECURSE SOURCE_FILES Source/*.c Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Include/*.h Include/*.hpp Inline/*.inl)

# exe
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC Include)
target_include_directories(${PROJECT_NAME} PUBLIC Inline)

## Assimp
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

# Begin Dependicies

# Core
target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)  
target_link_libraries(${PROJECT_NAME} PRIVATE Core)

##DXC depencies
if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE dxcompiler.lib)
else()
find_package(Vulkan REQUIRED COMPONENTS Vulkan_dxc_LIBRARY)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan dxc_lib)
endif()

# End Dependencies
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs to UnitTest output directory"
    )
endif()

# Secondary Data
set(EDITOR_RESOURCE_FOLDER_NAME EditorResource)
file(RELATIVE_PATH REL_PATH_FROM_TARGET "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_SOURCE_DIR}")

## TODO
target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH="${REL_PATH_FROM_TARGET}Engine/Source/Editor/EditorResource"
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH_W=L"${REL_PATH_FROM_TARGET}Engine/Source/Editor/EditorResource"
)


