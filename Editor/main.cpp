#include <iostream>
#include "core_header.hpp"
#include "app.hpp"
#include "editor.hpp"
#include "math/utils.hpp"

#include "world/transform.hpp"
#include "scripting/scripting_lua.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


int main(int argc, char* argv[])
{
    Editor editor;

    editor.Init();
    editor.Run();
    editor.Destroy();
    
    
    return 0;
}
