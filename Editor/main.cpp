#include <iostream>
#include "core_header.hpp"
#include "app.hpp"
#include "editor.hpp"

#include "world/transform.hpp"
#include "scripting/scripting_lua.hpp"
#include "reflection/reflector.hpp"
#include "ecs/component.h"
#include "world/transform.hpp"
#include "physics/sphere_collider.hpp"

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
