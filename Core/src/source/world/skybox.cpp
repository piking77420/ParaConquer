#include "world/skybox.hpp"

#include "app.hpp"

#include "resources/resource_manager.hpp"

using namespace PC_CORE;


void Skybox::Load(std::array<std::string, 6>& _maps)
{
    /*
    if (skyboxType == SkyBoxType::Procedural)
    {
        texture = ResourceManager::Get<Texture2D>("skydom.png");
    }
    else
    {
        texture = new Texture2D();
        texture->Load(_maps);

    }*/
    
    //App::instance->renderer.skyboxRender.CreateSkyboxBackend(*this);
}

void Skybox::Destroy()
{
    //delete texture;
}
