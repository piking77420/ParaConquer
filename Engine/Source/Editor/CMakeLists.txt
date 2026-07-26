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
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE meshoptimizer)

# Begin Dependicies

# Core
target_compile_definitions(${PROJECT_NAME} PRIVATE PC_CORE_IMPORT=1)  
target_link_libraries(${PROJECT_NAME} PRIVATE Core)

##DXC depencies

## use same out folder for each config 
## build one time 
set(DXC_BINARY_DIR
    "${CMAKE_SOURCE_DIR}/out/ThirdParty/DirectXShaderCompiler/${CMAKE_CXX_COMPILER_ID}"
)
set(DXC_LIBRARY
    ${DXC_BINARY_DIR}/lib/libdxcompiler.so
)
ExternalProject_Add(DXCBuild
    SOURCE_DIR "${DXC_SOURCE_DIR}"
    BINARY_DIR "${DXC_BINARY_DIR}"

    CONFIGURE_COMMAND
        ${CMAKE_COMMAND}
        -S "${DXC_SOURCE_DIR}"
        -B "${DXC_BINARY_DIR}"
        -G "${CMAKE_GENERATOR}"
        -C "${DXC_SOURCE_DIR}/cmake/caches/PredefinedParams.cmake"
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DLLVM_INCLUDE_TESTS=OFF
        -DHLSL_INCLUDE_TESTS=OFF
        -DSPIRV_BUILD_TESTS=OFF
        -DLLVM_BUILD_TESTS=OFF
        -DLLVM_BUILD_EXAMPLES=OFF

    BUILD_COMMAND
        ${CMAKE_COMMAND}
        --build "${DXC_BINARY_DIR}"
        --target dxcompiler

    INSTALL_COMMAND ""

    BUILD_BYPRODUCTS
        "${DXC_LIBRARY}"

    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
)

add_library(DXC::DXC SHARED IMPORTED GLOBAL)

set_target_properties(DXC::DXC PROPERTIES
    IMPORTED_LOCATION "${DXC_LIBRARY}"

    INTERFACE_INCLUDE_DIRECTORIES
        "${DXC_SOURCE_DIR}/include"
)

add_dependencies(DXC::DXC DXCBuild)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    DXC::DXC
)


if(MSVC)

function(CopyRuntimeDLL TargetName)
    foreach(Dep ${ARGN})
        if(NOT TARGET ${Dep})
            message(WARNING "CopyRuntimeDLL: '${Dep}' is not a target, skipping")
            continue()
        endif()

        add_custom_command(TARGET ${TargetName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<$<STREQUAL:$<TARGET_PROPERTY:${Dep},TYPE>,SHARED_LIBRARY>:$<TARGET_FILE:${Dep}>>"
                "$<TARGET_FILE_DIR:${TargetName}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<$<AND:$<BOOL:${MSVC}>,$<STREQUAL:$<TARGET_PROPERTY:${Dep},TYPE>,SHARED_LIBRARY>>:$<TARGET_PDB_FILE:${Dep}>>"
                "$<TARGET_FILE_DIR:${TargetName}>"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endforeach()
endfunction()
   
CopyRuntimeDLL(${PROJECT_NAME} Core assimp::assimp glfw)

if (PROFILING)
CopyRuntimeDLL(${PROJECT_NAME} Profiler)
endif()

endif()

# Secondary Data
set(EDITOR_RESOURCE_FOLDER_NAME EditorResource)
file(RELATIVE_PATH REL_PATH_FROM_TARGET "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_SOURCE_DIR}")

## TODO
target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH="${REL_PATH_FROM_TARGET}../../Engine/Source/Editor/EditorResource"
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE EDITOR_RESOURCE_PATH_W=L"${REL_PATH_FROM_TARGET}../../Engine/Source/Editor/EditorResource"
)

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

