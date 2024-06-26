#include <iostream>

#include "app.hpp"
#include "editor.hpp"

#include "world/transform.hpp"

int main(int argc, char* argv[])
{
    PC_EDITOR_CORE::Editor editor;


    editor.Init();
    editor.Run();
    editor.Destroy();
    
    return 0;
}
