cmake_minimum_required(VERSION 3.26)


project(Imgui VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()
set(SOURCE_FILES
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
)

set(HEADER_FILES
    ${THIRD_PARTY_DIR}/imgui/imgui.h
    ${THIRD_PARTY_DIR}/imgui/imconfig.h
    ${THIRD_PARTY_DIR}/imgui/imgui_internal.h
    ${THIRD_PARTY_DIR}/imgui/imstb_rectpack.h
    ${THIRD_PARTY_DIR}/imgui/imstb_textedit.h
    ${THIRD_PARTY_DIR}/imgui/imstb_truetype.h
)

list(APPEND SOURCE_FILES
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

list(APPEND HEADER_FILES
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.h
)

if(WIN32)
    list(APPEND SOURCE_FILES
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.cpp
    )

    list(APPEND HEADER_FILES
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.h
    )
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(Imgui STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

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
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/imgui ${THIRD_PARTY_DIR}/imgui/backends)
