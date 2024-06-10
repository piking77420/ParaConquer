
#include "app.hpp"

int main(int argc, char* argv[])
{
    PC_CORE::App app;
    app.Init();
    app.Run();
    app.Destroy();
    return 0;
}
