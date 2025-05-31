#include "editor.hpp"

#include "reflection/reflector.hpp"
#include "rendering/material.hpp"
#include "resources/resource.hpp"
#include "serialize/iseriazable.h"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;
using namespace Tbx;

#ifdef _DEBUG


#endif



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