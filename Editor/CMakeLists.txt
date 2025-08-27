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

file(GLOB_RECURSE SOURCE_FILES Src/source/*.c Src/source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Src/Include/*.h Src/Include/*.hpp Src/Inline/*.inl 
Src/Externals/Include/*.h Src/Externals/Include/*.hpp Src/Externals/Include/*.inl)

include_directories(Src/Include)
include_directories(Src/Externals/Include)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

find_package(D3D12 REQUIRED)
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${DXC_INCLUDE_DIRS})

##DXC depencies
if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE dxcompiler.lib)
else()
find_package(Vulkan REQUIRED COMPONENTS Vulkan_dxc_LIBRARY)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan dxc_lib)
endif()


if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs to UnitTest output directory"
    )
endif()


set(EDITOR_RESOURCE_FOLDER_NAME EditorResource)
file(RELATIVE_PATH REL_PATH_FROM_TARGET "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_SOURCE_DIR}")

target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH="${REL_PATH_FROM_TARGET}${EDITOR_RESOURCE_FOLDER_NAME}"
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH_W=L"${REL_PATH_FROM_TARGET}${EDITOR_RESOURCE_FOLDER_NAME}"
)

##  link Core
target_link_libraries(${PROJECT_NAME} PUBLIC Core)

## link Assimp
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

#target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)

