cmake_minimum_required(VERSION 3.26)


# Collect source and header files
file(GLOB_RECURSE CORE_HEADER_FILES ../Core/Include/*.h ../Core/Include/*.hpp ../Core/Inline/*.inl ../Core/Include/Source/.*h ../Core/Include/Source/.*hpp 
../Core/Include/*.h ../Core/Include/*.hpp)

list(APPEND CORE_HEADER_FILES)

add_library(CoreDependencies INTERFACE ${THIRD_PARTY_DIR}/lzav/lzav.h)

if (PROFILING)
    target_compile_definitions(CoreDependencies INTERFACE TRACY_IMPORTS)
endif()

target_link_libraries(CoreDependencies
    INTERFACE
        nlohmann_json
        magic_enum
        Profiler
        MotionCore
        )