
#include <Iostream>
#include "Editor.hpp"


#define WAIT_DEBUG_GPU_INJECTION 0

int main(int argc, char* argv[])
{

#if WAIT_DEBUG_GPU_INJECTION
std::getchar();
#endif

    bool appShounldClose = false;
    while (!appShounldClose)
    {
        appShounldClose = true;
        PC_EDITOR_CORE::Editor editor;
        editor.Init();
        editor.Run(&appShounldClose);
        editor.Destroy();
    }

  

    return 0;
}