cmake_minimum_required(VERSION 3.26)


project(Imgui VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

file(
    GLOB_RECURSE
    SOURCE_FILES
    ## source
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
)

file(
    GLOB_RECURSE
    HEADER_FILES
    ## include
    ${THIRD_PARTY_DIR}/imgui/imgui.h
    ${THIRD_PARTY_DIR}/imgui/imconfig.h
    ${THIRD_PARTY_DIR}/imgui/imgui_internal.h
    ${THIRD_PARTY_DIR}/imgui/imstb_rectpack.h
    ${THIRD_PARTY_DIR}/imgui/imstb_textedit.h
    ${THIRD_PARTY_DIR}/imgui/imstb_truetype.h
)

if(WIN32)

file(
    GLOB_RECURSE
    SOURCE_FILES
    ## source
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.cpp
)

file(
    GLOB_RECURSE
    HEADER_FILES
    ## include
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h.h
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.cpp
)

elseif(UNIX)

file(
    GLOB_RECURSE
    SOURCE_FILES
    ## source
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

file(
    GLOB_RECURSE
    HEADER_FILES
    ## include
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h.h
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_BUILD_DLL)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC d3d12 dxgi d3dcompiler)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/imgui)
