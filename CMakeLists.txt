cmake_minimum_required(VERSION 4.0)

option(PROFILING "Build profiling dependencies" OFF)
option(BUILD_TEST "Build UnitTest" ON)

add_compile_definitions(WITH_EDITOR=1)


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


#unit Test
if(BUILD_TEST)
add_subdirectory(UnitTest)
endif()

add_subdirectory(Editor)