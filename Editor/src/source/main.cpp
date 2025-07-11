
#include <iostream>
#include "editor.hpp"


#define RENDER_DOC_INJECTION 1


int main(int argc, char* argv[])
{

#if RENDER_DOC_INJECTION
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