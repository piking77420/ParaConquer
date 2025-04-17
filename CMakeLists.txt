cmake_minimum_required(VERSION 3.26)

option(PROFILING "Build profiling dependencies" OFF)


project(ParaConquer VERSION 0.1.0 LANGUAGES C CXX)


#if(MSVC)
 #   add_compile_options(/Zc:__cplusplus)
#endif()

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
add_subdirectory(Core)
add_subdirectory(Editor)