cmake_minimum_required(VERSION 3.26)


project(ParaConquer VERSION 0.1.0 LANGUAGES C CXX)

#if(MSVC)
 #   add_compile_options(/Zc:__cplusplus)
#endif()

#external first
add_subdirectory(MotionCore/MotionCore)


add_subdirectory(VulkanAPI)
add_subdirectory(Core)
add_subdirectory(Editor)