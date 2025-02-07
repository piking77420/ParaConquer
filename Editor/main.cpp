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

template <typename d , typename b>
void Register()
{
    constexpr auto db = COMPILE_TIME_TYPE_KEY(b);
    constexpr auto dKey = COMPILE_TIME_TYPE_KEY(d);

    static_assert(db != dKey);
}

class A : public Component
{
    char     dsffdsfd;
};

int main(int argc, char* argv[])
{

    Editor editor;

    editor.Init();
    editor.Run();
    editor.Destroy();


    return 0;
}
