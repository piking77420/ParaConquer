cmake_minimum_required(VERSION 4.0)

option(PROFILING "Build profiling dependencies" OFF)
option(BUILD_TEST "Build UnitTest" OFF)

add_compile_definitions(WITH_EDITOR=1)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  )


project(ParaConquer VERSION 0.1.0 LANGUAGES C CXX)

#external first
if (PROFILING)

#tracy-backend
add_subdirectory(tracy)
add_compile_definitions(PROFILING)
add_compile_definitions(TRACY_ENABLE)

endif()

## tracy-profiler
add_subdirectory(Profiler)
add_subdirectory(MotionCore/MotionCore)
add_subdirectory(VulkanAPI)

if (WIN32)
add_subdirectory(D3d12API)
endif()

add_subdirectory(Core)


#unit Test
if(BUILD_TEST)
add_subdirectory(UnitTest)
endif()

add_subdirectory(Editor)