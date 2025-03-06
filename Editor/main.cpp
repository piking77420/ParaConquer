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

template <class Tag>
struct stowed
{
    static typename Tag::type value;
};
template <class Tag>
typename Tag::type stowed<Tag>::value;

// Generate a static data member whose constructor initializes
// stowed<Tag>::value.  This type will only be named in an explicit
// instantiation, where it is legal to pass the address of a private
// member.
template <class Tag, typename Tag::type x>
struct stow_private
{
    stow_private() { stowed<Tag>::value = x; }
    static stow_private instance;
};
template <class Tag, typename Tag::type x>
stow_private<Tag, x> stow_private<Tag, x>::instance;
// ------- Usage -------
// A demonstration of how to use the library, with explanation

struct A
{
    A() : x("proof!") {}
private:
    char const* x;

    int b = 2;
};

// A tag type for A::x.Each distinct private member you need to
// access should have its own tag.  Each tag should contain a
// nested ::type that is the corresponding pointer-to-member type.
struct A_x { typedef char const* (A::* type); };

// Explicit instantiation; the only place where it is legal to pass
// the address of a private member.  Generates the static ::instance
// that in turn initializes stowed<Tag>::value.
template class stow_private<A_x, &A::x>;


struct A_b { typedef int (A::* type); };


template class stow_private<A_b, &A::b>;

#define Delctype()


int main(int argc, char* argv[])
{
    A a;
    std::cout <<  a.*stowed<A_x>::value << std::endl;
    std::cout << a.*stowed<A_b>::value << std::endl;
    std::cout << typeid(decltype(stowed<A_x>::value)).name() << std::endl;


#ifdef _DEBUG
    RelfectionHashingTest();
#endif

    bool appShounldClose = false;


    while (!appShounldClose)
    {
        Editor editor;

        editor.Init();
        editor.Run(&appShounldClose);
        editor.Destroy();
    }

  


    return 0;
}
