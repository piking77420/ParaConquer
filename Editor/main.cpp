#include "editor.hpp"

#include "reflection/reflector.hpp"
#include "rendering/material.hpp"
#include "resources/resource.hpp"

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;
using namespace Tbx;

#ifdef _DEBUG

constexpr TypeId NameSpaceS = COMPILE_TIME_TYPE_KEY(std::string);

void RelfectionHashingTest()
{
    using namespace std;
    static_assert(COMPILE_TIME_TYPE_KEY(string) == NameSpaceS);
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3f) != COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3d) == COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));
    static_assert(COMPILE_TIME_TYPE_KEY(PC_CORE::Resource) != COMPILE_TIME_TYPE_KEY(PC_CORE::Material));
    static_assert(COMPILE_TIME_TYPE_KEY(Tbx::Vector3f) != COMPILE_TIME_TYPE_KEY(Tbx::Vector3d));

    static_assert(COMPILE_TIME_TYPE_KEY(float_t) == COMPILE_TIME_TYPE_KEY(float));
    static_assert(COMPILE_TIME_TYPE_KEY(double_t) == COMPILE_TIME_TYPE_KEY(double));

    using d = uint32_t;
    static_assert(COMPILE_TIME_TYPE_KEY(d) == COMPILE_TIME_TYPE_KEY(unsigned int));

}

#endif


int main(int argc, char* argv[])
{
#ifdef _DEBUG
    RelfectionHashingTest();
#endif
    Editor editor;

    editor.Init();
    editor.Run();
    editor.Destroy();


    return 0;
}
