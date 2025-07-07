#include "editor.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;
using namespace Tbx;


int main(int argc, char* argv[])
{
    bool appShounldClose = false;
    while (!appShounldClose)
    {
        appShounldClose = true;
        Editor editor;

        editor.Init();
        editor.Run(&appShounldClose);
        editor.Destroy();
    }

  


    return 0;
}