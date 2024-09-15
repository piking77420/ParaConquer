#include "io/core_io.hpp"
#include <GLFW/glfw3.h>


using namespace PC_CORE;

void CoreIo::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void CoreIo::Destroy()
{
    glfwTerminate();
}

void CoreIo::PoolEvent()
{
    glfwPollEvents();
}
