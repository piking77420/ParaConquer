
#define BOOST_SYSTEM_NO_DEPRECATED

#include <iostream>
#include "editor.hpp"



int main(int argc, char* argv[])
{
    
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