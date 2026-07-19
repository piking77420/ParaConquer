#ifndef ASSIMP_HELPER_H
#define ASSIMP_HELPER_H


#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


static inline std::string_view AssimpTextureTypeToString(aiTextureType aiTextureType)
{
    switch (aiTextureType)
    {
    case aiTextureType_NONE:
        break;
    case aiTextureType_DIFFUSE:
        return "Diffuse"sv;
    case aiTextureType_SPECULAR:
        return "Specular"sv;
    case aiTextureType_AMBIENT:
        return "Ambient"sv;
    case aiTextureType_EMISSIVE:
        return "Ambient"sv;
    case aiTextureType_HEIGHT:
        return "Height"sv;
    case aiTextureType_NORMALS:
        return "Normals"sv;
    case aiTextureType_SHININESS:
        return "Shininess"sv;
    case aiTextureType_OPACITY:
        return "Opacity"sv;
    case aiTextureType_DISPLACEMENT:
        return "Displacement"sv;
    case aiTextureType_LIGHTMAP:
        return "LightMap"sv;
    case aiTextureType_REFLECTION:
        return "Reflection"sv;
    case aiTextureType_BASE_COLOR:
        return "Ambient"sv;
    case aiTextureType_NORMAL_CAMERA:
        return "Color"sv;
    case aiTextureType_EMISSION_COLOR:
        return "EmissionColor"sv;
    case aiTextureType_METALNESS:
        return "Metalness"sv;
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "Roughness"sv;
    case aiTextureType_AMBIENT_OCCLUSION:
        return "Ao"sv;
    case aiTextureType_UNKNOWN:
        return "UNKNOW"sv;
    case aiTextureType_SHEEN:
        return "Sheen"sv;
    case aiTextureType_CLEARCOAT:
        return "ClearnCoat"sv;
    case aiTextureType_TRANSMISSION:
        return "Transmission"sv;
    case aiTextureType_MAYA_BASE:
        return "MayaBase"sv;
    case aiTextureType_MAYA_SPECULAR:
        return "MayaSpecular"sv;
    case aiTextureType_MAYA_SPECULAR_COLOR:
        return "MayaSpecularColor"sv;
    case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
        return "MayaSpecularRougness"sv;
    case aiTextureType_ANISOTROPY:
        return "Anisotropy"sv;
    case aiTextureType_GLTF_METALLIC_ROUGHNESS:
        return "Mettalic_roughness"sv;
    case _aiTextureType_Force32Bit:
        return "Force32bit"sv;
    default:
        break;
    }

    return "?";
}


#endif