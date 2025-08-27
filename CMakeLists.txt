cmake_minimum_required(VERSION 3.26)

option(PROFILING "Build profiling dependencies" ON)
option(BUILD_TEST "Build UnitTest" ON)


## Force Assimp to not build Unit test
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)

add_compile_definitions(WITH_EDITOR=1)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  )


project(ParaConquer VERSION 0.1.0 LANGUAGES C CXX)

## Third Party
add_subdirectory(assimp)

## tracy-profiler
if (PROFILING)
#tracy-backend
add_subdirectory(tracy)
add_compile_definitions(PROFILING)
add_compile_definitions(TRACY_ENABLE)
endif()
add_subdirectory(Profiler)

# End of Third Party

## static 
add_subdirectory(VulkanAPI)
if (WIN32)
add_subdirectory(D3d12API)
endif()
add_subdirectory(MotionCore/MotionCore)

## dynamic
add_subdirectory(Core)


#unit Test
if(BUILD_TEST)
add_subdirectory(UnitTest)
endif()

add_subdirectory(Editor)