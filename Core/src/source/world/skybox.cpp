﻿#include "world/skybox.hpp"

#include "app.hpp"

#include "resources/resource_manager.hpp"

using namespace PC_CORE;


void Skybox::Load(std::array<std::string, 6>& _maps)
{
    if (skyboxType == SkyBoxType::Procedural)
    {
        texture = ResourceManager::Get<Texture>("skydom.png");
    }
    else
    {
        texture = new Texture();
        texture->Load(_maps);

    }
    
    //App::instance->renderer.skyboxRender.CreateSkyboxBackend(*this);
}

void Skybox::Destroy()
{
    delete texture;
}
