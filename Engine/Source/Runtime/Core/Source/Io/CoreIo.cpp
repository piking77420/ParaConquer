#include "Io/CoreIo.hpp"
#include <Glfw/Glfw3.h>


using namespace PC_CORE;

CoreIo::CoreIo()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

CoreIo::~CoreIo()
{
    glfwTerminate();
}


void CoreIo::PoolEvent()
{
    glfwPollEvents();
}
